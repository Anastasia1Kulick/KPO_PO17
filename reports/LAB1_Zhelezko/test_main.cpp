#define main original_main
#include "Main.cpp"
#undef main

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>

static std::istringstream g_in;
static std::ostringstream g_out;
static std::streambuf* g_oldCin = nullptr;
static std::streambuf* g_oldCout = nullptr;

void redirectStart(const std::string& input) {
    g_in.str(input);
    g_in.clear();
    g_out.str("");
    g_out.clear();
    g_oldCin = std::cin.rdbuf(g_in.rdbuf());
    g_oldCout = std::cout.rdbuf(g_out.rdbuf());
}

void redirectStop() {
    std::cin.rdbuf(g_oldCin);
    std::cout.rdbuf(g_oldCout);
}

std::string redirectOutput() {
    return g_out.str();
}

void test_calculateDifficulty() {
    assert(calculateDifficulty(5, 5) == "Легкий");
    assert(calculateDifficulty(10, 10) == "Легкий");   // 100
    assert(calculateDifficulty(10, 11) == "Средний");  // 110
    assert(calculateDifficulty(20, 20) == "Средний");  // 400
    assert(calculateDifficulty(20, 21) == "Сложный");  // 420
    assert(calculateDifficulty(50, 50) == "Сложный");  // 2500
    std::cout << "test_calculateDifficulty passed\n";
}

void test_updateDifficulty() {
    GameSettings s;
    s.mazeWidth = 15; s.mazeHeight = 15;
    updateDifficulty(s);
    assert(s.difficulty == "Средний"); // 225

    s.mazeWidth = 5; s.mazeHeight = 5;
    updateDifficulty(s);
    assert(s.difficulty == "Легкий");  // 25

    s.mazeWidth = 25; s.mazeHeight = 25;
    updateDifficulty(s);
    assert(s.difficulty == "Сложный"); // 625
    std::cout << "test_updateDifficulty passed\n";
}

void test_readInt_valid() {
    redirectStart("42\n");
    int v = readInt("Введите: ");
    redirectStop();
    assert(v == 42);
    std::cout << "test_readInt_valid passed\n";
}

void test_readInt_invalid_then_valid() {
    redirectStart("abc\n42\n");
    int v = readInt("Введите: ");
    std::string out = redirectOutput();
    redirectStop();
    assert(v == 42);
    assert(out.find("Ошибка") != std::string::npos);
    std::cout << "test_readInt_invalid_then_valid passed\n";
}

void test_waitEnter() {
    redirectStart("\n");
    waitEnter();
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("Нажмите Enter") != std::string::npos);
    std::cout << "test_waitEnter passed\n";
}

void test_startGame() {
    redirectStart("\n");
    startGame();
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("Начать игру") != std::string::npos);
    assert(out.find("Побег из лабиринта") != std::string::npos);
    std::cout << "test_startGame passed\n";
}

void test_loadGame() {
    redirectStart("\n");
    loadGame();
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("недоступна") != std::string::npos);
    std::cout << "test_loadGame passed\n";
}

void test_about() {
    redirectStart("\n");
    about();
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("Версия: 1.0") != std::string::npos);
    assert(out.find("Железко Андрей") != std::string::npos);
    std::cout << "test_about passed\n";
}

void test_changeMazeSize_valid() {
    GameSettings s;
    redirectStart("20\n30\n\n");
    changeMazeSize(s);
    redirectStop();
    assert(s.mazeWidth == 20);
    assert(s.mazeHeight == 30);
    assert(s.difficulty == "Сложный");
    std::cout << "test_changeMazeSize_valid passed\n";
}

void test_changeMazeSize_invalid_width() {
    GameSettings s;
    s.mazeWidth = 10; s.mazeHeight = 10; s.difficulty = "Легкий";
    redirectStart("4\n\n"); 
    changeMazeSize(s);
    redirectStop();
    assert(s.mazeWidth == 10);
    assert(s.mazeHeight == 10);
    assert(s.difficulty == "Легкий");
    std::cout << "test_changeMazeSize_invalid_width passed\n";
}

void test_changeMazeSize_invalid_height() {
    GameSettings s;
    s.mazeWidth = 10; s.mazeHeight = 10; s.difficulty = "Легкий";
    redirectStart("20\n51\n\n"); 
    changeMazeSize(s);
    redirectStop();
    assert(s.mazeWidth == 10);
    assert(s.mazeHeight == 10);
    assert(s.difficulty == "Легкий");
    std::cout << "test_changeMazeSize_invalid_height passed\n";
}

void test_settingsMenu_show_and_back() {
    GameSettings s;
    redirectStart("1\n\n3\n"); 
    settingsMenu(s);
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("Текущие настройки") != std::string::npos);
    assert(out.find("Сложность: Легкий") != std::string::npos);
    std::cout << "test_settingsMenu_show_and_back passed\n";
}

void test_settingsMenu_change_size() {
    GameSettings s;
    redirectStart("2\n15\n15\n\n3\n"); 
    settingsMenu(s);
    redirectStop();
    assert(s.mazeWidth == 15);
    assert(s.mazeHeight == 15);
    assert(s.difficulty == "Средний"); // 225
    std::cout << "test_settingsMenu_change_size passed\n";
}

void test_settingsMenu_invalid_choice() {
    GameSettings s;
    redirectStart("99\n\n3\n");
    settingsMenu(s);
    std::string out = redirectOutput();
    redirectStop();
    assert(out.find("Неверный пункт меню") != std::string::npos);
    std::cout << "test_settingsMenu_invalid_choice passed\n";
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

    std::cout << "\nAll tests passed!\n";
    return 0;
}
