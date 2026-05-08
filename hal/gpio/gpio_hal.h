#pragma once

typedef enum {
    GPIO_EVENT_SHORT_PRESS,
    GPIO_EVENT_LONG_PRESS,
} gpio_event_t;

typedef void (*gpio_callback_t)(gpio_event_t event);

typedef struct {
    int  (*init)(int chip, int line, int long_press_ms);
    void (*set_callback)(gpio_callback_t cb);
    int  (*start_listen)(void);   /* blocks or spawns a thread */
    void (*stop_listen)(void);
    void (*deinit)(void);
} gpio_ops_t;

void gpio_register(const gpio_ops_t *ops);

int  gpio_init(int chip, int line, int long_press_ms);
void gpio_set_callback(gpio_callback_t cb);
int  gpio_start_listen(void);
void gpio_stop_listen(void);
void gpio_deinit(void);
