#!/bin/bash
set -e

g++ -std=c++17 -O2 -o /tmp/lab3 src/lab3.cpp
g++ -std=c++17 -O2 -o /tmp/test3 tests/test3.cpp

/tmp/test3 /tmp/lab3
