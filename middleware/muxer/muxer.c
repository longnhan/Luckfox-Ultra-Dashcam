#include "muxer/muxer.h"
#include <assert.h>
#include <stddef.h>

static const muxer_ops_t *g_ops = NULL;

void muxer_register(const muxer_ops_t *ops)            { assert(ops); g_ops = ops; }
int  muxer_open(const char *p, const muxer_config_t *c){ assert(g_ops); return g_ops->open(p, c); }
int  muxer_write_packet(const muxer_packet_t *pkt)     { assert(g_ops); return g_ops->write_packet(pkt); }
int  muxer_close(void)                                  { assert(g_ops); return g_ops->close(); }
