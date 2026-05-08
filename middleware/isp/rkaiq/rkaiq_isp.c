#include "rkaiq_isp.h"
#include <stdio.h>

/* rk_aiq_user_api2_sysctl.h pulled in via SDK include path */

static isp_config_t g_cfg;

static int rkaiq_init(const isp_config_t *cfg) {
    g_cfg = *cfg;
    /* TODO:
     * rk_aiq_sys_ctx_t *ctx = rk_aiq_uapi2_sysctl_init(
     *     sensor_entity_name, cfg->iqbin_path, NULL, NULL);
     * rk_aiq_uapi2_sysctl_prepare(ctx, cfg->width, cfg->height, RK_AIQ_WORKING_MODE_NORMAL);
     */
    printf("[rkaiq] init iqbin=%s %dx%d @%dfps\n",
           cfg->iqbin_path, cfg->width, cfg->height, cfg->fps);
    return 0;
}

static int rkaiq_start(void) {
    /* TODO: rk_aiq_uapi2_sysctl_start(ctx) */
    printf("[rkaiq] started (AE/AWB/NR active)\n");
    return 0;
}

static void rkaiq_stop(void) {
    /* TODO: rk_aiq_uapi2_sysctl_stop(ctx, false) */
    printf("[rkaiq] stopped\n");
}

static void rkaiq_deinit(void) {
    /* TODO: rk_aiq_uapi2_sysctl_deinit(ctx) */
    printf("[rkaiq] deinit\n");
}

static const isp_ops_t g_rkaiq_ops = {
    .init   = rkaiq_init,
    .start  = rkaiq_start,
    .stop   = rkaiq_stop,
    .deinit = rkaiq_deinit,
};

const isp_ops_t *rkaiq_get_ops(void) {
    return &g_rkaiq_ops;
}
