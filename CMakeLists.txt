cmake_minimum_required(VERSION 3.16)
project(luckfox_bodycam C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Luckfox SDK path — override via -DLUCKFOX_SDK_PATH=...
set(LUCKFOX_SDK_PATH "$ENV{LUCKFOX_SDK_PATH}" CACHE PATH "Luckfox Pico SDK root")

if(NOT LUCKFOX_SDK_PATH)
    message(FATAL_ERROR "LUCKFOX_SDK_PATH is not set. Export it or pass -DLUCKFOX_SDK_PATH=...")
endif()

set(RK_SDK_INC  ${LUCKFOX_SDK_PATH}/sysdrv/source/mpp/include)
set(RK_MPI_INC  ${LUCKFOX_SDK_PATH}/sysdrv/source/mpp/rkmpi/include)
set(RKAIQ_INC   ${LUCKFOX_SDK_PATH}/sysdrv/source/isp/rkaiq/include)
set(SDK_LIB_DIR ${LUCKFOX_SDK_PATH}/sysdrv/source/mpp/lib)

include_directories(
    ${RK_SDK_INC}
    ${RK_MPI_INC}
    ${RKAIQ_INC}
    ${CMAKE_SOURCE_DIR}/hal
    ${CMAKE_SOURCE_DIR}/middleware
    ${CMAKE_SOURCE_DIR}/app
)

link_directories(${SDK_LIB_DIR})

add_subdirectory(hal)
add_subdirectory(middleware)
add_subdirectory(app)
