#include "live555_rtsp.h"
#include "log.h"
#include <pthread.h>

/* live555 headers (liveMedia, groupsock, UsageEnvironment, BasicUsageEnvironment)
 * must be present in your cross-compilation sysroot or built from source. */

static rtsp_config_t g_cfg;
static pthread_t     g_thread;
static volatile int  g_running = 0;

static void *rtsp_thread(void *arg) {
    (void)arg;
    /* TODO:
     *   TaskScheduler      *scheduler = BasicTaskScheduler::createNew();
     *   UsageEnvironment   *env       = BasicUsageEnvironment::createNew(*scheduler);
     *   RTSPServer         *server    = RTSPServer::createNew(*env, g_cfg.port);
     *   ServerMediaSession *sms       = ServerMediaSession::createNew(...);
     *   H264or5VideoStreamFramer, RTPSink, etc.
     *   env->taskScheduler().doEventLoop(&g_running);
     */
    LOG_I("[live555] RTSP event loop started on port %d", g_cfg.port);
    while (g_running) { /* event loop placeholder */ }
    return NULL;
}

static int live555_init(const rtsp_config_t *cfg) {
    g_cfg = *cfg;
    LOG_I("[live555] init rtsp://0.0.0.0:%d/%s", cfg->port, cfg->stream_name);
    return 0;
}

static int live555_start(void) {
    g_running = 1;
    return pthread_create(&g_thread, NULL, rtsp_thread, NULL);
}

static int live555_push_packet(const void *data, size_t size, uint64_t pts_us, int is_keyframe) {
    /* TODO: signal the framer with new NAL data via a thread-safe queue */
    (void)data; (void)size; (void)pts_us; (void)is_keyframe;
    return 0;
}

static void live555_stop(void) {
    g_running = 0;
    pthread_join(g_thread, NULL);
    LOG_I("[live555] stopped");
}

static void live555_deinit(void) {
    LOG_I("[live555] deinit");
}

static const rtsp_ops_t g_live555_ops = {
    .init        = live555_init,
    .start       = live555_start,
    .push_packet = live555_push_packet,
    .stop        = live555_stop,
    .deinit      = live555_deinit,
};

const rtsp_ops_t *live555_get_ops(void) {
    return &g_live555_ops;
}
