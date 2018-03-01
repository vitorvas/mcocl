cmake_minimum_required (VERSION 2.6)
project(MCOCL)

find_package(OpenCL REQUIRED)

#Force using -c99 flag
set(CMAKE_C_FLAGS "-std=gnu99")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")

add_executable(mc_ocl mc_ocl.c)
target_link_libraries(mc_ocl m ${OpenCL_LIBRARY})

