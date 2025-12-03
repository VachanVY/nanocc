#!/bin/bash
set -e

mode="$1"

if [ "$mode" = "compiler" ]; then
    cmake -S . -B build -DBUILD_TEST=OFF
    cmake --build build

elif [ "$mode" = "test" ]; then
    cmake -S . -B build -DBUILD_TEST=ON
    cmake --build build 

elif [ "$mode" = "clean" ]; then
        rm -rf build

elif [ "$mode" = "rebuild" ]; then
    rm -rf build
    cmake -S . -B build -DBUILD_TEST=OFF
    cmake --build build

elif [ "$mode" = "rebuild_test" ]; then
    rm -rf build
    cmake -S . -B build -DBUILD_TEST=ON
    cmake --build build

else
    echo "usage: $0 {compiler|test|clean|rebuild|rebuild_test}"
    exit 1
fi
