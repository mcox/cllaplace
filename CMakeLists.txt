cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(cllaplace)

add_subdirectory(clpp)

find_library(MATH_LIB m)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.43.0 COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(laplace laplace.cc defaults.cc)
target_link_libraries(laplace clpp ${MATH_LIB} ${Boost_LIBRARIES})
