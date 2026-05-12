#include "mis5001_camera.h"
#include "log.h"

/* rk_mpi_sys.h, rk_mpi_vi.h pulled in via SDK include path */

static cam_config_t g_cfg;

static int mis5001_init(const cam_config_t *cfg) {
    g_cfg = *cfg;
    /* TODO: RK_MPI_SYS_Init(), VI channel open, bind ISP */
    LOG_I("[mis5001] init %dx%d @%dfps iqbin=%s",
          cfg->width, cfg->height, cfg->fps, cfg->iqbin_path);
    return 0;
}

static int mis5001_start(void) {
    /* TODO: RK_MPI_VI_EnableChn() */
    LOG_I("[mis5001] start");
    return 0;
}

static int mis5001_get_frame(cam_frame_t *frame) {
    /* TODO: RK_MPI_VI_GetChnFrame() — fills frame->priv with VIDEO_FRAME_INFO_S */
    (void)frame;
    return 0;
}

static void mis5001_release_frame(cam_frame_t *frame) {
    /* TODO: RK_MPI_VI_ReleaseChnFrame() */
    (void)frame;
}

static int mis5001_stop(void) {
    /* TODO: RK_MPI_VI_DisableChn() */
    LOG_I("[mis5001] stop");
    return 0;
}

static void mis5001_deinit(void) {
    /* TODO: RK_MPI_SYS_Exit() */
    LOG_I("[mis5001] deinit");
}

static const cam_ops_t g_mis5001_ops = {
    .init          = mis5001_init,
    .start         = mis5001_start,
    .get_frame     = mis5001_get_frame,
    .release_frame = mis5001_release_frame,
    .stop          = mis5001_stop,
    .deinit        = mis5001_deinit,
};

const cam_ops_t *mis5001_get_ops(void) {
    return &g_mis5001_ops;
}
