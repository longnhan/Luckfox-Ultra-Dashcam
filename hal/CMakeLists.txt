add_library(hal STATIC
    camera/camera_hal.c
    camera/mis5001/mis5001_camera.c
    encoder/encoder_hal.c
    encoder/rkmpp/rkmpp_encoder.c
    storage/storage_hal.c
    storage/sdcard/sdcard_storage.c
    gpio/gpio_hal.c
    gpio/libgpiod/gpiod_button.c
)

target_include_directories(hal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hal PUBLIC gpiod pthread)
