cmake_minimum_required(VERSION 3.15...3.31.5)
project(SC3020 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(block src/block.cc include/block.h)
add_library(utility src/utility.cc include/utility.h)
add_library(disk src/diskmanager.cc include/diskmanager.h)
add_library(record src/record.cc include/record.h)
add_library(index src/indexmanager.cc include/indexmanager.h)
target_include_directories(block PUBLIC include)
target_include_directories(utility PUBLIC include)
target_include_directories(disk PUBLIC include)
target_include_directories(record PUBLIC include)
target_include_directories(index PUBLIC include)



target_link_libraries(record PUBLIC block utility)
target_link_libraries(disk PUBLIC block utility record)
target_link_libraries(index PUBLIC block utility record)


add_subdirectory(test)
add_executable(main src/main.cc)

target_link_libraries(main block utility record disk index)
