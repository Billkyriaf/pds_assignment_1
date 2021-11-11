cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)

include_directories(ext_libraries)
include_directories(libraries)

add_executable(SerialCount serial_main.c libraries/matrix_manipulation.c libraries/csr_matrix.c libraries/write_csr_to_file.c ext_libraries/mmio.c)
add_executable(MatrixManipulation libraries/matrix_manipulation.c libraries/csr_matrix.c ext_libraries/mmio.c)
add_executable(Csr libraries/csr_matrix.c libraries/write_csr_to_file.c ext_libraries/mmio.c)
