cmake_minimum_required (VERSION 2.6)
project (icu C CXX ASM)

if(NOT WIN32)
    add_subdirectory(linux)
    add_subdirectory(source/common)
    add_subdirectory(source/i18n)
endif(NOT WIN32)

