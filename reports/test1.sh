#!/usr/bin/env bash

set -uo pipefail

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$LAB_DIR"

GAME_BIN="tests/game_bin"
PASSED=0
FAILED=0

pass() { echo "  [PASS] $1"; PASSED=$((PASSED+1)); }
fail() { echo "  [FAIL] $1"; FAILED=$((FAILED+1)); }

check_contains() {
    if echo "$1" | grep -qF "$2"; then pass "$3"; else fail "$3"; fi
}

check_not_contains() {
    if echo "$1" | grep -qF "$2"; then fail "$3"; else pass "$3"; fi
}

g++ -std=c++17 -O2 -o "$GAME_BIN" src/main.cpp

echo "--- test_main_menu ---"
OUT=$(echo "5" | "$GAME_BIN")
check_contains "$OUT" "СОРЕВНОВАНИЕ АРМИЙ"   "заголовок игры"
check_contains "$OUT" "1. Начать игру"        "пункт 1"
check_contains "$OUT" "2. Загрузить игру"     "пункт 2"
check_contains "$OUT" "3. Настройки"          "пункт 3"
check_contains "$OUT" "4. О программе"        "пункт 4"
check_contains "$OUT" "5. Выход"              "пункт 5"

echo ""
echo "--- test_start_game ---"
OUT=$(printf "1\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "НАЧАЛО ИГРЫ"           "заголовок"
check_contains "$OUT" "Игра началась"         "сообщение о начале"
check_contains "$OUT" "Сложность:"            "вывод сложности"
check_contains "$OUT" "Всего игроков:"        "вывод всего"
check_contains "$OUT" "Живых игроков:"        "вывод живых"
check_contains "$OUT" "ИИ-игроков:"           "вывод ИИ"

echo ""
echo "--- test_load_game ---"
OUT=$(printf "2\n2\n5\n" | "$GAME_BIN")
check_contains "$OUT" "ЗАГРУЗКА ИГРЫ"         "заголовок"
check_contains "$OUT" "недоступна"            "недоступность"
check_contains "$OUT" "обновлениях"           "обновления"

echo ""
echo "--- test_about ---"
OUT=$(printf "4\n4\n5\n" | "$GAME_BIN")
check_contains "$OUT" "О ПРОГРАММЕ"           "заголовок"
check_contains "$OUT" "v1.0"                  "версия v1.0"
check_contains "$OUT" "Жук И.В."              "ФИО студента"
check_not_contains "$OUT" "Группа"            "нет 'Группа'"
check_not_contains "$OUT" "Дисциплина"        "нет 'Дисциплина'"

echo ""
echo "--- test_settings_difficulty ---"
OUT=$(printf "3\n1\n3\n1\n5\n5\n" | "$GAME_BIN")
check_contains "$OUT" "НАСТРОЙКИ"             "заголовок"
check_contains "$OUT" "УРОВЕНЬ СЛОЖНОСТИ"     "подменю"
check_contains "$OUT" "Сложный"               "'Сложный'"
check_contains "$OUT" "Уровень сложности сохранён" "сохранение"

echo ""
echo "--- test_settings_total ---"
OUT=$(printf "3\n2\n6\n2\n5\n5\n" | "$GAME_BIN")
check_contains "$OUT" "ВСЕГО ИГРОКОВ"         "подменю"
check_contains "$OUT" "Всего игроков сохранено" "сохранение"

echo ""
echo "--- test_ai_formula ---"
OUT=$(printf "3\n2\n6\n3\n2\n1\n5\n5\n" | "$GAME_BIN")
check_contains "$OUT" "ИИ-игроков:        4"  "ИИ = 6 - 2 = 4"

echo ""
echo "--- test_invalid_input ---"
OUT=$(printf "99\nabc\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Некорректный ввод"     "обработка '99' и 'abc'"

rm -f "$GAME_BIN"

echo ""
echo "Пройдено:  $PASSED"
echo "Провалено: $FAILED"

if [ "$FAILED" -eq 0 ]; then exit 0; else exit 1; fi