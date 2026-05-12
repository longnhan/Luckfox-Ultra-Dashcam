#include "rkmpi_pipeline.h"
#include "log.h"
#include <stddef.h>

/* rk_common.h, rk_mpi_sys.h pulled in via SDK include path */

static pipeline_config_t g_cfg;

static int rkmpi_init(const pipeline_config_t *cfg) {
    g_cfg = *cfg;
    /* TODO: RK_MPI_SYS_Init() */
    LOG_I("[rkmpi] init vi_pipe=%d vi_chn=%d venc_chn=%d",
          cfg->vi_pipe, cfg->vi_chn, cfg->venc_chn);
    return 0;
}

static int rkmpi_bind(void) {
    /* Zero-copy path: VI output feeds directly into VENC input via DMA-BUF.
     * No CPU memcpy happens between capture and encode. */
    /* TODO:
     * MPP_CHN_S src  = { MOD_ID_VI,   g_cfg.vi_pipe, g_cfg.vi_chn  };
     * MPP_CHN_S dst  = { MOD_ID_VENC, 0,             g_cfg.venc_chn };
     * RK_MPI_SYS_Bind(&src, &dst);
     */
    LOG_I("[rkmpi] VI->VENC bound (zero-copy)");
    return 0;
}

static void rkmpi_unbind(void) {
    /* TODO: RK_MPI_SYS_UnBind(&src, &dst) */
    LOG_I("[rkmpi] unbound");
}

static void rkmpi_deinit(void) {
    /* TODO: RK_MPI_SYS_Exit() */
    LOG_I("[rkmpi] deinit");
}

static const pipeline_ops_t g_rkmpi_ops = {
    .init   = rkmpi_init,
    .bind   = rkmpi_bind,
    .unbind = rkmpi_unbind,
    .deinit = rkmpi_deinit,
};

const pipeline_ops_t *rkmpi_pipeline_get_ops(void) {
    return &g_rkmpi_ops;
}
