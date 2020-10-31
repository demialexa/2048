cmake_minimum_required(VERSION 3.5)

add_executable(2048 main.cpp)

add_compile_options(-g -Wall)

target_link_libraries(2048 display_lib logic_lib engine_lib animation_lib)

file(COPY ../../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
