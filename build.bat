rmdir /q /s build
mkdir build
xcopy build_config.h build
cd build
cmake -G "Visual Studio 10" ..
cmake --build .
ctest -VV
cd ..
