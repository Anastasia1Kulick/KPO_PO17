#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/.."

echo "======================================"
echo "  1. Сборка C++-тестов"
echo "======================================"
g++ -std=c++17 -Wall -Wextra -DTESTING \
    -o /tmp/lab1_tests \
    src/main.cpp tests/test.cpp

echo
echo "======================================"
echo "  2. Запуск C++-юнит-тестов"
echo "======================================"
/tmp/lab1_tests

echo
echo "======================================"
echo "  3. Сборка самой игры (без TESTING)"
echo "======================================"
g++ -std=c++17 -Wall -Wextra -o /tmp/lab1_game src/main.cpp

echo
echo "======================================"
echo "  4. Проверка главного меню"
echo "======================================"
OUTPUT=$(echo "5" | /tmp/lab1_game)
echo "$OUTPUT" | grep -q "ГОРОД МЕЧТЫ"     || { echo "FAIL: нет заголовка"; exit 1; }
echo "$OUTPUT" | grep -q "Начать игру"     || { echo "FAIL: нет пункта 1"; exit 1; }
echo "$OUTPUT" | grep -q "Загрузить игру"  || { echo "FAIL: нет пункта 2"; exit 1; }
echo "$OUTPUT" | grep -q "Настройки"       || { echo "FAIL: нет пункта 3"; exit 1; }
echo "$OUTPUT" | grep -q "О программе"     || { echo "FAIL: нет пункта 4"; exit 1; }
echo "$OUTPUT" | grep -q "Выход"           || { echo "FAIL: нет пункта 5"; exit 1; }
echo "  [OK] Главное меню содержит все 5 пунктов"

echo
echo "======================================"
echo "  5. Проверка пункта «О программе»"
echo "======================================"
# 4 — «О программе», Enter — пауза, 5 — выход
OUTPUT=$(printf "4\n\n5\n" | /tmp/lab1_game)
echo "$OUTPUT" | grep -q "О ПРОГРАММЕ"                        || { echo "FAIL: нет заголовка"; exit 1; }
echo "$OUTPUT" | grep -q "Рекомендация музыкального плейлиста" || { echo "FAIL: нет рекомендации"; exit 1; }
echo "$OUTPUT" | grep -q "City Builder"                       || { echo "FAIL: нет City Builder"; exit 1; }
echo "$OUTPUT" | grep -q "SimCity"                            || { echo "FAIL: нет SimCity"; exit 1; }
echo "$OUTPUT" | grep -q "Lo-Fi"                              || { echo "FAIL: нет Lo-Fi"; exit 1; }
echo "$OUTPUT" | grep -q "Minecraft"                          || { echo "FAIL: нет Minecraft"; exit 1; }
echo "  [OK] Рекомендация плейлиста содержит все 4 пункта"

echo
echo "======================================"
echo "  6. Проверка подменю настроек"
echo "======================================"
# 3 — настройки, 1 — размер города, 4 — назад, 5 — выход
OUTPUT=$(printf "3\n1\n4\n5\n" | /tmp/lab1_game)
echo "$OUTPUT" | grep -q "Размер города"      || { echo "FAIL: нет размера города"; exit 1; }
echo "$OUTPUT" | grep -q "Начальный бюджет"   || { echo "FAIL: нет бюджета"; exit 1; }
echo "$OUTPUT" | grep -q "Количество жителей" || { echo "FAIL: нет жителей"; exit 1; }
echo "  [OK] Подменю настроек содержит все 3 параметра"

echo
echo "======================================"
echo "  7. Проверка выхода из программы"
echo "======================================"
OUTPUT=$(echo "5" | /tmp/lab1_game)
echo "$OUTPUT" | grep -q "Выход из программы" || { echo "FAIL: нет выхода"; exit 1; }
echo "  [OK] Выход из программы работает"

echo
echo "======================================"
echo "  ВСЕ ТЕСТЫ ПРОЙДЕНЫ УСПЕШНО!"
echo "======================================"
exit 0
