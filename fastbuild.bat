if exist build (
    ECHO Y | xcopy build_config.h build
    cd build

    rmdir /q /s base
    rmdir /q /s WIN32
    rmdir /q /s Debug
    rm -rf _test.dir

    cmake ..
    cmake --build .

    if errorlevel 1 (
        echo !!! Build Failure Reason Given is %errorlevel% !!!
        goto end
    )

    ctest -VV
:end
    cd ..
)
