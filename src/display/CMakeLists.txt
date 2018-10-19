cmake_minimum_required(VERSION 3.5)

find_package(OpenGL REQUIRED)

IF(NOT WIN32)
    find_package(glfw3 REQUIRED)
ENDIF()

add_library(display_lib display.cpp lodepng.cpp)

target_link_libraries(display_lib ${OPENGL_gl_LIBRARY} glfw)
