#!/usr/bin/env bash

set -euo pipefail

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$LAB_DIR"

CXX="${CXX:-g++}"

echo "ТЕСТЫ ЛР №2"
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
check_not_contains() {
    if echo "$1" | grep -qF "$2"; then fail "$3 (неожиданно: '$2')"; else pass "$3"; fi
}
check_regex() {
    if echo "$1" | grep -qE "$2"; then pass "$3"; else fail "$3"; fi
}

g++ -std=c++17 -O2 -o "$GAME_BIN" src/main.cpp 2>/dev/null
if [ ! -f "$GAME_BIN" ]; then
    echo "Ошибка сборки"
    exit 1
fi

rm -f settings.txt

echo "--- test_first_launch_creates_file ---"
OUT=$(echo "5" | "$GAME_BIN")
if [ -f settings.txt ]; then
    pass "settings.txt создан при первом запуске"
else
    fail "settings.txt НЕ создан"
fi
check_contains "$(cat settings.txt)" "difficulty=Легкий" "сложность по умолчанию"
check_contains "$(cat settings.txt)" "totalPlayers=2"    "всего 2"
check_contains "$(cat settings.txt)" "alivePlayers=2"    "живых 2"

echo ""
echo "--- test_main_menu ---"
OUT=$(echo "5" | "$GAME_BIN")
check_contains "$OUT" "СОРЕВНОВАНИЕ АРМИЙ"   "заголовок"
check_contains "$OUT" "1. Начать игру"        "пункт 1"
check_contains "$OUT" "2. Загрузить игру"     "пункт 2"
check_contains "$OUT" "3. Настройки"          "пункт 3"
check_contains "$OUT" "4. О программе"        "пункт 4"
check_contains "$OUT" "5. Выход"              "пункт 5"

echo ""
echo "--- test_about_v2 ---"
OUT=$(printf "4\n4\n5\n" | "$GAME_BIN")
check_contains "$OUT" "О ПРОГРАММЕ"           "заголовок"
check_contains "$OUT" "v2.0"                  "версия v2.0"
check_contains "$OUT" "Жук И.В."              "ФИО студента"
check_not_contains "$OUT" "Группа"            "нет 'Группа'"
check_not_contains "$OUT" "Дисциплина"        "нет 'Дисциплина'"

echo ""
echo "--- test_load_game ---"
OUT=$(printf "2\n2\n5\n" | "$GAME_BIN")
check_contains "$OUT" "ЗАГРУЗКА ИГРЫ"         "заголовок"
check_contains "$OUT" "недоступна"            "недоступность"
check_contains "$OUT" "обновлениях"           "обновления"

echo ""
echo "--- test_save_difficulty ---"
OUT=$(printf "3\n1\n3\n1\n5\n5\n" | "$GAME_BIN")
check_contains "$(cat settings.txt)" "difficulty=Сложный" "сложность сохранена в файл"
check_contains "$OUT" "Уровень сложности сохранён в файл" "сообщение о сохранении"

echo ""
echo "--- test_save_total ---"
OUT=$(printf "3\n2\n6\n2\n5\n5\n" | "$GAME_BIN")
check_contains "$(cat settings.txt)" "totalPlayers=6" "всего сохранено в файл"

echo ""
echo "--- test_save_alive ---"
OUT=$(printf "3\n3\n4\n3\n5\n5\n" | "$GAME_BIN")
check_contains "$(cat settings.txt)" "alivePlayers=4" "живых сохранено в файл"

echo ""
echo "--- test_ai_names_set ---"
OUT=$(printf "3\n4\n4\n5\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Иван"       "есть Иван"
check_contains "$OUT" "Кирилл"     "есть Кирилл"
check_contains "$OUT" "Даниил"     "есть Даниил"
check_contains "$OUT" "Анатолий"   "есть Анатолий"
check_contains "$OUT" "Андрей"     "есть Андрей"
check_contains "$OUT" "Александр"  "есть Александр"
check_contains "$OUT" "Виолетта"   "есть Виолетта"
check_contains "$OUT" "Глеб"       "есть Глеб"

echo ""
echo "--- test_load_on_second_launch ---"
printf "difficulty=Сложный\ntotalPlayers=8\nalivePlayers=3\n" > settings.txt
OUT=$(printf "1\n1\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Сложность:     Сложный"  "сложность загружена из файла"
check_contains "$OUT" "Всего игроков: 8"        "всего загружено"
check_contains "$OUT" "Живых игроков: 3"        "живых загружено"
check_regex   "$OUT" "ИИ-игроков:[ ]+5"         "ИИ = 8 - 3 = 5"

echo ""
echo "--- test_ai_formula_in_game ---"
printf "difficulty=Легкий\ntotalPlayers=6\nalivePlayers=2\n" > settings.txt
OUT=$(printf "1\n1\n5\n" | "$GAME_BIN")
check_regex "$OUT" "ИИ-игроков:[ ]+4"           "ИИ = 6 - 2 = 4"

echo ""
echo "--- test_invalid_input ---"
OUT=$(printf "99\nabc\n5\n" | "$GAME_BIN")
check_contains "$OUT" "Некорректный ввод"       "обработка '99' и 'abc'"

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