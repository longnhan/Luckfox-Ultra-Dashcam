#pragma once

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO  = 1,
    LOG_ERROR = 2,
} log_level_t;

/* Call once before any threads start; creates /root/camera_log/<timestamp>.log */
void log_init(void);
void log_deinit(void);
void log_set_level(log_level_t level);

void log_write(log_level_t level, const char *file, int line, const char *fmt, ...);

/* DEBUG and ERROR include file:line; INFO is clean */
#define LOG_D(fmt, ...) log_write(LOG_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) log_write(LOG_INFO,  NULL,     0,        fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) log_write(LOG_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
