#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

#define LOG_DIR "/root/camera_log"

static FILE           *g_log_file  = NULL;
static log_level_t     g_min_level = LOG_INFO;
static pthread_mutex_t g_mutex     = PTHREAD_MUTEX_INITIALIZER;

static const char * const g_tag[] = { "D", "I", "E" };

void log_init(void) {
    mkdir(LOG_DIR, 0755);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char path[64];
    strftime(path, sizeof(path), LOG_DIR "/%Y%m%d_%H%M%S.log", t);

    g_log_file = fopen(path, "w");
}

void log_deinit(void) {
    if (g_log_file) {
        fflush(g_log_file);
        fclose(g_log_file);
        g_log_file = NULL;
    }
}

void log_set_level(log_level_t level) {
    g_min_level = level;
}

void log_write(log_level_t level, const char *file, int line, const char *fmt, ...) {
    if (level < g_min_level)
        return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char ts[12];
    strftime(ts, sizeof(ts), "%H:%M:%S", t);

    if (file) {
        const char *slash = strrchr(file, '/');
        if (slash) file = slash + 1;
    }

    pthread_mutex_lock(&g_mutex);

    va_list args;

    va_start(args, fmt);
    if (file)
        fprintf(stderr, "%s [%s] %s:%d ", ts, g_tag[level], file, line);
    else
        fprintf(stderr, "%s [%s] ", ts, g_tag[level]);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);

    if (g_log_file) {
        va_start(args, fmt);
        if (file)
            fprintf(g_log_file, "%s [%s] %s:%d ", ts, g_tag[level], file, line);
        else
            fprintf(g_log_file, "%s [%s] ", ts, g_tag[level]);
        vfprintf(g_log_file, fmt, args);
        fputc('\n', g_log_file);
        fflush(g_log_file);
        va_end(args);
    }

    pthread_mutex_unlock(&g_mutex);
}
