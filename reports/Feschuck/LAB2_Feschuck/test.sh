#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# --- Проверки окружения ---
if ! command -v g++ >/dev/null 2>&1; then
    echo "ОШИБКА: g++ не найден" >&2
    exit 2
fi

for f in КПО2АР.cpp test.cpp; do
    if [[ ! -f "$f" ]]; then
        echo "ОШИБКА: не найден $f" >&2
        exit 2
    fi
done

# --- Временная директория ---
WORKDIR="$(mktemp -d -t kpo2_test_XXXXXX)"
trap 'rm -rf "$WORKDIR"' EXIT
echo "=== Рабочая директория: $WORKDIR ==="

cp КПО2АР.cpp test.cpp "$WORKDIR/"
cd "$WORKDIR"

# --- Сборка ---
echo "=== Сборка test.cpp ==="
g++ -std=c++17 -Wall -Wextra -O2 test.cpp -o test_bin
echo "Сборка успешна."

# --- Запуск (с таймаутом на случай зависания) ---
echo
echo "=== Запуск тестов ==="
set +e
timeout 30 ./test_bin
RC=$?
set -e

# --- Диагностика ---
echo
if [[ "$RC" -eq 124 ]]; then
    echo "=== ТАЙМАУТ: тесты не завершились за 30 с ===" >&2
elif [[ "$RC" -eq 0 ]]; then
    echo "=== РЕЗУЛЬТАТ: все тесты пройдены ==="
else
    echo "=== РЕЗУЛЬТАТ: есть проваленные тесты (код $RC) ===" >&2
fi
exit "$RC"
