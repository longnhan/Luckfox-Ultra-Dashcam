#include "state_machine.h"
#include "config.h"
#include "muxer/muxer.h"
#include "rtsp/rtsp.h"
#include "storage/storage_hal.h"
#include "log.h"
#include <time.h>

static app_state_t g_state = STATE_IDLE;

static void start_recording(void) {
    char path[256];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    snprintf(path, sizeof(path), CFG_RECORD_DIR "/%04d%02d%02d_%02d%02d%02d.mp4",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
             tm->tm_hour, tm->tm_min, tm->tm_sec);

    muxer_config_t mcfg = {
        .width    = CFG_WIDTH,
        .height   = CFG_HEIGHT,
        .fps      = CFG_FPS,
        .is_h265  = CFG_CODEC_H265,
    };
    if (muxer_open(path, &mcfg) < 0) {
        LOG_E("[sm] failed to open muxer");
        return;
    }
    LOG_I("[sm] recording → %s", path);
}

static void stop_recording(void) {
    muxer_close();
    LOG_I("[sm] recording stopped");
}

static void start_streaming(void) {
    rtsp_start();
    LOG_I("[sm] RTSP stream live at rtsp://<wifi_ip>:%d/%s",
          CFG_RTSP_PORT, CFG_RTSP_STREAM);
}

static void stop_streaming(void) {
    rtsp_stop();
    LOG_I("[sm] RTSP stopped");
}

void sm_init(void) {
    g_state = STATE_IDLE;
    LOG_I("[sm] idle");
}

/* Temporary mode: streaming is always on; button only controls recording. */
void sm_handle_event(app_event_t event) {
    switch (g_state) {

    case STATE_IDLE:
        if (event == EVENT_BUTTON_SHORT_PRESS || event == EVENT_BUTTON_LONG_PRESS) {
            start_recording();
            g_state = STATE_RECORDING;
        }
        break;

    case STATE_RECORDING:
        if (event == EVENT_BUTTON_SHORT_PRESS || event == EVENT_BUTTON_LONG_PRESS ||
            event == EVENT_STORAGE_FULL || event == EVENT_ERROR) {
            stop_recording();
            g_state = STATE_IDLE;
        }
        break;

    case STATE_STREAMING:
    case STATE_RECORDING_AND_STREAMING:
    case STATE_SHUTDOWN:
        break;
    }
}

app_state_t sm_get_state(void) {
    return g_state;
}
