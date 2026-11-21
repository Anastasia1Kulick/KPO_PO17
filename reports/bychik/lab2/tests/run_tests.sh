#!/bin/bash
set -e 

echo "======================================"
echo " 1. Проверка компиляции main.cpp"
echo "======================================"
g++ -std=c++17 -Wall -o ./tests/main_check src/main.cpp
echo "[OK] Основная программа компилируется."

echo -e "\n======================================"
echo " 2. Сборка и запуск тестов"
echo "======================================"
g++ -std=c++17 -Wall -o ./tests/test_runner tests/test_solution.cpp
./tests/test_runner

echo -e "\n======================================"
echo " Все проверки успешно пройдены!"
echo "======================================"

# Очистка
rm -f ./tests/main_check ./tests/test_runner
