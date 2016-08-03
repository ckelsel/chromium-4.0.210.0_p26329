rmdir /q /s build
mkdir build
xcopy build_config.h build
cd build
cmake ..
cmake --build .
ctest -VV
cd ..
