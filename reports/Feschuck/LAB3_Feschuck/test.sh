#!/bin/bash
# test.sh — сборка и запуск тестов «Город мечты»
# main.cpp собирается с -DUNIT_TEST: его main отключён,
# используется main из test.cpp

set -u

GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
NC="\033[0m"

BIN="test_runner"

echo "====================================="
echo " СБОРКА ТЕСТОВ"
echo "====================================="

if [ ! -f "main.cpp" ]; then
    echo -e "${RED}Ошибка: main.cpp не найден${NC}"
    exit 1
fi

if [ ! -f "test.cpp" ]; then
    echo -e "${RED}Ошибка: test.cpp не найден${NC}"
    exit 1
fi

g++ -std=c++17 -O2 -Wall -DUNIT_TEST -o "$BIN" main.cpp test.cpp

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки!${NC}"
    exit 1
fi

echo -e "${GREEN}Сборка успешна.${NC}"
echo

echo "====================================="
echo " ЗАПУСК ТЕСТОВ"
echo "====================================="

./"$BIN"
RC=$?

echo
echo "====================================="
echo " РЕЗУЛЬТАТ"
echo "====================================="

if [ $RC -eq 0 ]; then
    echo -e "${GREEN}Все тесты пройдены!${NC}"
else
    echo -e "${RED}Есть проваленные тесты (код $RC).${NC}"
fi

# Очистка бинарника (опционально)
if [ -f "$BIN" ]; then
    rm -f "$BIN"
    echo -e "${YELLOW}Бинарник $BIN удалён.${NC}"
fi
exit $RC
