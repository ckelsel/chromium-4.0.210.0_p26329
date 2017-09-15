cmake_minimum_required (VERSION 2.6)
project (modp_b64 CXX)

include_directories(.)
include_directories(../../)

add_library(modp_b64 modp_b64.cc)
