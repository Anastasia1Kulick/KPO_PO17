#!/bin/bash
set -e

cd "$(dirname "$0")"

if [ ! -f lab3.cpp ]; then
    echo "lab3.cpp not found"
    exit 1
fi

g++ -std=c++17 -O2 -o lab3 lab3.cpp

g++ -std=c++17 -O2 -o test3 test3.cpp

./test3 ./lab3