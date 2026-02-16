#!/bin/bash
set -e

mode="$1"

if [ "$mode" = "compiler" ]; then
    cmake -S . -B build -DBUILD_NANOCC_TEST=OFF
    cmake --build build

elif [ "$mode" = "rebuild" ]; then
    ./buildcc.sh clean
    ./buildcc.sh compiler

elif [ "$mode" = "test" ]; then
    test_component="$2"
    cmake -S . -B build -DBUILD_NANOCC_TEST=ON
    cmake --build build --target "$test_component"

elif [ "$mode" = "rebuild_test" ]; then
    test_component="$2"
    ./buildcc.sh clean
    ./buildcc.sh test "$test_component"

elif [ "$mode" = "clean" ]; then
    rm -rf build

else
    echo "usage: $0 {compiler|rebuild|test|rebuild_test|clean}"
    exit 1

fi
