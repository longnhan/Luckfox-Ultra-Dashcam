#include "gpiod_button.h"
#include <gpiod.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

static struct gpiod_chip   *g_chip     = NULL;
static struct gpiod_line   *g_line     = NULL;
static gpio_callback_t      g_cb       = NULL;
static int                  g_long_ms  = 1500;
static pthread_t            g_thread;
static volatile int         g_running  = 0;

static void *listen_thread(void *arg) {
    (void)arg;
    struct timespec press_time;
    while (g_running) {
        struct gpiod_line_event ev;
        if (gpiod_line_event_wait(g_line, NULL) < 0) break;
        if (gpiod_line_event_read(g_line, &ev) < 0) break;

        if (ev.event_type == GPIOD_LINE_EVENT_FALLING_EDGE) {
            clock_gettime(CLOCK_MONOTONIC, &press_time);
        } else if (ev.event_type == GPIOD_LINE_EVENT_RISING_EDGE) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            long held_ms = (now.tv_sec  - press_time.tv_sec)  * 1000
                         + (now.tv_nsec - press_time.tv_nsec) / 1000000;
            if (g_cb) {
                g_cb(held_ms >= g_long_ms ? GPIO_EVENT_LONG_PRESS
                                           : GPIO_EVENT_SHORT_PRESS);
            }
        }
    }
    return NULL;
}

static int gpiod_init(int chip, int line, int long_press_ms) {
    g_long_ms = long_press_ms;
    g_chip = gpiod_chip_open_by_number(chip);
    if (!g_chip) { perror("[gpiod] open chip"); return -1; }
    g_line = gpiod_chip_get_line(g_chip, line);
    if (!g_line) { perror("[gpiod] get line"); return -1; }
    if (gpiod_line_request_both_edges_events(g_line, "bodycam") < 0) {
        perror("[gpiod] request events");
        return -1;
    }
    printf("[gpiod] watching chip%d line%d (long_press=%dms)\n", chip, line, long_press_ms);
    return 0;
}

static void gpiod_set_callback(gpio_callback_t cb) { g_cb = cb; }

static int gpiod_start_listen(void) {
    g_running = 1;
    return pthread_create(&g_thread, NULL, listen_thread, NULL);
}

static void gpiod_stop_listen(void) {
    g_running = 0;
    pthread_join(g_thread, NULL);
}

static void gpiod_deinit(void) {
    if (g_line) gpiod_line_release(g_line);
    if (g_chip) gpiod_chip_close(g_chip);
    g_line = NULL;
    g_chip = NULL;
}

static const gpio_ops_t g_gpiod_ops = {
    .init         = gpiod_init,
    .set_callback = gpiod_set_callback,
    .start_listen = gpiod_start_listen,
    .stop_listen  = gpiod_stop_listen,
    .deinit       = gpiod_deinit,
};

const gpio_ops_t *gpiod_get_ops(void) {
    return &g_gpiod_ops;
}
