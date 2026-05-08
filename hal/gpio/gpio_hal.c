#include "gpio/gpio_hal.h"
#include <assert.h>

static const gpio_ops_t *g_ops = NULL;

void gpio_register(const gpio_ops_t *ops)       { assert(ops); g_ops = ops; }
int  gpio_init(int chip, int line, int ms)       { assert(g_ops); return g_ops->init(chip, line, ms); }
void gpio_set_callback(gpio_callback_t cb)       { assert(g_ops); g_ops->set_callback(cb); }
int  gpio_start_listen(void)                     { assert(g_ops); return g_ops->start_listen(); }
void gpio_stop_listen(void)                      { assert(g_ops); g_ops->stop_listen(); }
void gpio_deinit(void)                           { assert(g_ops); g_ops->deinit(); }
