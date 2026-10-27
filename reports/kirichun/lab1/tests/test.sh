#!/bin/bash

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}   СБОРКА И ЗАПУСК ТЕСТОВ${NC}"
echo -e "${YELLOW}========================================${NC}"

COMPILER=""
if command -v g++ >/dev/null 2>&1; then
    COMPILER="g++"
elif command -v clang++ >/dev/null 2>&1; then
    COMPILER="clang++"
else
    echo -e "${RED}ОШИБКА: не найден компилятор g++ или clang++${NC}"
    exit 1
fi

echo -e "${YELLOW}[1/4] Компилятор: $COMPILER${NC}"

if [ -f "main.cpp" ]; then
    echo -e "${YELLOW}[2/4] Проверка синтаксиса main.cpp...${NC}"
    if $COMPILER -fsyntax-only -std=c++17 main.cpp 2>/dev/null; then
        echo -e "${GREEN}      Синтаксис main.cpp корректен${NC}"
    else
        echo -e "${RED}      Синтаксическая ошибка в main.cpp${NC}"
    fi
else
    echo -e "${YELLOW}[2/4] main.cpp не найден — пропуск проверки синтаксиса${NC}"
fi

echo -e "${YELLOW}[3/4] Компиляция test.cpp...${NC}"
if $COMPILER -std=c++17 -Wall -Wextra test.cpp -o test_runner 2>&1; then
    echo -e "${GREEN}      Компиляция успешна${NC}"
else
    echo -e "${RED}      ОШИБКА компиляции тестов${NC}"
    exit 1
fi

echo -e "${YELLOW}[4/4] Запуск тестов...${NC}"
echo ""

if ./test_runner; then
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}   ВСЕ ТЕСТЫ ПРОЙДЕНЫ${NC}"
    echo -e "${GREEN}========================================${NC}"
    EXIT_CODE=0
else
    echo ""
    echo -e "${RED}========================================${NC}"
    echo -e "${RED}   ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ${NC}"
    echo -e "${RED}========================================${NC}"
    EXIT_CODE=1
fi

rm -f test_runner 2>/dev/null || true

exit $EXIT_CODE
