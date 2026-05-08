#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    int  (*mount)(const char *device, const char *mountpoint);
    int  (*open_file)(const char *path);
    int  (*write)(const void *data, size_t len);
    int  (*close_file)(void);
    int  (*get_free_bytes)(uint64_t *out);
    void (*unmount)(void);
} storage_ops_t;

void storage_register(const storage_ops_t *ops);

int  storage_mount(const char *device, const char *mountpoint);
int  storage_open_file(const char *path);
int  storage_write(const void *data, size_t len);
int  storage_close_file(void);
int  storage_get_free_bytes(uint64_t *out);
void storage_unmount(void);
