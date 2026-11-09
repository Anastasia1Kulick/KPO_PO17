#!/bin/bash

# ==============================================================
#  run_tests.sh — сборка и запуск тестов (ЛР2)
#  Использование:  ./run_tests.sh
# ==============================================================

set -u

# --- Настройки ---
TEST_SRC="tests/test_main.cpp"
SRC_FILES="settings.cpp logger.cpp"      # ⭐ файлы, которые тестируются
TEST_BIN="tests/tests"
TMP_FILES=("test_settings.txt" "tmp_test.txt" "test.log")

# --- Цвета ---
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[1;33m"
NC="\033[0m"

echo "=============================================="
echo " СБОРКА ТЕСТОВ (ЛР2)"
echo "=============================================="

# --- Проверка файлов ---
MISSING=0
for f in "$TEST_SRC" $SRC_FILES; do
    if [ ! -f "$f" ]; then
        echo -e "${RED}Ошибка: файл '$f' не найден!${NC}"
        MISSING=1
    fi
done
if [ $MISSING -eq 1 ]; then
    exit 1
fi

# --- Компиляция ---
# ВАЖНО: НЕ используем -DNDEBUG, иначе assert() будет отключён
g++ -std=c++17 -Wall -Wextra \
    "$TEST_SRC" $SRC_FILES \
    -o "$TEST_BIN"

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo -e "${GREEN}Компиляция успешна.${NC}"
echo

# --- Запуск тестов ---
echo "=============================================="
echo " ЗАПУСК ТЕСТОВ"
echo "=============================================="

"$TEST_BIN"
RESULT=$?

# --- Уборка временных файлов ---
for f in "${TMP_FILES[@]}"; do
    [ -f "$f" ] && rm -f "$f"
done

echo
echo "=============================================="
if [ $RESULT -eq 0 ]; then
    echo -e "${GREEN}РЕЗУЛЬТАТ: ВСЕ ТЕСТЫ ПРОЙДЕНЫ${NC}"
    echo "=============================================="
    exit 0
else
    echo -e "${RED}РЕЗУЛЬТАТ: ТЕСТЫ ПРОВАЛЕНЫ (код возврата: $RESULT)${NC}"
    echo "=============================================="
    exit 1
fi