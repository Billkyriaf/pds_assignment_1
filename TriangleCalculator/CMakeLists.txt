cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)

include_directories(ext_libraries)
include_directories(libraries)

add_executable(TriangleCalculator read_matrix.c ext_libraries/mmio.c)
add_executable(SerialTriangle triangles_serial.c ext_libraries/mmio.c libraries/csr_matrix.c)
add_executable(MatrixManipulation libraries/matrix_manipulation.c)
add_executable(Csr libraries/csr_matrix.c libraries/csr_matrix.h)
