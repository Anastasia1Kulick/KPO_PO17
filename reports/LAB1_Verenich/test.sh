#!/bin/bash

# ============================================================
#  Обёртка: собирает программу и C++ тесты, потом запускает
# ============================================================

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}>>> Сборка основной программы...${NC}"
g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o farm
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки main.cpp${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Сборка C++ тестов...${NC}"
g++ -std=c++17 -Wall -Wextra -O2 tests.cpp -o tests
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки tests.cpp${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Запуск тестов...${NC}\n"
./tests
exit $?