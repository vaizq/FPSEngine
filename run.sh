!#/bin/bash

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/home/amensia/Projects/FPSEngine/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build -j6 && ./build/src/lab/lab
