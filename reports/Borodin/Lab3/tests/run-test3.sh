#!/bin/bash
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -O2 -o lab3 lab3.cpp
g++ -std=c++17 -O2 -o test3 test3.cpp
./test3 ./lab3
