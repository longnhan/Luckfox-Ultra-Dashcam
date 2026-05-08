set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain installed at ~/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf
# This is the glibc toolchain (ARM official) — targets Ubuntu rootfs on the board.
# For Buildroot/uclibc rootfs, swap to arm-rockchip830-linux-uclibcgnueabihf instead.
set(TOOLCHAIN_DIR "$ENV{HOME}/gcc-arm-11.2-2022.02-x86_64-arm-none-linux-gnueabihf")
set(CROSS_COMPILE "${TOOLCHAIN_DIR}/bin/arm-none-linux-gnueabihf-")

set(CMAKE_C_COMPILER   "${CROSS_COMPILE}gcc")
set(CMAKE_CXX_COMPILER "${CROSS_COMPILE}g++")
set(CMAKE_AR           "${CROSS_COMPILE}ar")
set(CMAKE_STRIP        "${CROSS_COMPILE}strip")

set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_DIR}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=hard")
