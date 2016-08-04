#!/bin/sh

if [ "$1" = "-r" ]; then
    BUILD_TYPE=release
else
    BUILD_TYPE=debug
fi

rm -rf build

mkdir build
cp build_config.h build
cd build
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
cmake --build .
ctest -VV
