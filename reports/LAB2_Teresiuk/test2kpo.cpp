#include "../settings.h"
#include "../logger.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <stack>

void test_valid_dungeon() {
    assert(isValidDungeonSize(5));
    assert(isValidDungeonSize(100));
    assert(!isValidDungeonSize(4));
    assert(!isValidDungeonSize(101));
    std::cout << "[OK] valid_dungeon\n";
}

void test_valid_lives() {
    assert(isValidLives(1));
    assert(isValidLives(10));
    assert(!isValidLives(0));
    assert(!isValidLives(11));
    std::cout << "[OK] valid_lives\n";
}

void test_valid_difficulty() {
    assert(isValidDifficulty("Легко"));
    assert(isValidDifficulty("Средне"));
    assert(isValidDifficulty("Сложно"));
    assert(!isValidDifficulty("легко"));
    assert(!isValidDifficulty(""));
    std::cout << "[OK] valid_difficulty\n";
}

void test_save_and_load() {
    GameSettings s1;
    s1.dungeonSize = 42;
    s1.lives = 7;
    s1.difficulty = "Сложно";

    assert(saveSettings(s1, "test_settings.txt") == true);

    GameSettings s2;
    assert(loadSettings(s2, "test_settings.txt") == true);
    assert(s2.dungeonSize == 42);
    assert(s2.lives == 7);
    assert(s2.difficulty == "Сложно");

    std::remove("test_settings.txt");
    std::cout << "[OK] save/load\n";
}

void test_load_missing_file() {
    GameSettings s;
    assert(loadSettings(s, "no_such_file_12345.txt") == false);
    std::cout << "[OK] load missing file\n";
}

void test_stack_history() {
    std::stack<int> hist;
    pushSizeHistory(hist, 10);
    pushSizeHistory(hist, 20);
    pushSizeHistory(hist, 30);

    int prev;
    assert(popSizeHistory(hist, prev) && prev == 30);
    assert(popSizeHistory(hist, prev) && prev == 20);
    assert(popSizeHistory(hist, prev) && prev == 10);
    assert(popSizeHistory(hist, prev) == false);
    std::cout << "[OK] stack history\n";
}

void test_setters() {
    GameSettings s;
    assert(trySetLives(s, 7) && s.lives == 7);
    assert(!trySetLives(s, 99) && s.lives == 7);
    assert(trySetDungeonSize(s, 50) && s.dungeonSize == 50);
    assert(!trySetDungeonSize(s, 3) && s.dungeonSize == 50);
    assert(trySetDifficulty(s, "Средне") && s.difficulty == "Средне");
    assert(!trySetDifficulty(s, "легко") && s.difficulty == "Средне");
    std::cout << "[OK] setters\n";
}

int main() {
    Logger::init("test.log");
    std::cout << "=== Unit-тесты ЛР2 ===\n";

    test_valid_dungeon();
    test_valid_lives();
    test_valid_difficulty();
    test_save_and_load();
    test_load_missing_file();
    test_stack_history();
    test_setters();

    std::cout << "\n=== Все тесты пройдены ===\n";
    return 0;
}