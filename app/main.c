#include "config.h"
#include "state_machine.h"

/* HAL concrete implementations */
#include "camera/mis5001/mis5001_camera.h"
#include "encoder/rkmpp/rkmpp_encoder.h"
#include "storage/sdcard/sdcard_storage.h"
#include "gpio/libgpiod/gpiod_button.h"

/* Middleware concrete implementations */
#include "pipeline/rkmpi/rkmpi_pipeline.h"
#include "isp/rkaiq/rkaiq_isp.h"
#include "muxer/fmp4/fmp4_muxer.h"
#include "rtsp/live555/live555_rtsp.h"

/* HAL abstract interfaces */
#include "camera/camera_hal.h"
#include "encoder/encoder_hal.h"
#include "storage/storage_hal.h"
#include "gpio/gpio_hal.h"

/* Middleware abstract interfaces */
#include "pipeline/pipeline.h"
#include "isp/isp.h"
#include "muxer/muxer.h"
#include "rtsp/rtsp.h"

#include "log.h"
#include <signal.h>
#include <unistd.h>

static volatile int g_quit = 0;

static void on_signal(int sig) {
    (void)sig;
    g_quit = 1;
}

static void on_button(gpio_event_t event) {
    if (event == GPIO_EVENT_SHORT_PRESS)
        sm_handle_event(EVENT_BUTTON_SHORT_PRESS);
    else
        sm_handle_event(EVENT_BUTTON_LONG_PRESS);
}

int main(void) {
    log_init();
    signal(SIGINT,  on_signal);
    signal(SIGTERM, on_signal);

    /* --- Wire concrete implementations to abstract interfaces --- */
    cam_register(mis5001_get_ops());
    enc_register(rkmpp_get_ops());
    storage_register(sdcard_get_ops());
    gpio_register(gpiod_get_ops());
    pipeline_register(rkmpi_pipeline_get_ops());
    isp_register(rkaiq_get_ops());
    muxer_register(fmp4_get_ops());
    rtsp_register(live555_get_ops());

    /* --- Storage --- */
    if (storage_mount(CFG_SD_DEVICE, CFG_SD_MOUNTPOINT) < 0) {
        LOG_E("No SD card — recording disabled");
    }

    /* --- ISP --- */
    isp_config_t isp_cfg = {
        .iqbin_path = CFG_IQBIN_PATH,
        .width      = CFG_WIDTH,
        .height     = CFG_HEIGHT,
        .fps        = CFG_FPS,
    };
    isp_init(&isp_cfg);
    isp_start();

    /* --- Camera --- */
    cam_config_t cam_cfg = {
        .width      = CFG_WIDTH,
        .height     = CFG_HEIGHT,
        .fps        = CFG_FPS,
        .iqbin_path = CFG_IQBIN_PATH,
    };
    cam_init(&cam_cfg);
    cam_start();

    /* --- Encoder --- */
    enc_config_t enc_cfg = {
        .codec        = CFG_CODEC_H265 ? ENC_CODEC_H265 : ENC_CODEC_H264,
        .width        = CFG_WIDTH,
        .height       = CFG_HEIGHT,
        .fps          = CFG_FPS,
        .bitrate_kbps = CFG_BITRATE_KBPS,
    };
    enc_init(&enc_cfg);

    /* --- RK_MPI pipeline: bind VI → VENC (zero-copy) --- */
    pipeline_config_t pipe_cfg = {
        .vi_pipe  = CFG_VI_PIPE,
        .vi_chn   = CFG_VI_CHN,
        .venc_chn = CFG_VENC_CHN,
    };
    pipeline_init(&pipe_cfg);
    pipeline_bind();

    /* --- RTSP server --- */
    rtsp_config_t rtsp_cfg = {
        .port        = CFG_RTSP_PORT,
        .stream_name = CFG_RTSP_STREAM,
        .width       = CFG_WIDTH,
        .height      = CFG_HEIGHT,
        .fps         = CFG_FPS,
        .is_h265     = CFG_CODEC_H265,
    };
    rtsp_init(&rtsp_cfg);

    /* --- GPIO button --- */
    gpio_init(CFG_GPIO_CHIP, CFG_GPIO_LINE, CFG_LONG_PRESS_MS);
    gpio_set_callback(on_button);
    gpio_start_listen();

    /* --- App state machine --- */
    sm_init();
    rtsp_start();   /* stream always on; button only controls recording */
    LOG_I("Bodycam ready. Press button to record. Streaming on rtsp://<wifi_ip>:%d/%s",
          CFG_RTSP_PORT, CFG_RTSP_STREAM);

    /* --- Main encode/mux loop --- */
    while (!g_quit) {
        cam_frame_t frame;
        if (cam_get_frame(&frame) < 0) { usleep(1000); continue; }

        enc_packet_t pkt;
        if (enc_encode(&frame, &pkt) == 0 && pkt.size > 0) {
            app_state_t state = sm_get_state();

            if (state == STATE_RECORDING || state == STATE_RECORDING_AND_STREAMING) {
                muxer_packet_t mp = {
                    .data        = pkt.data,
                    .size        = pkt.size,
                    .is_keyframe = pkt.is_keyframe,
                    .pts_us      = pkt.timestamp_us,
                };
                muxer_write_packet(&mp);

                /* Check SD card space periodically on keyframes */
                if (pkt.is_keyframe) {
                    uint64_t free_bytes;
                    if (storage_get_free_bytes(&free_bytes) == 0 &&
                        free_bytes < (uint64_t)CFG_MIN_FREE_MB * 1024 * 1024) {
                        sm_handle_event(EVENT_STORAGE_FULL);
                    }
                }
            }

            rtsp_push_packet(pkt.data, pkt.size, pkt.timestamp_us, pkt.is_keyframe);

            enc_release_packet(&pkt);
        }
        cam_release_frame(&frame);
    }

    /* --- Teardown --- */
    gpio_stop_listen();
    gpio_deinit();
    pipeline_unbind();
    pipeline_deinit();
    rtsp_stop();
    rtsp_deinit();
    muxer_close();
    enc_stop();
    enc_deinit();
    cam_stop();
    cam_deinit();
    isp_stop();
    isp_deinit();
    storage_unmount();

    LOG_I("Bodycam shutdown complete.");
    log_deinit();
    return 0;
}
