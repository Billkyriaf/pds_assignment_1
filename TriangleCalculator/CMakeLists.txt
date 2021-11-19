cmake_minimum_required(VERSION 3.20)
project(TriangleCalculator C)

# standard variables
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -O3)
set(OpenMP_C_FLAGS "-fopenmp")

# include directories
include_directories(src/ext_libraries)
include_directories(src/libraries)
include_directories(src/OpenMP)
include_directories(src/OpenCilk)
include_directories(src/Pthreads)
include_directories(src/Serial)

# path variables
set(SRC_DIR "src")
set(LIB_DIR "${SRC_DIR}/libraries")
set(EXT_LIB_DIR "${SRC_DIR}/ext_libraries")

# libraries dependency
add_library(libraries ${LIB_DIR}/csr_matrix.c ${LIB_DIR}/matrix_manipulation.c ${LIB_DIR}/write_csr_to_file.c ${EXT_LIB_DIR}/mmio.c)

# Serial target
add_executable(Serial ${SRC_DIR}/serial_main.c ${SRC_DIR}/Serial/serial_mult.c)
target_link_libraries(Serial libraries)

# Pthread target
add_executable(Pthread ${SRC_DIR}/pthread_main.c ${SRC_DIR}/Pthreads/pthreads_parallel.c)
target_link_libraries(Pthread pthread libraries)

# Openmp target
add_executable(Openmp ${SRC_DIR}/openmp_main.c ${SRC_DIR}/OpenMP/openmp_parallel.c)
target_link_libraries(Openmp ${OpenMP_C_FLAGS} libraries)


#### Nothing bellow here matters you probably dont have the source code to run this ####
add_executable(Testing private/test_main.c  ${SRC_DIR}/OpenMP/openmp_parallel.c ${SRC_DIR}/Serial/serial_mult.c ${SRC_DIR}/Pthreads/pthreads_parallel.c)
target_link_libraries(Testing pthread ${OpenMP_C_FLAGS} libraries)


add_executable(Csr ${LIB_DIR}/csr_matrix.c ${LIB_DIR}/write_csr_to_file.c ${EXT_LIB_DIR}/mmio.c)
