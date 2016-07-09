#!/bin/sh

rm -rf build

mkdir build
cp build_config.h build
cd build
cmake ..
cmake --build .
ctest -VV
