add_library(middleware STATIC
    pipeline/pipeline.c
    pipeline/rkmpi/rkmpi_pipeline.c
    isp/isp.c
    isp/rkaiq/rkaiq_isp.c
    muxer/muxer.c
    muxer/fmp4/fmp4_muxer.c
    rtsp/rtsp.c
    rtsp/live555/live555_rtsp.c
)

target_include_directories(middleware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(middleware PUBLIC common hal avformat avcodec avutil pthread)
