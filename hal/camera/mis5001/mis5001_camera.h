#pragma once
#include "camera/camera_hal.h"

/* Returns the ops struct wired to RK_MPI VI + MIS5001 sensor. */
const cam_ops_t *mis5001_get_ops(void);
