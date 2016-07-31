#!/bin/sh

if [ -d build ]; then
    cp build_config.h build
    cd build

    rm -rf base
    rm _test

    cmake ..
    cmake --build .
    ctest -VV
fi
