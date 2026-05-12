#include "sdcard_storage.h"
#include "log.h"
#include <sys/mount.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static char  g_mountpoint[256];
static int   g_fd = -1;

static int sdcard_mount(const char *device, const char *mountpoint) {
    strncpy(g_mountpoint, mountpoint, sizeof(g_mountpoint) - 1);
    if (mount(device, mountpoint, "vfat", MS_NOATIME, NULL) < 0) {
        /* already mounted or exfat — try remount */
        if (errno != EBUSY) {
            LOG_E("[sdcard] mount: %s", strerror(errno));
            return -1;
        }
    }
    LOG_I("[sdcard] mounted %s -> %s", device, mountpoint);
    return 0;
}

static int sdcard_open_file(const char *path) {
    g_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (g_fd < 0) {
        LOG_E("[sdcard] open_file: %s", strerror(errno));
        return -1;
    }
    return 0;
}

static int sdcard_write(const void *data, size_t len) {
    ssize_t written = write(g_fd, data, len);
    return (written == (ssize_t)len) ? 0 : -1;
}

static int sdcard_close_file(void) {
    if (g_fd >= 0) {
        close(g_fd);
        g_fd = -1;
    }
    return 0;
}

static int sdcard_get_free_bytes(uint64_t *out) {
    struct statvfs st;
    if (statvfs(g_mountpoint, &st) < 0) {
        LOG_E("[sdcard] statvfs: %s", strerror(errno));
        return -1;
    }
    *out = (uint64_t)st.f_bavail * st.f_frsize;
    return 0;
}

static void sdcard_unmount(void) {
    sdcard_close_file();
    umount(g_mountpoint);
    LOG_I("[sdcard] unmounted %s", g_mountpoint);
}

static const storage_ops_t g_sdcard_ops = {
    .mount          = sdcard_mount,
    .open_file      = sdcard_open_file,
    .write          = sdcard_write,
    .close_file     = sdcard_close_file,
    .get_free_bytes = sdcard_get_free_bytes,
    .unmount        = sdcard_unmount,
};

const storage_ops_t *sdcard_get_ops(void) {
    return &g_sdcard_ops;
}
