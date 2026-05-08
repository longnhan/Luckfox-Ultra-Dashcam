#pragma once

/* Abstract pipeline: binds a camera source to an encoder sink.
 * On RV1106 this maps to RK_MPI_SYS_Bind(VI → VENC) for zero-copy DMA. */

typedef struct {
    int vi_pipe;
    int vi_chn;
    int venc_chn;
} pipeline_config_t;

typedef struct {
    int  (*init)(const pipeline_config_t *cfg);
    int  (*bind)(void);
    void (*unbind)(void);
    void (*deinit)(void);
} pipeline_ops_t;

void pipeline_register(const pipeline_ops_t *ops);

int  pipeline_init(const pipeline_config_t *cfg);
int  pipeline_bind(void);
void pipeline_unbind(void);
void pipeline_deinit(void);
