#!/bin/bash
set -u

SRC="../src/main.cpp"
BIN="./game_test"
BUILD_LOG="build.log"
PASS=0
FAIL=0

echo "=== Сборка проекта ==="
g++ -std=c++17 -Wall -o "$BIN" "$SRC" 2> "$BUILD_LOG"
if [ $? -ne 0 ]; then
    echo "[ОШИБКА] Компиляция не удалась. Лог:"
    cat "$BUILD_LOG"
    exit 1
fi
echo "Сборка успешна."
echo

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"
    
    echo "--- Тест: $name ---"
    output=$(printf "%b" "$input" | "$BIN" 2>&1)
    
    if echo "$output" | grep -qF "$expected"; then
        echo "[OK]   Найдено: $expected"
        PASS=$((PASS+1))
    else
        echo "[FAIL] Не найдено: $expected"
        FAIL=$((FAIL+1))
    fi
    echo
}

run_test "Выход из программы" \
    "5\n" \
    "Выход"

run_test "Начать игру" \
    "1\n\n5\n" \
    "Игра началась"

run_test "Загрузить игру" \
    "2\n\n5\n" \
    "не доступна"

echo "======================================"
echo "Пройдено: $PASS"
echo "Провалено: $FAIL"
echo "======================================"

rm -f "$BIN" "$BUILD_LOG"

if [ "$FAIL" -eq 0 ]; then
    exit 0
else
    exit 1
fi
