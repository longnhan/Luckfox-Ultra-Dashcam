#include "rkmpp_encoder.h"
#include <stdio.h>

/* rk_mpi_venc.h, rk_mpi_mb.h pulled in via SDK include path */

static enc_config_t g_cfg;

static int rkmpp_init(const enc_config_t *cfg) {
    g_cfg = *cfg;
    /* TODO: RK_MPI_VENC_CreateChn(), set RC params, H.264/H.265 profile */
    printf("[rkmpp] init codec=%d %dx%d @%dfps %dkbps\n",
           cfg->codec, cfg->width, cfg->height, cfg->fps, cfg->bitrate_kbps);
    return 0;
}

static int rkmpp_encode(const cam_frame_t *frame, enc_packet_t *pkt) {
    /* TODO: RK_MPI_VENC_SendFrame() then RK_MPI_VENC_GetStream()
     * The VI→VENC binding via RK_MPI_SYS_Bind() makes this zero-copy:
     * frames arrive in VENC output queue without a CPU memcpy. */
    (void)frame; (void)pkt;
    return 0;
}

static void rkmpp_release_packet(enc_packet_t *pkt) {
    /* TODO: RK_MPI_VENC_ReleaseStream() */
    (void)pkt;
}

static int rkmpp_stop(void) {
    /* TODO: RK_MPI_VENC_StopRecvFrame() */
    return 0;
}

static void rkmpp_deinit(void) {
    /* TODO: RK_MPI_VENC_DestroyChn() */
    printf("[rkmpp] deinit\n");
}

static const enc_ops_t g_rkmpp_ops = {
    .init           = rkmpp_init,
    .encode         = rkmpp_encode,
    .release_packet = rkmpp_release_packet,
    .stop           = rkmpp_stop,
    .deinit         = rkmpp_deinit,
};

const enc_ops_t *rkmpp_get_ops(void) {
    return &g_rkmpp_ops;
}
