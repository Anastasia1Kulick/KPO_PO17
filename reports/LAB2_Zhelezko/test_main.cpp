#define main original_main
#include "Main.cpp"
#undef main

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

using namespace std;

static istringstream g_in;
static ostringstream g_out;
static streambuf* g_oldCin = nullptr;
static streambuf* g_oldCout = nullptr;

void redirectStart(const string& input) {
    g_in.str(input);
    g_in.clear();
    g_out.str("");
    g_out.clear();
    g_oldCin = cin.rdbuf(g_in.rdbuf());
    g_oldCout = cout.rdbuf(g_out.rdbuf());
}

void redirectStop() {
    cin.rdbuf(g_oldCin);
    cout.rdbuf(g_oldCout);
}

string redirectOutput() {
    return g_out.str();
}

map<string, int> getDefaultLevels() {
    map<string, int> levels;
    levels["Легкий"] = 0;
    levels["Средний"] = 101;
    levels["Сложный"] = 401;
    return levels;
}

void test_calculateDifficulty() {
    auto levels = getDefaultLevels();
    assert(calculateDifficulty(5, 5, levels) == "Легкий");
    assert(calculateDifficulty(10, 10, levels) == "Легкий");
    assert(calculateDifficulty(10, 11, levels) == "Средний");
    assert(calculateDifficulty(20, 20, levels) == "Средний");
    assert(calculateDifficulty(20, 21, levels) == "Сложный");
    assert(calculateDifficulty(50, 50, levels) == "Сложный");
    cout << "test_calculateDifficulty passed\n";
}

void test_updateDifficulty() {
    auto levels = getDefaultLevels();
    GameSettings s;
    s.mazeWidth = 15; s.mazeHeight = 15;
    updateDifficulty(s, levels);
    assert(s.difficulty == "Средний");

    s.mazeWidth = 5; s.mazeHeight = 5;
    updateDifficulty(s, levels);
    assert(s.difficulty == "Легкий");

    s.mazeWidth = 25; s.mazeHeight = 25;
    updateDifficulty(s, levels);
    assert(s.difficulty == "Сложный");

    cout << "test_updateDifficulty passed\n";
}

void test_readInt_valid() {
    redirectStart("42\n");
    int v = readInt("Введите: ");
    redirectStop();
    assert(v == 42);
    cout << "test_readInt_valid passed\n";
}

void test_readInt_invalid_then_valid() {
    redirectStart("abc\n42\n");
    int v = readInt("Введите: ");
    string out = redirectOutput();
    redirectStop();
    assert(v == 42);
    assert(out.find("Ошибка") != string::npos);
    cout << "test_readInt_invalid_then_valid passed\n";
}

void test_waitEnter() {
    redirectStart("\n");
    waitEnter();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Нажмите Enter") != string::npos);
    cout << "test_waitEnter passed\n";
}

void test_startGame() {
    redirectStart("\n");
    startGame();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Начать игру") != string::npos);
    assert(out.find("Побег из лабиринта") != string::npos);
    cout << "test_startGame passed\n";
}

void test_loadGame() {
    redirectStart("\n");
    loadGame();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("недоступна") != string::npos);
    cout << "test_loadGame passed\n";
}

void test_about() {
    redirectStart("\n");
    about();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Версия: 2.0") != string::npos);
    assert(out.find("Железко Андрей") != string::npos);
    cout << "test_about passed\n";
}

void test_changeMazeSize_valid() {
    auto levels = getDefaultLevels();
    GameSettings s;
    redirectStart("20\n30\n\n");
    changeMazeSize(s, levels);
    redirectStop();
    assert(s.mazeWidth == 20);
    assert(s.mazeHeight == 30);
    assert(s.difficulty == "Сложный");
    cout << "test_changeMazeSize_valid passed\n";
}

void test_changeMazeSize_invalid_width() {
    auto levels = getDefaultLevels();
    GameSettings s;
    s.mazeWidth = 10; s.mazeHeight = 10; s.difficulty = "Легкий";
    redirectStart("4\n\n");
    changeMazeSize(s, levels);
    redirectStop();
    assert(s.mazeWidth == 10);
    assert(s.mazeHeight == 10);
    assert(s.difficulty == "Легкий");
    cout << "test_changeMazeSize_invalid_width passed\n";
}

void test_changeMazeSize_invalid_height() {
    auto levels = getDefaultLevels();
    GameSettings s;
    s.mazeWidth = 10; s.mazeHeight = 10; s.difficulty = "Легкий";
    redirectStart("20\n51\n\n");
    changeMazeSize(s, levels);
    redirectStop();
    assert(s.mazeWidth == 10);
    assert(s.mazeHeight == 10);
    assert(s.difficulty == "Легкий");
    cout << "test_changeMazeSize_invalid_height passed\n";
}

void test_settingsMenu_show_and_back() {
    auto levels = getDefaultLevels();
    GameSettings s;
    redirectStart("1\n\n3\n");
    settingsMenu(s, levels);
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Текущие настройки") != string::npos);
    assert(out.find("Сложность: Легкий") != string::npos);
    cout << "test_settingsMenu_show_and_back passed\n";
}

void test_settingsMenu_change_size() {
    auto levels = getDefaultLevels();
    GameSettings s;
    redirectStart("2\n15\n15\n\n3\n");
    settingsMenu(s, levels);
    redirectStop();
    assert(s.mazeWidth == 15);
    assert(s.mazeHeight == 15);
    assert(s.difficulty == "Средний");
    cout << "test_settingsMenu_change_size passed\n";
}

void test_settingsMenu_invalid_choice() {
    auto levels = getDefaultLevels();
    GameSettings s;
    redirectStart("99\n\n3\n");
    settingsMenu(s, levels);
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Неверный пункт меню") != string::npos);
    cout << "test_settingsMenu_invalid_choice passed\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    test_calculateDifficulty();
    test_updateDifficulty();
    test_readInt_valid();
    test_readInt_invalid_then_valid();
    test_waitEnter();
    test_startGame();
    test_loadGame();
    test_about();
    test_changeMazeSize_valid();
    test_changeMazeSize_invalid_width();
    test_changeMazeSize_invalid_height();
    test_settingsMenu_show_and_back();
    test_settingsMenu_change_size();
    test_settingsMenu_invalid_choice();

    cout << "\nAll tests passed!\n";
    return 0;
}