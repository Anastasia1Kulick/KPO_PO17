#!/bin/bash
set -e

if [ ! -f "lab2kpo_tests.exe" ]; then
    echo "Ошибка: lab2kpo_tests.exe не найден."
    echo "Соберите проект lab2kpo_tests в Visual Studio."
    exit 1
fi

echo "===> Запуск тестов..."
./lab2kpo_tests.exe
echo "===> Готово."