# Luckfox-Ultra-Dashcam

**Luckfox-Ultra-Dashcam** is a high-performance, lightweight firmware designed for the **Luckfox Ultra B (Rockchip RV1106)**. This project focuses on achieving stable, continuous video recording with minimal CPU overhead by leveraging the Rockchip Media Process Interface (RK_MPI) and hardware Video Processing Unit (VPU) for zero-copy encoding.

---

## Key Features

* **Optimized for RV1106 SoC:** Custom-built for the Luckfox Ultra B, ensuring rock-solid stability within a constrained memory footprint.
* **Hardware-Accelerated Encoding:** Offloads H.264/H.265 compression directly to the on-board VPU, keeping CPU usage extremely low.
* **State-Machine Driven:** Simple, interrupt-driven GPIO control (on-board button) to toggle recording states seamlessly.
* **Zero-Copy Memory Handling:** Efficiently passes raw frames from the MIS5001 sensor through the ISP and into the hardware encoder using DMA buffers without CPU intervention.
* **C/C++ Native:** A minimalist, low-level implementation designed for instant boot times and high reliability in automotive environments.

---

## Software Architecture Layer

The system follows a "Direct-to-Silicon" architecture, bypassing generic frameworks like OpenCV to ensure maximum hardware efficiency, low latency, and minimal CPU overhead on the RV1106 SoC.

| Layer | Component | Responsibility |
| :--- | :--- | :--- |
| **Application** | **Bodycam Logic (C)** | High-level state machine, file I/O management, and `libavformat` integration. |
| **Framework** | **RK_MPI** | **Media Process Interface:** Orchestrates the pipeline by binding Video Input (VI) to Video Encoder (VENC) for zero-copy memory exchange. |
| **Encoding** | **Rockchip MPP** | **Media Process Platform:** Handles hardware-accelerated H.264/H.265 compression logic and bitrate control. |
| **Image Signal** | **Rockchip AIQ** | **ISP Engine:** Manages sensor-specific tuning (AE, AWB, Noise Reduction) for the MIS5001 using `.iqbin` configuration files. |
| **I/O Control** | **libgpiod** | Modern C library for GPIO interaction; handles the hardware button interrupts to trigger recording states. |
| **Muxing** | **FFmpeg (libavformat)** | Wraps raw H.26x streams into fragmented MP4 (fMP4) containers to prevent file corruption during sudden power loss. |
| **Kernel/Driver** | **V4L2 / Video4Linux2** | Low-level Linux kernel drivers for the MIPI CSI sensor interface. |

### End-to-end data flow

```
┌─────────────────────────────────────────────────────────────────────┐
│  SENSOR                                                             │
│                                                                     │
│  MIS5001 ──MIPI CSI──► VI (Video Input)                            │
│                              │                                      │
│                         mis5001.iqbin                               │
│                              ▼                                      │
│                    ┌─── RKAIQ ISP ───┐                             │
│                    │  AE / AWB / NR  │  ← tuning runs before VI    │
│                    └────────┬────────┘                             │
└─────────────────────────────┼───────────────────────────────────────┘
                              │ DMA-BUF (zero-copy)
                    RK_MPI_SYS_Bind(VI → VENC)
                              │
┌─────────────────────────────▼───────────────────────────────────────┐
│  ENCODE                                                             │
│                                                                     │
│                    VENC ── Rockchip MPP ── H.265 packets           │
│                              │                                      │
└─────────────────────────────┼───────────────────────────────────────┘
                              │
               ┌──────────────┴──────────────┐
               │  state machine decides       │
               │  (button / storage events)   │
               └──────┬───────────────┬───────┘
                      │               │
         ┌────────────▼──┐     ┌──────▼────────────┐
         │  RECORD path  │     │   STREAM path      │
         │               │     │                    │
         │  fMP4 muxer   │     │  live555 RTSP      │
         │ (libavformat) │     │  server (wlan0)    │
         │  frag_keyframe│     │  port 8554         │
         │  +empty_moov  │     │                    │
         └──────┬────────┘     └──────┬─────────────┘
                │                     │
         ┌──────▼──────┐       ┌──────▼──────────────┐
         │  SD card    │       │  Wi-Fi client        │
         │ /mnt/sdcard │       │  rtsp://<ip>:8554/   │
         │  .mp4 file  │       │  live                │
         └─────────────┘       └─────────────────────-┘
```

**Key rules enforced by the flow:**
- RKAIQ ISP **must start before** VI opens — frames are untuned otherwise.
- `RK_MPI_SYS_Bind` keeps encoded data inside DMA buffers — **no CPU memcpy** between VI and VENC.
- Both paths receive the **same encoded packet** — the state machine decides which sinks are active (record-only, stream-only, or both simultaneously).
- fMP4 `frag_keyframe+empty_moov` flags commit metadata on every keyframe — files survive sudden power loss.

---

## Technical Implementation Details

### 1. High-Efficiency Pipeline
By utilizing **RK_MPI (Rockchip Media Process Interface)**, the video data stays within the hardware buffer (DMA-BUF) from capture to encoding. This "Zero-Copy" approach allows the RV1106 to record 1080p video while keeping CPU usage extremely low, preserving battery life for body-worn applications.

### 2. Storage Integrity (fMP4)
Standard MP4 files require a "moov atom" to be written at the end of recording. If the camera loses power or the SD card is ejected, the file becomes unplayable. This project uses **Fragmented MP4 (fMP4)** via `libavformat`, which writes metadata in continuous increments, ensuring that even if the system crashes, the video recorded up to that point is saved.

### 3. Hardware Button Handling
Instead of polling the CPU (which wastes power), the system uses **libgpiod** to listen for hardware interrupts on the Luckfox on-board button.
* **Single Press:** Start/Stop Recording.
* **Long Press:** System Shutdown / Toggle Wi-Fi (if applicable).

### 4. ISP Tuning
The **MIS5001** sensor is tuned via **RKAIQ**. The system loads a custom `.iqbin` file at runtime to optimize image quality for various lighting conditions, ensuring clear evidence capture in low-light environments.

---


## Hardware Requirements

| Component | Recommended Specification |
| :--- | :--- |
| **SBC** | Luckfox Ultra B (RV1106) |
| **Camera** | MIS5001 MIPI CSI Sensor |
| **Storage** | MicroSD Card (Class 10 U3 / V30) or USB Flash Drive |
| **Interface** | Standard MIPI CSI FPC Cable |
| **Controls** | On-board Luckfox user button (GPIO triggered) |

---

## Board Specifications (Verified)

> Collected from a live Luckfox Ultra B via SSH — 2025-05-09.

### System

| Field | Value |
| :--- | :--- |
| **OS** | Ubuntu 22.04.5 LTS (Jammy Jellyfish) |
| **Kernel** | Linux 5.10.160, armv7l |
| **CPU** | ARMv7 Cortex-A7 rev 5, single core — 48 BogoMIPS |
| **RAM** | ~211 MB total, ~158 MB available at idle |
| **Serial** | `20782c5dadb76845` |

### eMMC partition layout

| Device | Size | Mount | Purpose |
| :--- | :--- | :--- | :--- |
| `/dev/mmcblk0` (root) | 5.9 GB | `/` | Root filesystem |
| `/dev/mmcblk0p5` | 488 MB | `/oem` | Rockchip SDK libs & IQ files |
| `/dev/mmcblk0p6` | 238 MB | `/userdata` | Application binaries (deploy here) |

> There is **no SD card slot** on the board itself. The MicroSD card is attached via USB OTG and appears as `/dev/sda1`.

### Network interfaces

| Interface | State | IP | Purpose |
| :--- | :--- | :--- | :--- |
| `usb0` | UP | `172.32.0.70` | USB RNDIS — dev access from host |
| `wlan0` | UP | DHCP (e.g. `10.176.68.91`) | Wi-Fi — RTSP stream target |
| `eth0` | DOWN | — | Unused |

### GPIO

5 GPIO chips present: `gpio0`–`gpio3` (32 lines each), `gpio4` (24 lines).
Button is wired to **chip 0, line 14** (`CFG_GPIO_CHIP=0`, `CFG_GPIO_LINE=14` in [app/config.h](app/config.h)).

### Pre-installed Rockchip libraries

These are shipped with the firmware and require **no manual installation**:

| Library | Path | Purpose |
| :--- | :--- | :--- |
| `librockchip_mpp.so` | `/oem/usr/lib/` | MPP hardware codec (VENC/VDEC) |
| `librkaiq.so` | `/oem/usr/lib/`, `/usr/lib/` | ISP tuning engine (AE/AWB/NR) |
| `librga.so` | `/oem/usr/lib/`, `/usr/lib/` | 2D graphics acceleration |
| `librockit.so` | `/oem/usr/lib/` | Rockchip media toolkit |
| `librknnmrt.so` | `/oem/usr/lib/` | RKNN NPU runtime |

### IQ files present on board

```
/etc/iqfiles/mis5001_CMK-OT2115-PC1_30IRC-F16.json   ← MIS5001 (this project)
/etc/iqfiles/sc3336_CMK-OT2119-PC1_30IRC-F16.json
/etc/iqfiles/sc4336_OT01_40IRC_F16.json
```

---

## Board Library Dependencies

The board runs **Ubuntu 22.04** — use `apt` to install missing runtime libraries.

### Pre-installed — no action needed

All libraries required by the current implementation are already present on the board:

| Library | Version | Purpose |
| :--- | :--- | :--- |
| `libgpiod2` | 1.6.3 | GPIO button handling |
| `libgpiod-dev` | 1.6.3 | Headers for cross-compile sysroot |
| `libavformat58` | 4.4.2 | fMP4 muxer |
| `libavcodec58` | 4.4.2 | FFmpeg codec layer |
| `libavutil56` | 4.4.2 | FFmpeg utilities |
| `librockchip_mpp.so` | — | MPP hardware encoder |
| `librkaiq.so` | — | ISP tuning engine |

### live555 — build from source (future)

`liblivemedia-dev` is **not available** in the Ubuntu 22.04 armhf repos. Install it from source only when the `live555_rtsp.c` TODOs are implemented:

```bash
# On the board
cd /tmp
wget http://www.live555.com/liveMedia/public/live555-latest.tar.gz
tar xzf live555-latest.tar.gz && cd live
./genMakefiles linux-with-shared-libraries
make -j2
sudo make install
```

### Verify all runtime dependencies resolve

```bash
ldd /userdata/bodycam
```

All lines should say a path, not `not found`.

---

## Getting Started

### 1. Clone the repository

```bash
git clone <repo-url> Luckfox-Ultra-Dashcam
cd Luckfox-Ultra-Dashcam
```

### 2. Download and install the cross-compiler

The Luckfox board runs **Ubuntu 22.04 (glibc)**, so you need the `arm-none-linux-gnueabihf` glibc toolchain — **not** the uclibc buildroot toolchain.

**Download:**

| Toolchain | Target system | Source |
|-----------|--------------|--------|
| `gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf` | Ubuntu 22.04 / glibc (this project) | ARM official website |
| `arm-rockchip830-linux-uclibcgnueabihf` | Buildroot / uclibc (not used here) | Luckfox SDK download page |

Extract the toolchain into the `toolchain/` folder inside this repo:

```bash
# Extract — adjust the filename to match your downloaded archive
tar zxvf arm-rockchip830-linux-uclibcgnueabihf.tar.gz -C toolchain/
```

After extraction the layout should look like:
```
toolchain/
└── arm-rockchip830-linux-uclibcgnueabihf/
    ├── bin/
    │   ├── arm-rockchip830-linux-uclibcgnueabihf-gcc
    │   └── ...
    └── ...
```

Export the toolchain and SDK paths (add to `~/.bashrc` or `~/.zshrc` to make permanent):

```bash
export LUCKFOX_SDK_PATH=/path/to/luckfox-pico   # full Luckfox SDK (for RK_MPI headers)
export CROSS_COMPILE=arm-rockchip830-linux-uclibcgnueabihf-
export PATH="$(pwd)/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin:$PATH"
```

> **Note:** The `toolchain/` directory is git-ignored. Each developer extracts the toolchain locally after cloning.

### 3. Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/luckfox.toolchain.cmake
make -j$(nproc)
```

### 4. Deploy to the board

Connect the Luckfox board via USB-C (USB RNDIS: board IP `172.32.0.70`).

```bash
# Via ADB
adb push build/bodycam /userdata/
adb push iqfiles/mis5001.iqbin /etc/iqfiles/

# Via SSH (once network is up)
scp build/bodycam root@172.32.0.70:/userdata/
scp iqfiles/mis5001.iqbin root@172.32.0.70:/etc/iqfiles/

# Run on the board
ssh root@172.32.0.70 "/userdata/bodycam"
```

## Performance Benchmarks
Resolution: 1920x1080 (1080p) @ 30 FPS (Adjust based on your final MIS5001 config)

CPU Utilization: < 10%

Encoding: H.265 (HEVC) Main Profile

Output: Raw .h265 elementary stream / .mp4 container

## License
This project is licensed under the MIT License.
