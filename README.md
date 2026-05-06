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

## Getting Started

### 1. Prerequisites
Because this targets the RV1106, you must build the project using the official Luckfox cross-compilation toolchain on a Linux host machine (e.g., Ubuntu). 

Ensure you have cloned the Luckfox Pico SDK and exported your toolchain path:
```bash
export LUCKFOX_SDK_PATH=/path/to/luckfox-pico
export CROSS_COMPILE=arm-rockchip830-linux-uclibcgnueabihf-
```
### 2. Build Instructions
The project uses CMake for easy cross-compilation mapping.

```bash
# Prepare build environment
mkdir build && cd build

# Compile using the Luckfox toolchain configuration
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/luckfox.toolchain.cmake
make -j$(nproc)
```
### 3. Deployment
Transfer the compiled binary and the required MIS5001 .iqfile to your Luckfox board via SSH or ADB:
```bash
adb push build/luckfox_dashcam /userdata/
adb push iqfiles/mis5001.json /etc/iqfiles/
```

## Performance Benchmarks
Resolution: 1920x1080 (1080p) @ 30 FPS (Adjust based on your final MIS5001 config)

CPU Utilization: < 10%

Encoding: H.265 (HEVC) Main Profile

Output: Raw .h265 elementary stream / .mp4 container

## License
This project is licensed under the MIT License.
