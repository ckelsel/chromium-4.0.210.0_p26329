#!/bin/sh

if [ -d build ]; then
    cp build_config.h build
    cd build

    rm -rf base
    rm _test
    rm -rf CMakeFiles/_test.dir

    cmake ..
    cmake --build .
    ctest -VV
fi
