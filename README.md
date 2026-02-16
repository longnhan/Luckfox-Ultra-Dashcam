# Wearable-Pi-Cam

**Wearable-Pi-Cam** is a high-performance, lightweight firmware designed for the **Raspberry Pi Zero 2 W**. This project focuses on achieving stable, long-duration video recording with minimal CPU overhead by integrating the modern `libcamera` stack into a hardware-accelerated GStreamer pipeline.

---

## Key Features

* **Optimized for Pi Zero 2 W:** Tailored for the 64-bit ARM architecture, ensuring smooth operation within 512MB RAM.
* **Hardware-Accelerated Encoding:** Offloads H.264 compression to the VideoCore IV, keeping CPU usage below **15%**.
* **Dual-Stream Pipeline:**
    * **Primary:** High-bitrate H.264 recording to MicroSD (.mp4).
    * **Secondary:** Low-latency live preview via HDMI.
* **Zero-Copy Memory Handling:** Efficiently passes frames from the sensor to the encoder without unnecessary CPU copies.
* **C++ Efficiency:** Minimalist implementation for fast boot times and high reliability.

---

## Software Architecture Layer

The system is designed in layers to ensure maximum performance and hardware compatibility. By using `GStreamer` as the orchestration engine, the pipeline links the camera hardware directly to the video encoder with zero-copy efficiency.



| Layer | Component | Responsibility |
| :--- | :--- | :--- |
| **Application** | **Wearable-Pi-Cam (C++)** | Pipeline management, file handling, and user interaction. |
| **Framework** | **GStreamer** | Data flow orchestration, H.264 muxing, and HDMI output. |
| **Abstraction** | **libcamera** | Sensor control (AWB, AE, Focus) and frame capture API. |
| **Kernel** | **V4L2 / Media Controller** | Drivers for the Broadcom ISP and CSI interface. |
| **Hardware** | **RPi Camera / SoC** | Physical image capture and H.264 hardware encoding. |

---

## Hardware Requirements

| Component | Recommended Specification |
| :--- | :--- |
| **SBC** | Raspberry Pi Zero 2 W |
| **Camera** | RPi Camera Module V2 or V3 |
| **Storage** | MicroSD Card (Class 10 U3 / V30) |
| **Interface** | Mini CSI Camera Cable (22-pin to 15-pin) |
| **Power** | 3.7V LiPo + 5V Step-up converter |

---

## Getting Started

### 1. Prerequisites
Ensure your system is running **Raspberry Pi OS (64-bit)** with the `libcamera` stack enabled.

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
gstreamer1.0-plugins-bad gstreamer1.0-libcamera cmake g++
```

### 2. Build Instructions
```bash

# Prepare build environment
mkdir build && cd build

# Compile
cmake ..
make -j$(nproc)
```

## Performance Benchmarks
Resolution: 1280x720 (720p) @ 30 FPS
CPU Utilization: ~12%
RAM Footprint: < 40MB
Output: H.264 / MP4 Container

## License
This project is licensed under the MIT License.
