# CLAUDE.md — Luckfox-Ultra-Dashcam

## Token-saving rules (read first)

- **Never read `.pdf` files** in `docs/`. An equivalent `.md` exists for every PDF — read that instead.
- **Never glob-read all source files** at the start of a session. Use the folder structure below to find what you need.
- **Header files (`.h`) are the source of truth** for interfaces. Read the `.h` before the `.c` to understand a component.
- **`app/config.h`** holds every tunable constant (resolution, paths, GPIO pin, bitrate). Check it before searching for magic numbers.
- When asked "where is X", grep first, read second.

---

## Project overview

Body camera firmware for **Luckfox Ultra B (Rockchip RV1106)**. Records H.265 video to a USB SD card using hardware-accelerated encoding (zero CPU copy), and streams live video over RTSP via the on-board Wi-Fi card.

Key goals:
- < 10% CPU during 1080p30 recording
- Power-loss-safe fMP4 files (no corrupt recordings on sudden shutdown)
- Single button controls: short press = record toggle, long press = RTSP stream toggle
- Hardware-swappable: change one `register()` call in `main.c` to target different hardware

---

## Hardware

| Component | Detail |
|-----------|--------|
| SoC | Rockchip RV1106 (ARM Cortex-A7 + NPU + VPU + ISP) |
| Board | Luckfox Ultra B |
| Camera | MIS5001 MIPI CSI sensor |
| Storage | MicroSD via USB OTG (`/dev/sda1`, mounted at `/mnt/sdcard`) |
| Network | On-board Wi-Fi (`wlan0`) for RTSP; USB RNDIS (`usb0`) for dev |
| Button | On-board user button — GPIO chip 0, line 14 |

---

## Development environment

| Machine | Role |
|---------|------|
| Ubuntu laptop | Build host, cross-compiler |
| Luckfox board | Target — connected via USB-C OTG |

**USB network:**
- Board IP (usb0): `172.32.0.70`
- Host IP (Ubuntu): `172.32.0.1`
- SSH: `ssh root@172.32.0.70` (password: luckfox)
- ADB: `adb shell` (device ID changes per session)

**Build toolchain:**
```bash
export LUCKFOX_SDK_PATH=/path/to/luckfox-pico
export CROSS_COMPILE=arm-rockchip830-linux-uclibcgnueabihf-

mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/luckfox.toolchain.cmake
make -j$(nproc)
```

**Deploy:**
```bash
adb push build/bodycam /userdata/
adb push iqfiles/mis5001.iqbin /etc/iqfiles/
ssh root@172.32.0.70 "/userdata/bodycam"
```

---

## Architecture — three layers

```
┌──────────────────────────────────────────┐
│  APP  app/main.c · state_machine · config │  ← business logic only
├──────────────────────────────────────────┤
│  MIDDLEWARE  pipeline · isp · muxer · rtsp│  ← orchestration
├──────────────────────────────────────────┤
│  HAL  camera · encoder · storage · gpio  │  ← hardware specifics
└──────────────────────────────────────────┘
```

**Rule:** upper layers call only the abstract `_hal.h` / middleware `.h` interfaces. They never `#include` a concrete implementation header directly — that only happens in `main.c` at the wiring step.

**Swapping hardware:** in `main.c`, change one line per subsystem:
```c
cam_register(mis5001_get_ops());   // ← swap to v4l2_get_ops() for generic camera
enc_register(rkmpp_get_ops());     // ← swap to sw_encoder_get_ops() for CPU encode
muxer_register(fmp4_get_ops());    // ← swap to ts_muxer_get_ops() for MPEG-TS
rtsp_register(live555_get_ops());  // ← swap to ffmpeg_rtsp_get_ops()
```

**Interface pattern** (same in every HAL and middleware component):
```c
// abstract header: declares ops struct + forwarding functions
typedef struct { int (*init)(...); ... } foo_ops_t;
void foo_register(const foo_ops_t *ops);

// concrete implementation: fills the ops struct, exposes get_ops()
const foo_ops_t *bar_get_ops(void);

// main.c: foo_register(bar_get_ops())
```

---

## Folder structure

```
Luckfox-Ultra-Dashcam/
│
├── CLAUDE.md                        ← you are here
├── README.md                        ← project overview and benchmarks
├── CMakeLists.txt                   ← root CMake; sets SDK paths, adds subdirs
├── cmake/
│   └── luckfox.toolchain.cmake      ← ARM cross-compile toolchain definition
│
├── app/                             ── APPLICATION LAYER ──
│   ├── config.h                     ← ALL tunable constants (edit here first)
│   ├── main.c                       ← wires HAL+middleware impls, encode/mux loop
│   ├── state_machine.h              ← states (IDLE/RECORDING/STREAMING/BOTH) + events
│   ├── state_machine.c              ← FSM transitions; calls muxer_open/close, rtsp_start/stop
│   └── CMakeLists.txt
│
├── hal/                             ── HARDWARE ABSTRACTION LAYER ──
│   ├── CMakeLists.txt
│   ├── camera/
│   │   ├── camera_hal.h             ← cam_ops_t interface + cam_*() forwarding API
│   │   ├── camera_hal.c             ← dispatches through registered ops
│   │   └── mis5001/                 ← MIS5001 sensor via RK_MPI VI
│   │       ├── mis5001_camera.h     ← mis5001_get_ops()
│   │       └── mis5001_camera.c     ← TODO: RK_MPI_VI_* calls
│   ├── encoder/
│   │   ├── encoder_hal.h            ← enc_ops_t (init/encode/release/stop/deinit)
│   │   ├── encoder_hal.c
│   │   └── rkmpp/                   ← Rockchip MPP hardware encoder
│   │       ├── rkmpp_encoder.h      ← rkmpp_get_ops()
│   │       └── rkmpp_encoder.c      ← TODO: RK_MPI_VENC_* calls
│   ├── storage/
│   │   ├── storage_hal.h            ← storage_ops_t (mount/open/write/close/free/unmount)
│   │   ├── storage_hal.c
│   │   └── sdcard/                  ← SD card via USB OTG (/dev/sda1 → /mnt/sdcard)
│   │       ├── sdcard_storage.h     ← sdcard_get_ops()
│   │       └── sdcard_storage.c     ← POSIX open/write/statvfs/mount — fully implemented
│   └── gpio/
│       ├── gpio_hal.h               ← gpio_ops_t + gpio_event_t (SHORT/LONG press)
│       ├── gpio_hal.c
│       └── libgpiod/                ← libgpiod interrupt-driven button
│           ├── gpiod_button.h       ← gpiod_get_ops()
│           └── gpiod_button.c       ← fully implemented; fires callback on press/release
│
├── middleware/                      ── MIDDLEWARE LAYER ──
│   ├── CMakeLists.txt
│   ├── pipeline/
│   │   ├── pipeline.h               ← pipeline_ops_t (init/bind/unbind/deinit)
│   │   ├── pipeline.c
│   │   └── rkmpi/                   ← RK_MPI_SYS_Bind(VI→VENC) zero-copy DMA
│   │       ├── rkmpi_pipeline.h     ← rkmpi_pipeline_get_ops()
│   │       └── rkmpi_pipeline.c     ← TODO: RK_MPI_SYS_Bind/UnBind calls
│   ├── isp/
│   │   ├── isp.h                    ← isp_ops_t (init/start/stop/deinit)
│   │   ├── isp.c
│   │   └── rkaiq/                   ← RKAIQ AE/AWB/noise-reduction engine
│   │       ├── rkaiq_isp.h          ← rkaiq_get_ops()
│   │       └── rkaiq_isp.c          ← TODO: rk_aiq_uapi2_sysctl_* calls
│   ├── muxer/
│   │   ├── muxer.h                  ← muxer_ops_t (open/write_packet/close)
│   │   ├── muxer.c
│   │   └── fmp4/                    ← fragmented MP4 via libavformat
│   │       ├── fmp4_muxer.h         ← fmp4_get_ops()
│   │       └── fmp4_muxer.c         ← fully implemented; movflags=frag_keyframe+empty_moov
│   └── rtsp/
│       ├── rtsp.h                   ← rtsp_ops_t (init/start/push_packet/stop/deinit)
│       ├── rtsp.c
│       └── live555/                 ← live555 RTSP server on wlan0
│           ├── live555_rtsp.h       ← live555_get_ops()
│           └── live555_rtsp.c       ← TODO: live555 TaskScheduler + RTPSink wiring
│
├── iqfiles/                         ← drop mis5001.iqbin here (not in git)
└── docs/                            ← reference docs
    ├── *.md                         ← READ THESE (human-readable)
    └── *.pdf                        ← DO NOT READ (same content, wastes tokens)
```

---

## Implementation status

| Component | File | Status |
|-----------|------|--------|
| SD card storage | `hal/storage/sdcard/sdcard_storage.c` | Complete |
| GPIO button | `hal/gpio/libgpiod/gpiod_button.c` | Complete |
| fMP4 muxer | `middleware/muxer/fmp4/fmp4_muxer.c` | Complete |
| State machine | `app/state_machine.c` | Complete |
| MIS5001 camera | `hal/camera/mis5001/mis5001_camera.c` | TODO: RK_MPI_VI_* |
| RKMPP encoder | `hal/encoder/rkmpp/rkmpp_encoder.c` | TODO: RK_MPI_VENC_* |
| RK_MPI pipeline | `middleware/pipeline/rkmpi/rkmpi_pipeline.c` | TODO: SYS_Bind |
| RKAIQ ISP | `middleware/isp/rkaiq/rkaiq_isp.c` | TODO: rk_aiq_uapi2_* |
| RTSP server | `middleware/rtsp/live555/live555_rtsp.c` | TODO: live555 wiring |

---

## Key design decisions

- **fMP4 over standard MP4** — `frag_keyframe+empty_moov+default_base_moof` flags ensure every keyframe commits metadata to disk; files survive power loss.
- **`ops` struct pattern** — C-style polymorphism via function pointer structs. No vtables, no C++, no dynamic allocation.
- **RKAIQ must start before VI** — ISP tuning context must be initialized before the camera pipeline opens, or frames are untuned.
- **RK_MPI bind is zero-copy** — once `pipeline_bind()` is called, VI output DMA-buffers are handed directly to VENC without any CPU memcpy.
- **SD card as `/dev/sda1`** — the Luckfox USB OTG exposes inserted SD cards as a USB mass storage device enumerated as `/dev/sda1`.

---

## Reference docs

| File | Content |
|------|---------|
| `docs/Luckfox-Pico-Ultra-W-*.md` | Board pinout, GPIO table, USB gadget, Wi-Fi setup |
| `README.md` | Architecture table, build instructions, benchmarks |
