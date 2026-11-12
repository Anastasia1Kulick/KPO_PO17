#!/bin/bash
set -e 

echo "=== 1. Проверка компиляции main.cpp ==="
g++ -std=c++17 -Wall -o ./tests/main_check src/main.cpp
echo "[OK] Код компилируется."

echo -e "\n=== 2. Запуск юнит-тестов ==="
g++ -std=c++17 -Wall -o ./tests/test_runner tests/test_solution.cpp
./tests/test_runner

echo -e "\nВсе проверки успешно пройдены!"
rm -f ./tests/main_check ./tests/test_runner
