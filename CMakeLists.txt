cmake_minimum_required(VERSION 3.31.5)
project(SC3020 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(block src/block.cc include/block.h)
add_library(utility src/utility.cc include/utility.h)
target_include_directories(block PUBLIC include)
target_include_directories(utility PUBLIC include)

add_executable(main src/main.cc)
target_link_libraries(main block utility)
