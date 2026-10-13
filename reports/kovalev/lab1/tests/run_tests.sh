#!/bin/bash
mkdir -p build
if ! g++ -std=c++11 -o build/tests tests/test_main.cpp; then
    echo "ОШИБКА: тесты не собрались"
    exit 1
fi

./build/tests
