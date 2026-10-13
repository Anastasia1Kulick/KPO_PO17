#!/bin/bash
set -e

cd "$(dirname "$0")"

if [ ! -f 1lab.cpp ]; then
    echo "1lab.cpp not found"
    exit 1
fi

g++ -std=c++17 -O2 -o 1lab 1lab.cpp

g++ -std=c++17 -O2 -o test test.cpp

./test ./1lab