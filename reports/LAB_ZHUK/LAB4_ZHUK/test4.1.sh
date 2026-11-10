#!/usr/bin/env bash

set -e

cd "$(dirname "$0")/.."

echo "=== STAGE 1: UNIT TESTS ==="
g++ -std=c++17 -O2 -o tests/unit_runner tests/test_unit.cpp
./tests/unit_runner

echo "=== STAGE 2: INTEGRATION TESTS ==="
g++ -std=c++17 -O2 -o tests/game_bin src/main.cpp

echo "Test: Exit program (Option 5)..."
echo "5" | ./tests/game_bin > /dev/null

echo "Test: About menu (Option 4)..."
printf "4\n4\n5\n" | ./tests/game_bin > /dev/null

echo "Test: Save game (Option 1 -> Save -> Exit)..."
rm -f savegame.txt
# Вводим имя "Captain" латиницей, чтобы не зависеть от кодировки
printf "1\nCaptain\n1\n6\n5\n5\n" | ./tests/game_bin > /dev/null

if [ ! -f savegame.txt ]; then
    echo "ERROR: savegame.txt was not created"
    exit 1
fi

echo "Test: Load game (Option 2)..."
printf "2\n5\n5\n" | ./tests/game_bin > /dev/null

rm -f tests/game_bin tests/unit_runner settings.txt savegame.txt

echo "ALL TESTS PASSED SUCCESSFULLY"