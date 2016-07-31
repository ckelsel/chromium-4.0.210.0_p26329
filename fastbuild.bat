if exist build (
    ECHO Y | xcopy build_config.h build
    cd build

    rmdir /q /s base
    rmdir /q /s WIN32

    cmake ..
    cmake --build .
    ctest -VV
)
