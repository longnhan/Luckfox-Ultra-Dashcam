#include "encoder/encoder_hal.h"
#include <assert.h>
#include <stddef.h>

static const enc_ops_t *g_ops = NULL;

void enc_register(const enc_ops_t *ops) { assert(ops); g_ops = ops; }

int  enc_init(const enc_config_t *cfg)             { assert(g_ops); return g_ops->init(cfg); }
int  enc_encode(const cam_frame_t *f, enc_packet_t *p) { assert(g_ops); return g_ops->encode(f, p); }
void enc_release_packet(enc_packet_t *p)           { assert(g_ops); g_ops->release_packet(p); }
int  enc_stop(void)                                { assert(g_ops); return g_ops->stop(); }
void enc_deinit(void)                              { assert(g_ops); g_ops->deinit(); }
