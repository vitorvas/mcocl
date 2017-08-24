cmake_minimum_required (VERSION 2.6)
project(MCOCL)

find_package(OpenCL REQUIRED)

add_executable(mc_ocl mc_ocl.c)
target_link_libraries(mc_ocl m ${OpenCL_LIBRARY})


