#!/bin/bash
set -e
cd "$(dirname "$0")"
g++ -std=c++17 -Wall -o /tmp/lab3_tests test_main.cpp
/tmp/lab3_tests