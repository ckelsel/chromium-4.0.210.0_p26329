rmdir /q /s build
mkdir build
cd build
cmake ..
cmake --build .
ctest -VV
