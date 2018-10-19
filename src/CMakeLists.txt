cmake_minimum_required(VERSION 3.5)

include(CTest)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(bin)
add_subdirectory(googletest)
add_subdirectory(display)
