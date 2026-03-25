@echo off
call "D:\application\Visual Studio 2026\VC\Auxiliary\Build\vcvars64.bat"

set "CONFIG=%~1"
if /I "%CONFIG%"=="" set "CONFIG=Debug"
if /I not "%CONFIG%"=="Debug" if /I not "%CONFIG%"=="Release" (
    echo Usage: build.bat [Debug^|Release]
    exit /b 1
)

cmake -B build-ninja/%CONFIG% -S D:/mini ^
 -G Ninja ^
 -DCMAKE_BUILD_TYPE=%CONFIG% ^
 -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build build-ninja/%CONFIG%
