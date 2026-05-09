#include "pipeline/pipeline.h"
#include <assert.h>
#include <stddef.h>

static const pipeline_ops_t *g_ops = NULL;

void pipeline_register(const pipeline_ops_t *ops) { assert(ops); g_ops = ops; }
int  pipeline_init(const pipeline_config_t *cfg)  { assert(g_ops); return g_ops->init(cfg); }
int  pipeline_bind(void)                           { assert(g_ops); return g_ops->bind(); }
void pipeline_unbind(void)                         { assert(g_ops); g_ops->unbind(); }
void pipeline_deinit(void)                         { assert(g_ops); g_ops->deinit(); }
