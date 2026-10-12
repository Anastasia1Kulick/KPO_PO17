#!/bin/bash
set -e

cd "$(dirname "$0")"

if [ ! -f Lab2.cpp ]; then
    echo "Lab2.cpp not found"
    exit 1
fi

g++ -std=c++17 -O2 -o lab2 Lab2.cpp

g++ -std=c++17 -O2 -o test test.cpp

./test ./lab2