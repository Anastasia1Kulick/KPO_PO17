#!/bin/bash
set -e

cd "$(dirname "$0")"

if [ ! -f test.cpp ]; then
    echo "test.cpp not found"
    exit 1
fi

g++ -std=c++17 -O2 -o test test.cpp

./test
