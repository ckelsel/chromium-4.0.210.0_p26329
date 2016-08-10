#!/bin/sh

if [ "$1" = "-r" ]; then
    BUILD_TYPE=release
else
    BUILD_TYPE=debug
fi

OUT=build
rm -rf $OUT

mkdir $OUT
cp build_config.h $OUT
cd $OUT
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
cmake --build .
ctest -VV
