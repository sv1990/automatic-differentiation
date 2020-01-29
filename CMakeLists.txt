cmake_minimum_required(VERSION 3.14)

project(AutomaticDifferentiation CXX)

set(CMAKE_BUILD_TYPE Debug)

add_library(ad INTERFACE)
add_library(ad::ad ALIAS ad)
target_compile_features(ad INTERFACE cxx_std_17)

target_include_directories(
  ad
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
