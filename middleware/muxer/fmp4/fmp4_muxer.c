#include "fmp4_muxer.h"
#include "log.h"
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

static AVFormatContext *g_fmt_ctx  = NULL;
static AVStream        *g_stream   = NULL;
static int64_t          g_pkt_idx  = 0;

static int fmp4_open(const char *filepath, const muxer_config_t *cfg) {
    avformat_alloc_output_context2(&g_fmt_ctx, NULL, "mp4", filepath);
    if (!g_fmt_ctx) return -1;

    /* Force fragmented MP4: moov atom written per fragment, not at EOF.
     * This makes files recoverable after sudden power loss. */
    av_opt_set(g_fmt_ctx->priv_data, "movflags",
               "frag_keyframe+empty_moov+default_base_moof", 0);

    enum AVCodecID codec_id = cfg->is_h265 ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;
    g_stream = avformat_new_stream(g_fmt_ctx, NULL);
    if (!g_stream) goto fail;

    g_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    g_stream->codecpar->codec_id   = codec_id;
    g_stream->codecpar->width      = cfg->width;
    g_stream->codecpar->height     = cfg->height;
    g_stream->time_base            = (AVRational){1, 1000000}; /* microseconds */

    if (!(g_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if (avio_open(&g_fmt_ctx->pb, filepath, AVIO_FLAG_WRITE) < 0) goto fail;
    }

    if (avformat_write_header(g_fmt_ctx, NULL) < 0) goto fail;

    g_pkt_idx = 0;
    LOG_I("[fmp4] opened %s (%s %dx%d @%dfps)",
          filepath, cfg->is_h265 ? "H.265" : "H.264",
          cfg->width, cfg->height, cfg->fps);
    return 0;

fail:
    avformat_free_context(g_fmt_ctx);
    g_fmt_ctx = NULL;
    return -1;
}

static int fmp4_write_packet(const muxer_packet_t *pkt) {
    if (!g_fmt_ctx) return -1;

    AVPacket *av_pkt = av_packet_alloc();
    av_packet_from_data(av_pkt, (uint8_t *)pkt->data, (int)pkt->size);
    av_pkt->stream_index = g_stream->index;
    av_pkt->pts          = pkt->pts_us;
    av_pkt->dts          = pkt->pts_us;
    av_pkt->pos          = -1;
    if (pkt->is_keyframe) av_pkt->flags |= AV_PKT_FLAG_KEY;

    int ret = av_interleaved_write_frame(g_fmt_ctx, av_pkt);
    av_packet_free(&av_pkt);
    g_pkt_idx++;
    return ret;
}

static int fmp4_close(void) {
    if (!g_fmt_ctx) return 0;
    av_write_trailer(g_fmt_ctx);
    if (!(g_fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&g_fmt_ctx->pb);
    avformat_free_context(g_fmt_ctx);
    g_fmt_ctx = NULL;
    LOG_I("[fmp4] closed (%lld packets)", (long long)g_pkt_idx);
    return 0;
}

static const muxer_ops_t g_fmp4_ops = {
    .open         = fmp4_open,
    .write_packet = fmp4_write_packet,
    .close        = fmp4_close,
};

const muxer_ops_t *fmp4_get_ops(void) {
    return &g_fmp4_ops;
}
