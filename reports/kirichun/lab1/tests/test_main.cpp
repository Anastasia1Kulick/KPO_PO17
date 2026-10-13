#include <iostream>
#include <string>
#include <sstream>
#include <cassert>

struct SETTING {
    std::string specialty = "Программная инженерия";
    std::string studentName = "Аня";
    int subjectCount = 11;
};

void startGame();
void loadGame();
void settingGame(SETTING& set);
void infoGame();
void exitGame();

#define main original_main
#include "../src/main.cpp"
#undef main

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

void test_default_settings() {
    SETTING set;
    assert(set.specialty == "Программная инженерия");
    assert(set.studentName == "Аня");
    assert(set.subjectCount == 11);
    cout << "test_default_settings passed\n";
}

void test_startGame() {
    redirectStart("\n");
    startGame();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Игра началась!") != string::npos);
    cout << "test_startGame passed\n";
}

void test_loadGame() {
    redirectStart("\n");
    loadGame();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("не доступна") != string::npos);
    cout << "test_loadGame passed\n";
}

void test_infoGame() {
    redirectStart("\n");
    infoGame();
    string out = redirectOutput();
    redirectStop();
    assert(out.find("Киричун Анна") != string::npos);
    assert(out.find("1.0.0") != string::npos);
    cout << "test_infoGame passed\n";
}

void test_settingGame_change_name() {
    SETTING set;
    redirectStart("2\nАнна\n\n4\n");
    settingGame(set);
    redirectStop();
    assert(set.studentName == "Анна");
    cout << "test_settingGame_change_name passed\n";
}

void test_settingGame_change_specialty() {
    SETTING set;
    redirectStart("1\nИскусственный интеллект\n\n4\n");
    settingGame(set);
    redirectStop();
    assert(set.specialty == "Искусственный интеллект");
    cout << "test_settingGame_change_specialty passed\n";
}

void test_settingGame_change_subjects() {
    SETTING set;
    redirectStart("3\n8\n\n4\n");
    settingGame(set);
    redirectStop();
    assert(set.subjectCount == 8);
    cout << "test_settingGame_change_subjects passed\n";
}

int main() {
    test_default_settings();
    test_startGame();
    test_loadGame();
    test_infoGame();
    test_settingGame_change_name();
    test_settingGame_change_specialty();
    test_settingGame_change_subjects();

    cout << "\nAll tests passed!\n";
    return 0;
}
