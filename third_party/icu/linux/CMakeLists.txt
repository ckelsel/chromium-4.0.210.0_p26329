cmake_minimum_required (VERSION 2.8)

set_source_files_properties(icudt42l_dat.s PROPERTIES COMPILE_FLAGS "-x assembler-with-cpp")


add_library(icudata icudt42l_dat.s)
