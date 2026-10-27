#!/bin/bash
set -e

if [ ! -f "lab3kpo_tests.exe" ]; then
    echo "Ошибка: lab3kpo_tests.exe не найден."
    echo "Соберите проект lab3kpo_tests в Visual Studio."
    exit 1
fi

echo "===> Запуск тестов..."
./lab3kpo_tests.exe
echo "===> Готово."