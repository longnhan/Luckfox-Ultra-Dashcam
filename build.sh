#!/bin/bash
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="$SCRIPT_DIR/build"
TOOLCHAIN="$SCRIPT_DIR/cmake/luckfox.toolchain.cmake"
BINARY="$BUILD_DIR/app/bodycam"

usage() {
    echo "Usage: $0 [clean|deploy|help]"
    echo ""
    echo "  (no arg)  Incremental build"
    echo "  clean     Wipe build/ and rebuild from scratch"
    echo "  deploy    Build then push binary to board via ADB"
    echo "  help      Show this message"
}

check_sysroot() {
    if [ ! -d "$SCRIPT_DIR/sysroot" ]; then
        echo "ERROR: sysroot/ not found. Run the rsync commands from README first."
        exit 1
    fi
}

do_cmake() {
    if [ ! -f "$BUILD_DIR/CMakeCache.txt" ]; then
        echo "==> Configuring..."
        cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
            -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN"
    fi
}

do_build() {
    echo "==> Building..."
    cmake --build "$BUILD_DIR" -- -j"$(nproc)"
    echo "==> Done: $BINARY"
}

case "${1:-}" in
    "")
        check_sysroot
        mkdir -p "$BUILD_DIR"
        do_cmake
        do_build
        ;;
    clean)
        check_sysroot
        echo "==> Wiping $BUILD_DIR"
        rm -rf "$BUILD_DIR"
        mkdir -p "$BUILD_DIR"
        do_cmake
        do_build
        ;;
    deploy)
        check_sysroot
        mkdir -p "$BUILD_DIR"
        do_cmake
        do_build
        echo "==> Deploying via ADB..."
        adb push "$BINARY" /userdata/bodycam
        echo "==> Deployed. Run: adb shell /userdata/bodycam"
        ;;
    help|--help|-h)
        usage
        ;;
    *)
        echo "Unknown option: $1"
        usage
        exit 1
        ;;
esac
