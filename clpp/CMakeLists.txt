cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(clpp)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenCL REQUIRED)
include_directories(${OPENCL_INCLUDE_DIR})

add_library(clpp buffer.cc context.cc device.cc error.cc event.cc platform.cc
                 program.cc queue.cc)
target_link_libraries(clpp ${OPENCL_LIBRARIES})
