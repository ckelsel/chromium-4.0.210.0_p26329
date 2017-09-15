rmdir /q /s build
mkdir build
xcopy build_config.h build
cd build

::cmake -G "Visual Studio 10 Win64" ..
::cmake --build . --config release

cmake -DCMAKE_INSTALL_PREFIX=%CD% -G "Visual Studio 10" ..
cmake --build . --target install
if errorlevel 1 (
   echo !!! Build Failure Reason Given is %errorlevel% !!!
   goto end
)
ctest -VV
:end
cd ..
