#pragma once

typedef enum {
    STATE_IDLE,
    STATE_RECORDING,
    STATE_STREAMING,
    STATE_RECORDING_AND_STREAMING,
    STATE_SHUTDOWN,
} app_state_t;

typedef enum {
    EVENT_BUTTON_SHORT_PRESS,
    EVENT_BUTTON_LONG_PRESS,
    EVENT_STORAGE_FULL,
    EVENT_ERROR,
} app_event_t;

void sm_init(void);
void sm_handle_event(app_event_t event);
app_state_t sm_get_state(void);
