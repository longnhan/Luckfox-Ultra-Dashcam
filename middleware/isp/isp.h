#pragma once

typedef struct {
    const char *iqbin_path;
    int         width;
    int         height;
    int         fps;
} isp_config_t;

typedef struct {
    int  (*init)(const isp_config_t *cfg);
    int  (*start)(void);
    void (*stop)(void);
    void (*deinit)(void);
} isp_ops_t;

void isp_register(const isp_ops_t *ops);

int  isp_init(const isp_config_t *cfg);
int  isp_start(void);
void isp_stop(void);
void isp_deinit(void);
