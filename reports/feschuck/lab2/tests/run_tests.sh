#!/usr/bin/env bash
set -euo pipefail

# Переходим в корень лабы (из папки tests в lab2)
cd "$(dirname "$0")/.."

# Проверка наличия g++
if ! command -v g++ >/dev/null 2>&1; then
    echo "ОШИБКА: g++ не найден" >&2
    exit 2
fi

# Создаем временную директорию, чтобы файлы settings.txt не мусорили в репозитории
WORKDIR="$(mktemp -d -t kpo2_test_XXXXXX)"
trap 'rm -rf "$WORKDIR"' EXIT
echo "=== Рабочая директория: $WORKDIR ==="

# Копируем исходники и тесты во временную папку
cp src/main.cpp "$WORKDIR/"
cp tests/test_main.cpp "$WORKDIR/"
cd "$WORKDIR"

# Сборка
echo "=== Сборка test_main.cpp ==="
g++ -std=c++17 -Wall -Wextra -O2 test_main.cpp -o test_bin
echo "Сборка успешна."

# Запуск (с таймаутом)
echo
echo "=== Запуск тестов ==="
set +e
timeout 30 ./test_bin
RC=$?
set -e

# Диагностика
echo
if [[ "$RC" -eq 124 ]]; then
    echo "=== ТАЙМАУТ: тесты не завершились за 30 с ===" >&2
elif [[ "$RC" -eq 0 ]]; then
    echo "=== РЕЗУЛЬТАТ: все тесты пройдены ==="
else
    echo "=== РЕЗУЛЬТАТ: есть проваленные тесты (код $RC) ===" >&2
fi
exit "$RC"
