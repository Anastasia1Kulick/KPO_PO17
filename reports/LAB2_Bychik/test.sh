#!/bin/bash
# sudo apt install libgtest-dev cmake
# cd /usr/src/googletest && sudo cmake . && sudo make && sudo make install

set -e   # остановиться при первой ошибке

echo "=== Сборка тестов ==="

g++ -std=c++17 -Wall -Wextra -O0 -g \
    test.cpp settings.cpp \
    -lgtest -lgtest_main -pthread \
    -o test_runner

echo "=== Запуск тестов ==="
./test_runner --gtest_color=yes

echo "=== Очистка ==="
rm -f test_runner test_*.txt settings.txt students.txt specialities.txt

echo "Готово."
