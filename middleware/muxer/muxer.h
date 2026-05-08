#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    int      width;
    int      height;
    int      fps;
    int      is_h265;   /* 0 = H.264, 1 = H.265 */
} muxer_config_t;

typedef struct {
    const void *data;
    size_t      size;
    int         is_keyframe;
    uint64_t    pts_us;
} muxer_packet_t;

typedef struct {
    int  (*open)(const char *filepath, const muxer_config_t *cfg);
    int  (*write_packet)(const muxer_packet_t *pkt);
    int  (*close)(void);
} muxer_ops_t;

void muxer_register(const muxer_ops_t *ops);

int  muxer_open(const char *filepath, const muxer_config_t *cfg);
int  muxer_write_packet(const muxer_packet_t *pkt);
int  muxer_close(void);
