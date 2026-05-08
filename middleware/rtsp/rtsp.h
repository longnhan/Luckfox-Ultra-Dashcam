#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    int         port;
    const char *stream_name;  /* rtsp://host:port/<stream_name> */
    int         width;
    int         height;
    int         fps;
    int         is_h265;
} rtsp_config_t;

typedef struct {
    int  (*init)(const rtsp_config_t *cfg);
    int  (*start)(void);
    int  (*push_packet)(const void *data, size_t size, uint64_t pts_us, int is_keyframe);
    void (*stop)(void);
    void (*deinit)(void);
} rtsp_ops_t;

void rtsp_register(const rtsp_ops_t *ops);

int  rtsp_init(const rtsp_config_t *cfg);
int  rtsp_start(void);
int  rtsp_push_packet(const void *data, size_t size, uint64_t pts_us, int is_keyframe);
void rtsp_stop(void);
void rtsp_deinit(void);
