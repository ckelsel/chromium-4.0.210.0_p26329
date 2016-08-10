#!/bin/sh

if [ "$1" = "-r" ]; then
    BUILD_TYPE=release
else
    BUILD_TYPE=debug
fi

OUT=build

if [ -d $OUT ]; then
    cp build_config.h $OUT
    cd $OUT

    rm -rf base
    rm _test
    rm -rf CMakeFiles/_test.dir

    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    cmake --build .
    ctest -VV
fi
