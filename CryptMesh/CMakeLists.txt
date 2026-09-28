cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE
    "C:/Users/vailp/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file"
)

set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")

project("CryptMesh")

add_subdirectory("CryptMesh")