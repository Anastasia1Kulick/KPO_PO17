#!/bin/bash

# ============================================================
#  Скрипт автотестирования для Лабораторной работы №3
#  Собирает main.cpp, собирает test.cpp и запускает проверки
# ============================================================

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}>>> Сборка основной программы (main.cpp -> farm)...${NC}"

# Флаги -finput-charset и -fexec-charset гарантируют правильную работу с UTF-8 (русский язык)
g++ -std=c++17 -Wall -Wextra -O2 \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    main.cpp -o farm

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки main.cpp!${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Сборка тестов (test.cpp -> tests)...${NC}"

g++ -std=c++17 -Wall -Wextra -O2 \
    -finput-charset=UTF-8 -fexec-charset=UTF-8 \
    test.cpp -o tests

if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка сборки test.cpp!${NC}"
    exit 1
fi

echo -e "${YELLOW}>>> Запуск тестов...${NC}\n"

./tests

# Сохраняем код возврата тестов, чтобы вернуть его из скрипта (для CI/CD)
RESULT=$?

echo ""
if [ $RESULT -eq 0 ]; then
    echo -e "${GREEN}>>> ТЕСТИРОВАНИЕ ЗАВЕРШЕНО УСПЕШНО <<<${NC}"
else
    echo -e "${RED}>>> ТЕСТИРОВАНИЕ ЗАВЕРШЕНО С ОШИБКАМИ <<<${NC}"
fi

exit $RESULT