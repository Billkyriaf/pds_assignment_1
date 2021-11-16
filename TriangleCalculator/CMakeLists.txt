cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3 -fopenmp")

include_directories(ext_libraries)
include_directories(libraries)
include_directories(OpenMP)
include_directories(OpenCilk)
include_directories(Pthreads)
include_directories(Serial)

add_executable(Testing test_main.c ext_libraries/mmio.c libraries/matrix_manipulation.c libraries/csr_matrix.c  OpenMP/openmp_parallel.c Serial/serial_mult.c Pthreads/pthreads_parallel.c)
add_executable(Csr libraries/csr_matrix.c libraries/write_csr_to_file.c ext_libraries/mmio.c)
