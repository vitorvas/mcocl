cmake_minimum_required (VERSION 3.0)
project(MCOCL)

find_package(OpenCL REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=gnu99 -Wall")

add_executable(mc_ocl mc_ocl.c)
target_link_libraries(mc_ocl m ${OpenCL_LIBRARY})


