cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3 -fopenmp")

include_directories(ext_libraries)
include_directories(libraries)
include_directories(OpenMP)

add_executable(SerialCount serial_main.c libraries/matrix_manipulation.c libraries/csr_matrix.c libraries/write_csr_to_file.c ext_libraries/mmio.c OpenMP/m_man_openmp.c)
add_executable(MatrixManipulation libraries/matrix_manipulation.c libraries/csr_matrix.c ext_libraries/mmio.c)
add_executable(Csr libraries/csr_matrix.c libraries/write_csr_to_file.c ext_libraries/mmio.c)

add_executable(openmp OpenMP/test.c)
