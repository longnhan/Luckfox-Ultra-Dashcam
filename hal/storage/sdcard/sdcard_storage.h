#pragma once
#include "storage/storage_hal.h"

/* SD card inserted via USB OTG — typically /dev/sda1 on RV1106 */
const storage_ops_t *sdcard_get_ops(void);
