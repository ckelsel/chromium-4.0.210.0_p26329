#!/bin/sh

if [ "$1" == "-d" ]; then
    BUILD_TYPE=debug
else
    BUILD_TYPE=release
fi

OUT=build
rm -rf $OUT

mkdir $OUT
cp build_config.h $OUT
cd $OUT
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
cmake --build .
ctest -VV
