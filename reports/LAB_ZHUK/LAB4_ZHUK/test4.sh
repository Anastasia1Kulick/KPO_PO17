#!/usr/bin/env bash

set -euo pipefail

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$LAB_DIR"

CXX="${CXX:-g++}"

echo "ТЕСТЫ ЛР №4"
echo ""
echo "ЭТАП 1: ЮНИТ-ТЕСТЫ"
echo ""

if ! "$CXX" -std=c++17 -Wall -Wextra -O2 \
    -o tests/unit_runner tests/test_unit.cpp; then
    echo "Ошибка компиляции юнит-тестов"
    exit 1
fi

set +e
./tests/unit_runner
UNIT_RESULT=$?
set -e

rm -f tests/unit_runner

if [ "$UNIT_RESULT" -ne 0 ]; then
    echo "ЮНИТ-ТЕСТЫ ПРОВАЛЕНЫ"
    exit "$UNIT_RESULT"
fi

echo ""
echo "ЭТАП 2: ИНТЕГРАЦИОННЫЕ ТЕСТЫ"
echo ""

GAME_BIN="tests/game_bin"
PASSED=0
FAILED=0

pass() { echo "  [PASS] $1"; PASSED=$((PASSED+1)); }
fail() { echo "  [FAIL] $1"; FAILED=$((FAILED+1)); }

check_contains() {
    if echo "$1" | grep -qF "$2"; then pass "$3"; else fail "$3 (не найдено: '$2')"; fi
}

g++ -std=c++17 -O2 -o "$GAME_BIN" src/main.cpp 2>/dev/null
if [ ! -f "$GAME_BIN" ]; then
    echo "Ошибка сборки"
    exit 1
fi

rm -f settings.txt savegame.txt

echo "--- test_main_menu ---"
OUT=$(echo "5" | "$GAME_BIN")
check_contains "$OUT" "СОРЕВНОВАНИЕ АРМИЙ"   "заголовок"
check_contains "$OUT" "1. Начать игру"        "пункт 1"
check_contains "$OUT" "2. Загрузить игру"     "пункт 2"
check_contains "$OUT" "3. Настройки"          "пункт 3"
check_contains "$OUT" "4. О программе"        "пункт 4"
check_contains "$OUT" "5. Выход"              "пункт 5"

echo ""
echo "--- test_about_v4 ---"
OUT=$(printf "4\n4\n5\n" | "$GAME_BIN")
check_contains "$OUT" "v4.0"                  "версия v4.0"
check_contains "$OUT" "Жук И.В."              "ФИО студента"

echo ""
echo "--- test_savegame_created ---"
# Имитируем игру: нанять спортсмена (1), сохранить (6), выйти (5)
printf "1\nКапитан\n1\n6\n5\n1\n5\n" | "$GAME_BIN" > /dev/null
if [ -f savegame.txt ]; then
    pass "savegame.txt создан"
else
    fail "savegame.txt НЕ создан"
fi

echo ""
echo "--- test_loadgame_works ---"
# Загружаем игру (2), выходим (5)
OUT=$(printf "2\n5\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Игра успешно загружена"  "успешная загрузка"
check_contains "$OUT" "Капитан: Капитан"       "имя капитана восстановлено"
check_contains "$OUT" "МЕНЮ КАПИТАНА"          "сразу перешли в игру"

rm -f "$GAME_BIN" settings.txt savegame.txt

echo ""
echo "Пройдено:  $PASSED"
echo "Провалено: $FAILED"

if [ "$FAILED" -eq 0 ]; then
    echo "ВСЕ ТЕСТЫ ПРОЙДЕНЫ"
    exit 0
else
    echo "ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ"
    exit 1
fi