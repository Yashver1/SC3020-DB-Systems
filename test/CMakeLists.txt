cmake_minimum_required(VERSION 3.31.5)
project(Test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6
  )

FetchContent_MakeAvailable(catch)

add_executable(test test.cc)
target_include_directories(test PUBLIC include)
target_link_libraries( test PRIVATE record Catch2::Catch2 block utility disk )