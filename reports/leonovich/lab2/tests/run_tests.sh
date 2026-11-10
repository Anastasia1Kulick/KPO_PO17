#!/bin/bash
set -e

cd "$(dirname "$0")"

if [ ! -f test_main.cpp ]; then
    echo "test_main.cpp not found"
    exit 1
fi

g++ -std=c++17 -O2 -o test_main test_main.cpp

./test_main
