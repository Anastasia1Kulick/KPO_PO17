#!/bin/bash
# Запуск тестов для lab1kpo.
# Компиляция выполняется в Visual Studio.
# Скрипт просто запускает уже собранный lab1kpo_tests.exe.

set -e

EXE="lab1kpo_tests.exe"

if [ ! -f "$EXE" ]; then
    echo "Ошибка: $EXE не найден в текущей папке."
    echo "Сначала соберите проект lab1kpo_tests в Visual Studio."
    exit 1
fi

echo "===> Запуск тестов..."
./$EXE
echo "===> Готово."