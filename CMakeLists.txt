cmake_minimum_required(VERSION 3.14)

project(AutomaticDifferentiation CXX)

set(CMAKE_BUILD_TYPE Debug)

add_executable(one_dimensional one_dimensional.cc)
target_compile_features(one_dimensional PRIVATE cxx_std_20)
target_compile_options(one_dimensional PRIVATE "-Wall;-Wextra;-pedantic")

add_executable(multi_dimensional multi_dimensional.cc)
target_compile_features(multi_dimensional PRIVATE cxx_std_20)
target_compile_options(multi_dimensional PRIVATE "-Wall;-Wextra;-pedantic")
