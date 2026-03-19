@echo off
call "D:\application\Visual Studio 2026\VC\Auxiliary\Build\vcvars64.bat"

cmake -B build-ninja -S D:/mini ^
 -G Ninja ^
 -DCMAKE_BUILD_TYPE=Debug ^
 -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build build-ninja