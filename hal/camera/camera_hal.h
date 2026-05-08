#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    void    *data;
    size_t   size;
    uint64_t timestamp_us;
    int      width;
    int      height;
    void    *priv;  /* implementation-specific handle (e.g. RK_MPI frame) */
} cam_frame_t;

typedef struct {
    int width;
    int height;
    int fps;
    const char *iqbin_path;
} cam_config_t;

typedef struct {
    int  (*init)(const cam_config_t *cfg);
    int  (*start)(void);
    int  (*get_frame)(cam_frame_t *frame);
    void (*release_frame)(cam_frame_t *frame);
    int  (*stop)(void);
    void (*deinit)(void);
} cam_ops_t;

/* Register the concrete implementation before calling any cam_* function. */
void cam_register(const cam_ops_t *ops);

int  cam_init(const cam_config_t *cfg);
int  cam_start(void);
int  cam_get_frame(cam_frame_t *frame);
void cam_release_frame(cam_frame_t *frame);
int  cam_stop(void);
void cam_deinit(void);
