#!/bin/sh

if [ -d build ]; then
    cp build_config.h build
    cd build
    cmake ..
    cmake --build .
    ctest -VV
fi
