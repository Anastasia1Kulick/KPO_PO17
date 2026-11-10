#!/usr/bin/env bash
set -e

# CI запускает скрипт с рабочей директорией = папка лабы
cd "$(dirname "$0")/.."

g++ -std=c++17 -Wall -o /tmp/lab3_tests tests/test_main.cpp

/tmp/lab3_tests
