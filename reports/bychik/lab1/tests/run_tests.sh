#!/bin/bash

set -u

# ---------- Настройки (пути ОТНОСИТЕЛЬНО папки лабы!) ----------
SRC="src/main.cpp"
TEST_SRC="tests/test_main.cpp"
UNIT_BIN="./tests/unit_tests"
GAME_BIN="./tests/game_bin"
BUILD_LOG="./tests/build.log"

PASS=0
FAIL=0

# ---------- Сборка основной программы ----------
echo "=== Сборка основной программы ==="
g++ -std=c++17 -Wall -o "$GAME_BIN" "$SRC" 2> "$BUILD_LOG"
if [ $? -ne 0 ]; then
    echo "[ОШИБКА] Компиляция main.cpp не удалась. Лог:"
    cat "$BUILD_LOG"
    exit 1
fi
echo "Сборка main.cpp успешна."
echo

# ---------- Сборка модульных тестов ----------
echo "=== Сборка модульных тестов ==="
g++ -std=c++17 -Wall -o "$UNIT_BIN" "$TEST_SRC" 2>> "$BUILD_LOG"
if [ $? -ne 0 ]; then
    echo "[ОШИБКА] Компиляция test_solution.cpp не удалась. Лог:"
    cat "$BUILD_LOG"
    exit 1
fi
echo "Сборка тестов успешна."
echo

# ---------- Запуск модульных тестов ----------
echo "=== Модульные тесты ==="
"$UNIT_BIN" < /dev/null
UNIT_RESULT=$?
if [ $UNIT_RESULT -eq 0 ]; then
    echo "[OK]   Модульные тесты пройдены"
    PASS=$((PASS+1))
else
    echo "[FAIL] Модульные тесты провалены (код $UNIT_RESULT)"
    FAIL=$((FAIL+1))
fi
echo

# ---------- Функциональные тесты (stdin → программа) ----------
run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    echo "--- Тест: $name ---"
    output=$(printf "%b" "$input" | "$GAME_BIN" 2>&1)
    if echo "$output" | grep -qF "$expected"; then
        echo "[OK]   Найдено: $expected"
        PASS=$((PASS+1))
    else
        echo "[FAIL] Не найдено: $expected"
        echo "       Первые строки вывода:"
        echo "$output" | head -5 | sed 's/^/       /'
        FAIL=$((FAIL+1))
    fi
    echo
}

run_test "Выход из главного меню"            "5\n"                "Выход из программы"
run_test "Пункт 1 — Начать игру"             "1\n\n5\n"           "Начало игры"
run_test "Пункт 2 — Загрузить игру"          "2\n\n5\n"           "Функция загрузки пока недоступна"
run_test "Пункт 4 — О программе"             "4\n\n5\n"           "О программе"
run_test "Пункт 3 — Открытие настроек"       "3\n4\n5\n"          "Настройки"
run_test "Изменение специальности"           "3\n1\nФизика\n4\n5\n" "Физика"
run_test "Изменение имени студента"          "3\n2\nАлексей\n4\n5\n" "Алексей"
run_test "Изменение количества предметов"    "3\n3\n7\n4\n5\n"    "7"
run_test "Некорректный ввод (буквы)"         "abc\n5\n"           "Некорректный ввод"
run_test "Отрицательное количество предметов" "3\n3\n-1\n4\n5\n" "Некорректное значение"
run_test "Ноль предметов"                    "3\n3\n0\n4\n5\n"    "Некорректное значение"
run_test "Несуществующий пункт меню"         "99\n5\n"            "Такого пункта нет"

echo "======================================"
echo "Итог: пройдено $PASS, провалено $FAIL"
echo "======================================"

# Уборка артефактов (в tests/, чтобы не мусорить в корне лабы)
rm -f "$GAME_BIN" "$UNIT_BIN" "$BUILD_LOG"

if [ "$FAIL" -eq 0 ]; then
    exit 0
else
    exit 1
fi
