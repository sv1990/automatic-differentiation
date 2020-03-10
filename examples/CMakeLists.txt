cmake_minimum_required(VERSION 3.14)

add_executable(one_dimensional one_dimensional.cc)
target_compile_options(one_dimensional PRIVATE "-Wall;-Wextra;-pedantic")
target_link_libraries(one_dimensional PRIVATE ad::ad)

add_executable(multi_dimensional multi_dimensional.cc)
target_compile_features(multi_dimensional PRIVATE cxx_std_20)
target_compile_options(multi_dimensional PRIVATE "-Wall;-Wextra;-pedantic")
target_link_libraries(multi_dimensional PRIVATE ad::ad)

add_executable(ostream ostream.cc)
target_compile_features(ostream PRIVATE cxx_std_20)
target_compile_options(ostream PRIVATE "-Wall;-Wextra;-pedantic")
target_link_libraries(ostream PRIVATE ad::ad)

add_executable(error_propagation error_propagation.cc)
target_compile_features(error_propagation PRIVATE cxx_std_20)
target_compile_options(error_propagation PRIVATE "-Wall;-Wextra;-pedantic")
target_link_libraries(error_propagation PRIVATE ad::ad)

add_executable(gradient gradient.cc)
target_compile_features(gradient PRIVATE cxx_std_20)
target_compile_options(gradient PRIVATE "-Wall;-Wextra;-pedantic")
target_link_libraries(gradient PRIVATE ad::ad)
