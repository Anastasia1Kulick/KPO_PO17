#!/bin/bash

# Проверка наличия gtest
if ! dpkg -l | grep -q libgtest-dev; then
    echo "Установите gtest: sudo apt install libgtest-dev"
fi

echo "=== Сборка тестов ==="
# Компилируем test.cpp (он внутри себя инклудит main.cpp)
g++ -std=c++17 -Wall -Wextra test.cpp -lgtest -lgtest_main -pthread -o test_runner

if [ $? -ne 0 ]; then
    echo "Ошибка компиляции!"
    exit 1
fi

echo "=== Запуск тестов ==="
./test_runner --gtest_color=yes

echo "=== Очистка ==="
rm -f test_runner test_*.txt settings.txt students.txt specialities.txt
