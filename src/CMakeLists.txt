cmake_minimum_required(VERSION 3.5)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

set(GCC_COMPILE_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")

add_subdirectory(bin)
add_subdirectory(googletest)
add_subdirectory(display)
add_subdirectory(logic)
add_subdirectory(engine)
add_subdirectory(animation)

IF(WIN32)
    add_subdirectory(glfw-3.2.1)
ENDIF()
