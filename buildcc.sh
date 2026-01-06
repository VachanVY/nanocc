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
    cmake -S . -B build -DBUILD_NANOCC_TEST=ON
    cmake --build build

elif [ "$mode" = "rebuild_test" ]; then
    ./buildcc.sh clean
    ./buildcc.sh test

elif [ "$mode" = "clean" ]; then
    rm -rf build

else
    echo "usage: $0 {compiler|rebuild|test|rebuild_test|clean}"
    exit 1

fi
