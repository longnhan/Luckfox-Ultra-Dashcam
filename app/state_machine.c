#include "state_machine.h"
#include "config.h"
#include "muxer/muxer.h"
#include "rtsp/rtsp.h"
#include "storage/storage_hal.h"
#include <stdio.h>
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
        fprintf(stderr, "[sm] failed to open muxer\n");
        return;
    }
    printf("[sm] recording → %s\n", path);
}

static void stop_recording(void) {
    muxer_close();
    printf("[sm] recording stopped\n");
}

static void start_streaming(void) {
    rtsp_start();
    printf("[sm] RTSP stream live at rtsp://<wifi_ip>:%d/%s\n",
           CFG_RTSP_PORT, CFG_RTSP_STREAM);
}

static void stop_streaming(void) {
    rtsp_stop();
    printf("[sm] RTSP stopped\n");
}

void sm_init(void) {
    g_state = STATE_IDLE;
    printf("[sm] idle\n");
}

void sm_handle_event(app_event_t event) {
    switch (g_state) {

    case STATE_IDLE:
        if (event == EVENT_BUTTON_SHORT_PRESS) {
            start_recording();
            g_state = STATE_RECORDING;
        } else if (event == EVENT_BUTTON_LONG_PRESS) {
            start_streaming();
            g_state = STATE_STREAMING;
        }
        break;

    case STATE_RECORDING:
        if (event == EVENT_BUTTON_SHORT_PRESS) {
            stop_recording();
            g_state = STATE_IDLE;
        } else if (event == EVENT_BUTTON_LONG_PRESS) {
            start_streaming();
            g_state = STATE_RECORDING_AND_STREAMING;
        } else if (event == EVENT_STORAGE_FULL || event == EVENT_ERROR) {
            stop_recording();
            g_state = STATE_IDLE;
        }
        break;

    case STATE_STREAMING:
        if (event == EVENT_BUTTON_SHORT_PRESS) {
            start_recording();
            g_state = STATE_RECORDING_AND_STREAMING;
        } else if (event == EVENT_BUTTON_LONG_PRESS) {
            stop_streaming();
            g_state = STATE_IDLE;
        }
        break;

    case STATE_RECORDING_AND_STREAMING:
        if (event == EVENT_BUTTON_SHORT_PRESS) {
            stop_recording();
            g_state = STATE_STREAMING;
        } else if (event == EVENT_BUTTON_LONG_PRESS) {
            stop_recording();
            stop_streaming();
            g_state = STATE_IDLE;
        } else if (event == EVENT_STORAGE_FULL || event == EVENT_ERROR) {
            stop_recording();
            g_state = STATE_STREAMING;
        }
        break;

    case STATE_SHUTDOWN:
        break;
    }
}

app_state_t sm_get_state(void) {
    return g_state;
}
