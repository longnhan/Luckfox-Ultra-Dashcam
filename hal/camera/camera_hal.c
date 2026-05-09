#include "camera/camera_hal.h"
#include <assert.h>
#include <stddef.h>

static const cam_ops_t *g_ops = NULL;

void cam_register(const cam_ops_t *ops) {
    assert(ops);
    g_ops = ops;
}

int  cam_init(const cam_config_t *cfg)        { assert(g_ops); return g_ops->init(cfg); }
int  cam_start(void)                           { assert(g_ops); return g_ops->start(); }
int  cam_get_frame(cam_frame_t *f)             { assert(g_ops); return g_ops->get_frame(f); }
void cam_release_frame(cam_frame_t *f)         { assert(g_ops); g_ops->release_frame(f); }
int  cam_stop(void)                            { assert(g_ops); return g_ops->stop(); }
void cam_deinit(void)                          { assert(g_ops); g_ops->deinit(); }
