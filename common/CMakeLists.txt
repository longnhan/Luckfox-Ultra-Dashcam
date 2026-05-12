add_library(common STATIC log.c)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common PRIVATE pthread)
