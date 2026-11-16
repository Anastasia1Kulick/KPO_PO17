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

# Проверяем наличие main.cpp и test_main.cpp
if [ ! -f "src/main.cpp" ] || [ ! -f "tests/test_main.cpp" ]; then
    echo -e "${RED}Ошибка: файлы src/main.cpp или tests/test_main.cpp не найдены!${NC}"
    exit 1
fi

# Компилируем test_main.cpp (внутри него уже есть #include "../src/main.cpp")
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
