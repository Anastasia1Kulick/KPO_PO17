#!/usr/bin/env bash

set -euo pipefail

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$LAB_DIR"

CXX="${CXX:-g++}"

echo "ТЕСТЫ ЛР №3"
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

rm -f settings.txt

echo "--- test_start_game_flow ---"
OUT=$(printf "1\nTestCap\n5\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Добро пожаловать, капитан TestCap" "ввод имени капитана"
check_contains "$OUT" "=== МЕНЮ КАПИТАНА ===" "меню капитана"
check_contains "$OUT" "Нанят спортсмен: Новичок" "начальный спортсмен"

echo ""
echo "--- test_hire_sportsman ---"
OUT=$(printf "1\nCap\n1\n5\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Нанят спортсмен: Спортсмен" "найм спортсмена"

echo ""
echo "--- test_competition_finish ---"
OUT=$(printf "1\nCap\n4\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "--- СОРЕВНОВАНИЕ ---" "соревнование началось"
check_contains "$OUT" "Общий счёт вашей команды" "счёт подсчитан"
check_contains "$OUT" "Игра завершена!" "игра завершилась"

echo ""
echo "--- test_difficulty_in_competition ---"
printf "difficulty=Сложный\ntotalPlayers=2\nalivePlayers=2\n" > settings.txt
OUT=$(printf "1\nCap\n4\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Сложность игры: Сложный" "сложность из файла"
check_contains "$OUT" "(сложность x1)" "модификатор x1 для сложного"

rm -f "$GAME_BIN" settings.txt

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