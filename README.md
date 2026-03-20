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

The system bypasses heavy generic frameworks like OpenCV or standard V4L2 in favor of the official Rockchip SDK to guarantee maximum frame rates and hardware efficiency. 

| Layer | Component | Responsibility |
| :--- | :--- | :--- |
| **Application** | **Dashcam App (C/C++)** | Main thread loop, GPIO interrupt handling, state machine, and file I/O. |
| **Framework** | **RK_MPI** | Orchestrates the video pipeline, binding the Video Input (VI) directly to the Video Encoder (VENC). |
| **Image Processing** | **RKAIQ** | Controls the Image Signal Processor (ISP) for auto-exposure, white balance, and sensor tuning using `.iqfile` configurations. |
| **Kernel** | **V4L2 / sysfs** | Low-level drivers for the MIS5001 sensor and GPIO button interrupts. |
| **Hardware** | **MIS5001 / RV1106** | Physical MIPI CSI image capture and silicon-level H.265 encoding. |

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
