#include "storage/storage_hal.h"
#include <assert.h>
#include <stddef.h>

static const storage_ops_t *g_ops = NULL;

void storage_register(const storage_ops_t *ops) { assert(ops); g_ops = ops; }

int  storage_mount(const char *dev, const char *mp) { assert(g_ops); return g_ops->mount(dev, mp); }
int  storage_open_file(const char *path)            { assert(g_ops); return g_ops->open_file(path); }
int  storage_write(const void *d, size_t n)         { assert(g_ops); return g_ops->write(d, n); }
int  storage_close_file(void)                       { assert(g_ops); return g_ops->close_file(); }
int  storage_get_free_bytes(uint64_t *out)          { assert(g_ops); return g_ops->get_free_bytes(out); }
void storage_unmount(void)                          { assert(g_ops); g_ops->unmount(); }
