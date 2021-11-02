cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)

include_directories(libraries)

add_executable(TriangleCalculator read_matrix.c libraries/mmio.c libraries/mmio.h)
add_executable(SerialTriangle triangles_serial.c libraries/mmio.c libraries/mmio.h)
