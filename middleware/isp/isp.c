#include "isp/isp.h"
#include <assert.h>

static const isp_ops_t *g_ops = NULL;

void isp_register(const isp_ops_t *ops)    { assert(ops); g_ops = ops; }
int  isp_init(const isp_config_t *cfg)     { assert(g_ops); return g_ops->init(cfg); }
int  isp_start(void)                        { assert(g_ops); return g_ops->start(); }
void isp_stop(void)                         { assert(g_ops); g_ops->stop(); }
void isp_deinit(void)                       { assert(g_ops); g_ops->deinit(); }
