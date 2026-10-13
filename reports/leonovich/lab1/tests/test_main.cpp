#define main original_main
#include "../src/main.cpp"
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

// 1. Тест функции Info (О программе)
void test_Info() {
    redirectStart("\n");
    Info();
    std::string out = redirectOutput();
    redirectStop();
    
    assert(out.find("О ПРОГРАММЕ") != std::string::npos);
    assert(out.find("Яна Леонович") != std::string::npos);
    std::cout << "test_Info passed\n";
}

// 2. Тест функции Load (Загрузка игры)
void test_Load() {
    redirectStart("\n");
    Load();
    std::string out = redirectOutput();
    redirectStop();
    
    assert(out.find("недоступна") != std::string::npos);
    std::cout << "test_Load passed\n";
}

// 3. Тест функции Start (Начало игры)
void test_Start() {
    redirectStart("\n");
    Start();
    std::string out = redirectOutput();
    redirectStop();
    
    assert(out.find("Начало игры") != std::string::npos);
    std::cout << "test_Start passed\n";
}

// 4. Тест изменения сложности в настройках (пункт 1 -> выбор 2: Средний -> выход 6)
void test_Settings_ChangeDifficulty() {
    difficulty = DEFAULT_DIFFICULTY;
    allPlayers = DEFAULT_ALL_PLAYERS;
    humanPlayers = DEFAULT_HUMAN_PLAYERS;

    redirectStart("1\n2\n6\n");
    Settings();
    redirectStop();

    assert(difficulty == "Средний");
    std::cout << "test_Settings_ChangeDifficulty passed\n";
}

// 5. Тест неверного выбора сложности (пункт 1 -> ввод 9 (ошибка) -> выход 6)
void test_Settings_InvalidDifficulty() {
    difficulty = "Легкий";

    redirectStart("1\n9\n6\n");
    Settings();
    std::string out = redirectOutput();
    redirectStop();

    assert(difficulty == "Легкий"); // Сложность не должна измениться
    assert(out.find("Неверный выбор!") != std::string::npos);
    std::cout << "test_Settings_InvalidDifficulty passed\n";
}

// 6. Тест изменения общего числа игроков (пункт 2 -> ввод 5 -> выход 6)
void test_Settings_ChangeAllPlayers_Valid() {
    difficulty = DEFAULT_DIFFICULTY;
    allPlayers = DEFAULT_ALL_PLAYERS;
    humanPlayers = DEFAULT_HUMAN_PLAYERS;

    redirectStart("2\n5\n6\n");
    Settings();
    redirectStop();

    assert(allPlayers == 5);
    std::cout << "test_Settings_ChangeAllPlayers_Valid passed\n";
}

// 7. Тест некорректного ввода общего числа игроков (например, 10)
void test_Settings_ChangeAllPlayers_Invalid() {
    allPlayers = 4;
    
    redirectStart("2\n10\n6\n");
    Settings();
    std::string out = redirectOutput();
    redirectStop();

    assert(allPlayers == 4); // Значение не должно измениться
    assert(out.find("Число игроков должно быть от 2 до 5") != std::string::npos);
    std::cout << "test_Settings_ChangeAllPlayers_Invalid passed\n";
}

// 8. Тест изменения живых игроков (пункт 3)
void test_Settings_ChangeHumanPlayers_Valid() {
    allPlayers = 4;
    humanPlayers = 2;

    redirectStart("3\n3\n6\n");
    Settings();
    redirectStop();

    assert(humanPlayers == 3);
    std::cout << "test_Settings_ChangeHumanPlayers_Valid passed\n";
}

// 9. Тест проверки пункта 4 (Игроков ИИ - выводится автоматически)
void test_Settings_AIPlayersInfo() {
    redirectStart("4\n6\n");
    Settings();
    std::string out = redirectOutput();
    redirectStop();

    assert(out.find("Этот параметр считается автоматически") != std::string::npos);
    std::cout << "test_Settings_AIPlayersInfo passed\n";
}

// 10. Тест сброса настроек (пункт 5)
void test_Settings_ResetDefaults() {
    difficulty = "Сложный";
    allPlayers = 5;
    humanPlayers = 4;

    redirectStart("5\n6\n");
    Settings();
    redirectStop();

    assert(difficulty == DEFAULT_DIFFICULTY);
    assert(allPlayers == DEFAULT_ALL_PLAYERS);
    assert(humanPlayers == DEFAULT_HUMAN_PLAYERS);
    std::cout << "test_Settings_ResetDefaults passed\n";
}

int main() {
    setlocale(LC_ALL, "Rus");

    test_Info();
    test_Load();
    test_Start();
    test_Settings_ChangeDifficulty();
    test_Settings_InvalidDifficulty();
    test_Settings_ChangeAllPlayers_Valid();
    test_Settings_ChangeAllPlayers_Invalid();
    test_Settings_ChangeHumanPlayers_Valid();
    test_Settings_AIPlayersInfo();
    test_Settings_ResetDefaults();

    std::cout << "\nAll tests passed successfully\n";
    return 0;
}
