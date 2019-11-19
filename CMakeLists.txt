cmake_minimum_required(VERSION 3.14)

project(AutomaticDifferentiation CXX)

set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.cc)
target_compile_features(main PRIVATE cxx_std_20)
target_compile_options(main PRIVATE "-Wall;-Wextra;-pedantic")
