#define main original_main
#include "Main.cpp"
#undef main

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>

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
    assert(out.find("Версия: 3.0") != string::npos);
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

void test_Room_default() {
    Room r;
    for (int i = 0; i < 4; ++i) {
        assert(r.walls[i] == true);
    }
    assert(r.visited == false);
    cout << "test_Room_default passed\n";
}

void test_Hero_default_and_set() {
    Hero h;
    assert(h.getX() == 0);
    assert(h.getY() == 0);

    Hero h2(3, 7);
    assert(h2.getX() == 3);
    assert(h2.getY() == 7);

    h.setPosition(5, 9);
    assert(h.getX() == 5);
    assert(h.getY() == 9);
    cout << "test_Hero_default_and_set passed\n";
}

void test_Labyrinth_dimensions() {
    Labyrinth lab(8, 12);
    assert(lab.getWidth() == 8);
    assert(lab.getHeight() == 12);
    assert(lab.getExitX() == 7);
    assert(lab.getExitY() == 11);
    cout << "test_Labyrinth_dimensions passed\n";
}

void test_Labyrinth_isExit() {
    Labyrinth lab(5, 5);
    assert(lab.isExit(4, 4) == true);
    assert(lab.isExit(0, 0) == false);
    assert(lab.isExit(3, 3) == false);
    cout << "test_Labyrinth_isExit passed\n";
}

void test_Labyrinth_generation_connected() {
    const int w = 10, h = 10;
    Labyrinth lab(w, h);

    vector<vector<bool>> visited(h, vector<bool>(w, false));
    vector<pair<int, int>> stack;
    stack.push_back({0, 0});
    visited[0][0] = true;
    int count = 1;

    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {-1, 0, 1, 0};

    while (!stack.empty()) {
        auto [x, y] = stack.back();
        stack.pop_back();
        for (int d = 0; d < 4; ++d) {
            if (lab.canMove(x, y, d)) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                if (nx >= 0 && nx < w && ny >= 0 && ny < h && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    count++;
                    stack.push_back({nx, ny});
                }
            }
        }
    }

    assert(count == w * h);
    cout << "test_Labyrinth_generation_connected passed\n";
}

void test_Labyrinth_boundary_walls() {
    const int w = 8, h = 8;
    Labyrinth lab(w, h);

    for (int x = 0; x < w; ++x) {
        assert(lab.canMove(x, 0, 0) == false);         
        assert(lab.canMove(x, h - 1, 2) == false);    
    }
    for (int y = 0; y < h; ++y) {
        assert(lab.canMove(0, y, 3) == false);         
        assert(lab.canMove(w - 1, y, 1) == false);     
    }
    cout << "test_Labyrinth_boundary_walls passed\n";
}

void test_Labyrinth_wall_consistency() {
    const int w = 7, h = 7;
    Labyrinth lab(w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x + 1 < w) {
                assert(lab.canMove(x, y, 1) == lab.canMove(x + 1, y, 3));
            }
            if (y + 1 < h) {
                assert(lab.canMove(x, y, 2) == lab.canMove(x, y + 1, 0));
            }
        }
    }
    cout << "test_Labyrinth_wall_consistency passed\n";
}

void test_Labyrinth_print_contains_hero() {
    Labyrinth lab(5, 5);
    lab.markVisited(0, 0);

    redirectStart("");
    lab.print(0, 0);
    string out = redirectOutput();
    redirectStop();

    assert(out.find(" H ") != string::npos);
    assert(out.find("H - герой") != string::npos);
    cout << "test_Labyrinth_print_contains_hero passed\n";
}

void test_calculateInitialSteps() {
    GameSettings s;
    s.mazeWidth = 10; s.mazeHeight = 10;   // площадь 100

    s.difficulty = "Легкий";
    assert(calculateInitialSteps(s) == 150);   // 100 * 1.5
    s.difficulty = "Средний";
    assert(calculateInitialSteps(s) == 130);   // 100 * 1.3
    s.difficulty = "Сложный";
    assert(calculateInitialSteps(s) == 110);   // 100 * 1.1

    s.mazeWidth = 5; s.mazeHeight = 5;     // площадь 25
    s.difficulty = "Легкий";
    assert(calculateInitialSteps(s) == 37);    // 25 * 1.5 = 37.5 -> 37
    s.difficulty = "Средний";
    assert(calculateInitialSteps(s) == 32);    // 25 * 1.3 = 32.5 -> 32
    s.difficulty = "Сложный";
    assert(calculateInitialSteps(s) == 27);    // 25 * 1.1 = 27.5 -> 27

    s.mazeWidth = 10; s.mazeHeight = 10;
    s.difficulty = "Неизвестный";
    assert(calculateInitialSteps(s) == 100);

    cout << "test_calculateInitialSteps passed\n";
}

void test_startGame() {
    GameSettings s; 
    redirectStart("5\n\n");
    startGame(s);
    string out = redirectOutput();
    redirectStop();

    assert(out.find("Начать игру") != string::npos);
    assert(out.find("Генерация нового лабиринта") != string::npos);
    assert(out.find("Осталось шагов: 150") != string::npos);   // 10*10*1.5
    assert(out.find("Вы вышли из игры") != string::npos);
    cout << "test_startGame passed\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    test_calculateDifficulty();
    test_updateDifficulty();
    test_readInt_valid();
    test_readInt_invalid_then_valid();
    test_waitEnter();
    test_loadGame();
    test_about();
    test_changeMazeSize_valid();
    test_changeMazeSize_invalid_width();
    test_changeMazeSize_invalid_height();
    test_settingsMenu_show_and_back();
    test_settingsMenu_change_size();
    test_settingsMenu_invalid_choice();

    test_Room_default();
    test_Hero_default_and_set();
    test_Labyrinth_dimensions();
    test_Labyrinth_isExit();
    test_Labyrinth_generation_connected();
    test_Labyrinth_boundary_walls();
    test_Labyrinth_wall_consistency();
    test_Labyrinth_print_contains_hero();
    test_calculateInitialSteps();
    test_startGame();

    cout << "\nAll tests passed!\n";
    return 0;
}