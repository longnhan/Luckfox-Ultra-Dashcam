#pragma once

/* --- Camera --- */
#define CFG_WIDTH          1920
#define CFG_HEIGHT         1080
#define CFG_FPS            30
#define CFG_IQBIN_PATH     "/etc/iqfiles/mis5001.iqbin"

/* --- Encoder --- */
#define CFG_CODEC_H265     1          /* 0 = H.264, 1 = H.265 */
#define CFG_BITRATE_KBPS   8000

/* --- Storage (SD card via USB OTG) --- */
#define CFG_SD_DEVICE      "/dev/sda1"
#define CFG_SD_MOUNTPOINT  "/mnt/sdcard"
#define CFG_RECORD_DIR     "/mnt/sdcard/DCIM"
#define CFG_MIN_FREE_MB    256        /* stop recording below this threshold */

/* --- RTSP --- */
#define CFG_RTSP_PORT      8554
#define CFG_RTSP_STREAM    "live"    /* rtsp://<wifi_ip>:8554/live */

/* --- GPIO (Luckfox on-board button) --- */
#define CFG_GPIO_CHIP      0
#define CFG_GPIO_LINE      14
#define CFG_LONG_PRESS_MS  1500

/* --- RK_MPI channel IDs --- */
#define CFG_VI_PIPE        0
#define CFG_VI_CHN         0
#define CFG_VENC_CHN       0
