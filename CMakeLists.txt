cmake_minimum_required (VERSION 2.8.7 FATAL_ERROR)
project (MyChromium)

add_subdirectory (googletest)

add_executable (MyChromium main.cpp)
target_link_libraries (MyChromium googletest)

