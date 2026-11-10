#include <iostream>
#include <string>
#include <cassert>   

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_DIFFICULTY = 1;
const int SET_DUNGEON_SIZE = 2;
const int SET_LIVES = 3;
const int SET_BACK = 4;

const string PROGRAM_VERSION = "1.0";
const string STUDENT_NAME = "Тересюк Д. Ю.";

struct GameSettings {
    int difficulty = 1;
    int dungeonSize = 10;
    int lives = 3;
};

bool testDefaults() {
    GameSettings s;
    assert(s.difficulty == 1);
    assert(s.dungeonSize == 10);
    assert(s.lives == 3);
    return true;
}

bool testMainMenuConstants() {
    assert(MENU_START_GAME == 1);
    assert(MENU_LOAD_GAME == 2);
    assert(MENU_SETTINGS == 3);
    assert(MENU_ABOUT == 4);
    assert(MENU_EXIT == 5);
    return true;
}

bool testSettingsConstants() {
    assert(SET_DIFFICULTY == 1);
    assert(SET_DUNGEON_SIZE == 2);
    assert(SET_LIVES == 3);
    assert(SET_BACK == 4);
    return true;
}

bool testSettingValues() {
    GameSettings s;
    s.difficulty = 3;   assert(s.difficulty == 3);
    s.dungeonSize = 50; assert(s.dungeonSize == 50);
    s.lives = 7;        assert(s.lives == 7);
    return true;
}

bool testBoundaries() {
    GameSettings s;
    s.dungeonSize = 5;   assert(s.dungeonSize == 5);
    s.dungeonSize = 100; assert(s.dungeonSize == 100);
    s.lives = 1;         assert(s.lives == 1);
    s.lives = 10;        assert(s.lives == 10);
    return true;
}

bool testValidRanges() {
    auto validDifficulty = [](int d) { return d >= 1 && d <= 3; };
    auto validDungeonSize = [](int s) { return s >= 5 && s <= 100; };
    auto validLives = [](int l) { return l >= 1 && l <= 10; };

    assert(validDifficulty(1) && validDifficulty(2) && validDifficulty(3));
    assert(!validDifficulty(0) && !validDifficulty(4));
    assert(validDungeonSize(5) && validDungeonSize(100));
    assert(!validDungeonSize(4) && !validDungeonSize(101));
    assert(validLives(1) && validLives(10));
    assert(!validLives(0) && !validLives(11));
    return true;
}

bool testMetadata() {
    assert(!PROGRAM_VERSION.empty());
    assert(!STUDENT_NAME.empty());
    assert(PROGRAM_VERSION == "1.0");
    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");
    cout << " ЗАПУСК ТЕСТОВ \n\n";

    testDefaults();
    cout << "[OK] testDefaults\n";

    testMainMenuConstants();
    cout << "[OK] testMainMenuConstants\n";

    testSettingsConstants();
    cout << "[OK] testSettingsConstants\n";

    testSettingValues();
    cout << "[OK] testSettingValues\n";

    testBoundaries();
    cout << "[OK] testBoundaries\n";

    testValidRanges();
    cout << "[OK] testValidRanges\n";

    testMetadata();
    cout << "[OK] testMetadata\n";

    cout << "\n ВСЕ ТЕСТЫ ПРОЙДЕНЫ \n";
    return 0;
}