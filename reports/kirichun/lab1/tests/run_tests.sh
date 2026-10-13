#!/usr/bin/env bash
set -e
g++ -std=c++17 -Wall -o /tmp/lab1_tests tests/test_main.cpp src/main.cpp
/tmp/lab1_tests
