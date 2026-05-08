#pragma once
#include <stdint.h>
#include <stddef.h>
#include "camera/camera_hal.h"

typedef enum {
    ENC_CODEC_H264,
    ENC_CODEC_H265,
} enc_codec_t;

typedef struct {
    enc_codec_t codec;
    int         width;
    int         height;
    int         fps;
    int         bitrate_kbps;
} enc_config_t;

typedef struct {
    void    *data;
    size_t   size;
    int      is_keyframe;
    uint64_t timestamp_us;
    void    *priv;
} enc_packet_t;

typedef struct {
    int  (*init)(const enc_config_t *cfg);
    int  (*encode)(const cam_frame_t *frame, enc_packet_t *pkt);
    void (*release_packet)(enc_packet_t *pkt);
    int  (*stop)(void);
    void (*deinit)(void);
} enc_ops_t;

void enc_register(const enc_ops_t *ops);

int  enc_init(const enc_config_t *cfg);
int  enc_encode(const cam_frame_t *frame, enc_packet_t *pkt);
void enc_release_packet(enc_packet_t *pkt);
int  enc_stop(void);
void enc_deinit(void);
