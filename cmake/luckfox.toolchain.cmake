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

# Sysroot — copy from board via: rsync -av --safe-links root@172.32.0.70:/usr/ sysroot/usr/
# Then fix absolute symlinks: symlinks -cr sysroot/
# CMAKE_CURRENT_LIST_DIR is always this file's directory (cmake/), so ../sysroot is stable
# even during CMake's compiler test which changes CMAKE_SOURCE_DIR.
get_filename_component(SYSROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../sysroot" ABSOLUTE)
set(CMAKE_SYSROOT "${SYSROOT_DIR}")

set(CMAKE_FIND_ROOT_PATH
    "${TOOLCHAIN_DIR}"
    "${SYSROOT_DIR}"
    "${SYSROOT_DIR}/oem"
)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# -B points GCC to Ubuntu's multiarch startup files (crt1.o, crti.o, crtn.o).
# The toolchain triple is arm-none-linux-gnueabihf but Ubuntu multiarch uses
# arm-linux-gnueabihf (no "none"), so the linker misses them without this flag.
# Ubuntu multiarch: arch-specific headers (bits/*.h) and libs are in arm-linux-gnueabihf/
# subdirs. The toolchain triple uses "none" so GCC doesn't find them automatically.
set(MULTIARCH_INC "${SYSROOT_DIR}/usr/include/arm-linux-gnueabihf")
set(MULTIARCH_LIB "${SYSROOT_DIR}/usr/lib/arm-linux-gnueabihf")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=hard -B${MULTIARCH_LIB} -I${MULTIARCH_INC}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${MULTIARCH_LIB} -Wl,--as-needed -Wl,-rpath-link,${MULTIARCH_LIB} -Wl,-rpath-link,${SYSROOT_DIR}/lib/arm-linux-gnueabihf")
