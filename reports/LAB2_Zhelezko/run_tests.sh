#!/bin/bash
set -e

cd "$(dirname "$0")"

g++ -std=c++17 -Wall -o /tmp/lab2_tests test_main.cpp

/tmp/lab2_tests