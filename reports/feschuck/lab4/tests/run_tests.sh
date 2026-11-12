#!/usr/bin/env bash
set -u

cd "$(dirname "$0")/.."

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
NC="\033[0m"

BIN="test_runner"

echo "========================================"
echo " СБОРКА ТЕСТОВ"
echo "========================================"

if [ ! -f "src/main.cpp" ]; then
    echo -e "${RED}Ошибка: src/main.cpp не найден!${NC}"
    exit 1
fi

if [ ! -f "tests/test.cpp" ]; then
    echo -e "${RED}Ошибка: tests/test_main.cpp не найден!${NC}"
    exit 1
fi

# Компилируем только test.cpp, так как внутри него уже есть #include "../src/main.cpp"
g++ -std=c++17 -O2 -Wall -DUNIT_TEST -o "$BIN" tests/test_main.cpp

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки!${NC}"
    exit 1
fi

echo -e "${GREEN}Сборка успешна.${NC}"
echo

echo "========================================"
echo " ЗАПУСК ТЕСТОВ"
echo "========================================"

./"$BIN"
RC=$?

echo
echo "========================================"
echo " РЕЗУЛЬТАТ"
echo "========================================"

if [ $RC -eq 0 ]; then
    echo -e "${GREEN}Все тесты пройдены!${NC}"
else
    echo -e "${RED}Есть проваленные тесты (код $RC).${NC}"
fi

if [ -f "$BIN" ]; then
    rm -f "$BIN"
    echo -e "${YELLOW}Бинарник $BIN удалён.${NC}"
fi
exit $RC
