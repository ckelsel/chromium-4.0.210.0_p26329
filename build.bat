rmdir /q /s build_
mkdir build_
cd build_
cmake .. -G "NMake Makefiles"
nmake
