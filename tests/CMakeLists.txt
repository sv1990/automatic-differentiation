cmake_minimum_required(VERSION 3.14)

add_executable(static_tests test.cc)
target_compile_features(static_tests PRIVATE cxx_std_20)
target_compile_options(static_tests PRIVATE "-Wall;-Wextra;-pedantic;-Werror")
target_link_libraries(static_tests PRIVATE ad::ad)
add_test(static_tests static_tests)
