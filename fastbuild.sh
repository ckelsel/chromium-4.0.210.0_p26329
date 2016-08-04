#!/bin/sh

if [ "$1" = "-r" ]; then
    BUILD_TYPE=release
else
    BUILD_TYPE=debug
fi

if [ -d build ]; then
    cp build_config.h build
    cd build

    rm -rf base
    rm _test
    rm -rf CMakeFiles/_test.dir

    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    cmake --build .
    ctest -VV
fi
