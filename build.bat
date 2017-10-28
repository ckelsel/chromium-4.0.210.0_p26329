rmdir /q /s build
mkdir build
xcopy build_config.h build
cd build
cmake ..
cmake --build .

if errorlevel 1 (
   echo !!! Build Failure Reason Given is %errorlevel% !!!
   goto end
)

ctest -VV

:end

cd ..
