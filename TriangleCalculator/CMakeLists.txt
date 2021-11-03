cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)

include_directories(ext_libraries)

add_executable(TriangleCalculator read_matrix.c ext_libraries/mmio.c ext_libraries/mmio.h)
add_executable(SerialTriangle triangles_serial.c ext_libraries/mmio.c ext_libraries/mmio.h)
