#!/bin/bash
set -e

if [ ! -f "lab4kpo_tests.exe" ]; then
    echo "Ошибка: lab4kpo_tests.exe не найден."
    echo "Соберите проект lab4kpo_tests в Visual Studio."
    exit 1
fi

echo "===> Запуск тестов..."
./lab4kpo_tests.exe
echo "===> Готово."