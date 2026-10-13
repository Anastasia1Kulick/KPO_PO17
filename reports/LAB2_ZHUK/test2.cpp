#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <cstdio>

using namespace std;

const string DIFF_EASY = "Легкий";
const string DIFF_MEDIUM = "Средний";
const string DIFF_HARD = "Сложный";

struct GameSettings {
    string difficulty = DIFF_EASY;
    int    totalPlayers = 2;
    int    alivePlayers = 2;

    int getAICount() const {
        int ai = totalPlayers - alivePlayers;
        return (ai < 0) ? 0 : ai;
    }
};

static int g_passed = 0;
static int g_failed = 0;

#define CHECK_EQ(a, e, msg)                                              \
    do {                                                                 \
        auto _a = (a); auto _e = (e);                                    \
        if (_a == _e) { ++g_passed; cout << "  [PASS] " << msg << "\n"; }\
        else {                                                           \
            ++g_failed;                                                  \
            cout << "  [FAIL] " << msg                                 \
                 << " (ожидалось: " << _e << ", получено: " << _a << ")\n"; \
        }                                                                \
    } while (0)

#define CHECK(cond, msg)                                                 \
    do {                                                                 \
        if (cond) { ++g_passed; cout << "  [PASS] " << msg << "\n"; }    \
        else      { ++g_failed; cout << "  [FAIL] " << msg << "\n"; }    \
    } while (0)

void testAICount() {
    cout << "--- testAICount ---\n";
    GameSettings s;
    s.totalPlayers = 2; s.alivePlayers = 2;
    CHECK_EQ(s.getAICount(), 0, "2/2 -> 0");
    s.totalPlayers = 6; s.alivePlayers = 2;
    CHECK_EQ(s.getAICount(), 4, "6/2 -> 4");
    s.totalPlayers = 8; s.alivePlayers = 1;
    CHECK_EQ(s.getAICount(), 7, "8/1 -> 7");
    s.totalPlayers = 2; s.alivePlayers = 5;
    CHECK_EQ(s.getAICount(), 0, "защита от отрицательного");
}

void testDifficultyString() {
    cout << "\n--- testDifficultyString ---\n";
    GameSettings s;
    CHECK_EQ(s.difficulty, string("Легкий"), "по умолчанию Легкий");
    s.difficulty = DIFF_MEDIUM;
    CHECK_EQ(s.difficulty, string("Средний"), "Средний");
    s.difficulty = DIFF_HARD;
    CHECK_EQ(s.difficulty, string("Сложный"), "Сложный");
}

void testSaveLoad() {
    cout << "\n--- testSaveLoad ---\n";
    const string FN = "test_settings.txt";

    {
        ofstream f(FN);
        f << "difficulty=Сложный\n";
        f << "totalPlayers=7\n";
        f << "alivePlayers=3\n";
    }

    GameSettings s2;
    {
        ifstream f(FN);
        string line;
        while (getline(f, line)) {
            auto pos = line.find('=');
            if (pos == string::npos) continue;
            string key = line.substr(0, pos);
            string val = line.substr(pos + 1);
            if (key == "difficulty")   s2.difficulty = val;
            if (key == "totalPlayers") s2.totalPlayers = stoi(val);
            if (key == "alivePlayers") s2.alivePlayers = stoi(val);
        }
    }

    CHECK_EQ(s2.difficulty, string("Сложный"), "difficulty из файла");
    CHECK_EQ(s2.totalPlayers, 7, "totalPlayers из файла");
    CHECK_EQ(s2.alivePlayers, 3, "alivePlayers из файла");

    remove(FN.c_str());
}

void testAINamesSet() {
    cout << "\n--- testAINamesSet ---\n";
    set<string> names = { "Иван","Кирилл","Даниил","Анатолий",
                         "Андрей","Александр","Виолетта","Глеб" };
    CHECK_EQ(names.size(), 8u, "8 ИИ-имён");
    CHECK(names.count("Иван") == 1, "есть Иван");
    CHECK(names.count("Кирилл") == 1, "есть Кирилл");
    CHECK(names.count("Даниил") == 1, "есть Даниил");
    CHECK(names.count("Анатолий") == 1, "есть Анатолий");
    CHECK(names.count("Андрей") == 1, "есть Андрей");
    CHECK(names.count("Александр") == 1, "есть Александр");
    CHECK(names.count("Виолетта") == 1, "есть Виолетта");
    CHECK(names.count("Глеб") == 1, "есть Глеб");
}

int main() {
    setlocale(LC_ALL, "Russian");

    testAICount();
    testDifficultyString();
    testSaveLoad();
    testAINamesSet();

    cout << "\nПройдено:  " << g_passed << "\n";
    cout << "Провалено: " << g_failed << "\n";

    return (g_failed == 0) ? 0 : 1;
}