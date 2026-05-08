#include "rtsp/rtsp.h"
#include <assert.h>

static const rtsp_ops_t *g_ops = NULL;

void rtsp_register(const rtsp_ops_t *ops) { assert(ops); g_ops = ops; }
int  rtsp_init(const rtsp_config_t *cfg)  { assert(g_ops); return g_ops->init(cfg); }
int  rtsp_start(void)                      { assert(g_ops); return g_ops->start(); }
int  rtsp_push_packet(const void *d, size_t n, uint64_t pts, int key) {
    assert(g_ops); return g_ops->push_packet(d, n, pts, key);
}
void rtsp_stop(void)   { assert(g_ops); g_ops->stop(); }
void rtsp_deinit(void) { assert(g_ops); g_ops->deinit(); }
