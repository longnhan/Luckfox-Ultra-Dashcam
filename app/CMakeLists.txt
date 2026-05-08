add_executable(bodycam
    main.c
    state_machine.c
)

target_include_directories(bodycam PRIVATE
    ${CMAKE_SOURCE_DIR}/hal
    ${CMAKE_SOURCE_DIR}/middleware
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bodycam PRIVATE middleware hal)
