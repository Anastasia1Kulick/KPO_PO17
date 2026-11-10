#!/bin/bash
set -u
cd "$(dirname "$0")" || exit 1

TEST_SRC="tests/test_main.cpp"
SRC_FILES="settings.cpp logger.cpp"
TEST_BIN="tests/tests"
TMP_FILES=("test_settings.txt" "test.log")

GREEN="\033[0;32m"; RED="\033[0;31m"; NC="\033[0m"

echo "=============================================="
echo " СБОРКА ТЕСТОВ (ЛР3)"
echo "=============================================="

for f in "$TEST_SRC" $SRC_FILES; do
    if [ ! -f "$f" ]; then
        echo -e "${RED}Ошибка: файл '$f' не найден!${NC}"
        exit 1
    fi
done

g++ -std=c++17 -Wall -Wextra "$TEST_SRC" $SRC_FILES -o "$TEST_BIN"
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

echo -e "${GREEN}Компиляция успешна.${NC}"

"$TEST_BIN"
RESULT=$?

for f in "${TMP_FILES[@]}"; do
    [ -f "$f" ] && rm -f "$f"
done

if [ $RESULT -eq 0 ]; then
    echo -e "${GREEN}РЕЗУЛЬТАТ: ВСЕ ТЕСТЫ ПРОЙДЕНЫ${NC}"
    exit 0
else
    echo -e "${RED}РЕЗУЛЬТАТ: ТЕСТЫ ПРОВАЛЕНЫ (код: $RESULT)${NC}"
    exit 1
fi