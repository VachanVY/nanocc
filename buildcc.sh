#!/bin/bash
set -e

mode="$1"

cmake -S . -B build
cmake --build build
