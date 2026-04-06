#!/bin/bash
set -e

CC=${CC:-gcc} 
CXX=${CXX:-g++}

mode="$1"
BUILD_TYPE="${2:-Release}"   # default = Release

if [ "$mode" = "compiler" ]; then
    CC=$CC CXX=$CXX cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DBUILD_NANOCC_TEST=OFF
    cmake --build build

elif [ "$mode" = "rebuild" ]; then
    ./buildcc.sh clean
    ./buildcc.sh compiler "$BUILD_TYPE"

elif [ "$mode" = "test" ]; then
    test_component="$2"
    BUILD_TYPE="${3:-Release}"

    if [ -z "$test_component" ]; then
        echo "Usage: $0 test {test_component} [Debug|Release]"
        exit 1
    fi

    CC=$CC CXX=$CXX cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DBUILD_NANOCC_TEST=ON
    cmake --build build --target "$test_component"

elif [ "$mode" = "rebuild_test" ]; then
    test_component="$2"
    BUILD_TYPE="${3:-Release}"

    ./buildcc.sh clean
    ./buildcc.sh test "$test_component" "$BUILD_TYPE"

elif [ "$mode" = "clean" ]; then
    rm -rf build

else
    echo "usage:"
    echo "$0 compiler [Debug|Release]"
    echo "$0 rebuild [Debug|Release]"
    echo "$0 test {test_component} [Debug|Release]"
    echo "$0 rebuild_test {test_component} [Debug|Release]"
    echo "$0 clean"
    exit 1
fi