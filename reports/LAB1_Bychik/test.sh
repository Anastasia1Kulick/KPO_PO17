#!/bin/bash
# =========================================================
#  Сборка и запуск тестов для лабораторной работы №1
#  Работает в Git Bash / MSYS2 / WSL под Windows.
# =========================================================

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}>>> Сборка основной программы (main.cpp)...${NC}"
g++ -std=c++17 -Wall -O2 main.cpp -o farm
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки main.cpp${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Сборка тестов (test.cpp)...${NC}"
g++ -std=c++17 -Wall -O2 test.cpp -o tests
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки test.cpp${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Запуск тестов...${NC}\n"
./tests
RESULT=$?

# Чистим временные файлы
rm -f input.txt output.txt

if [ $RESULT -eq 0 ]; then
    echo -e "\n${GREEN}Все тесты пройдены успешно.${NC}"
else
    echo -e "\n${RED}Некоторые тесты провалены.${NC}"
fi

exit $RESULT
