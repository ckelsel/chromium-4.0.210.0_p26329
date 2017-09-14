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
if [ $? != 0 ] ; then
    echo "Build failed!"
    exit 1
fi
ctest -VV

