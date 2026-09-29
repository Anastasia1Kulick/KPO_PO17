#include <iostream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_DIFFICULTY = 1;
const int SET_TOTAL = 2;
const int SET_ALIVE = 3;
const int SET_AI_INFO = 4;
const int SET_BACK = 5;

const string PROGRAM_VERSION = "v1.0";
const string STUDENT_NAME = "Жук И.В.";

struct GameSettings {
    int difficulty = 1;
    int totalPlayers = 2;
    int alivePlayers = 2;

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

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "--- AICount_Formula ---\n";
    GameSettings s;
    s.totalPlayers = 2;  s.alivePlayers = 2;
    CHECK_EQ(s.getAICount(), 0, "2 всего, 2 живых -> 0 ИИ");
    s.totalPlayers = 4;  s.alivePlayers = 3;
    CHECK_EQ(s.getAICount(), 1, "4 всего, 3 живых -> 1 ИИ");
    s.totalPlayers = 6;  s.alivePlayers = 2;
    CHECK_EQ(s.getAICount(), 4, "6 всего, 2 живых -> 4 ИИ");
    s.totalPlayers = 8;  s.alivePlayers = 1;
    CHECK_EQ(s.getAICount(), 7, "8 всего, 1 живой -> 7 ИИ");
    s.totalPlayers = 5;  s.alivePlayers = 5;
    CHECK_EQ(s.getAICount(), 0, "5 всего, 5 живых -> 0 ИИ");

    cout << "\n--- AICount_NegativeGuard ---\n";
    GameSettings n;
    n.totalPlayers = 2;
    n.alivePlayers = 5;
    CHECK_EQ(n.getAICount(), 0, "живых больше всего -> 0");

    cout << "\n--- AICount_Boundaries ---\n";
    GameSettings b;
    b.totalPlayers = 2;   b.alivePlayers = 1;
    CHECK_EQ(b.getAICount(), 1, "минимум: 2 всего, 1 живой -> 1");
    b.totalPlayers = 100; b.alivePlayers = 100;
    CHECK_EQ(b.getAICount(), 0, "максимум: 100 всего, 100 живых -> 0");
    b.totalPlayers = 10;  b.alivePlayers = 1;
    CHECK_EQ(b.getAICount(), 9, "10 всего, 1 живой -> 9");

    cout << "\n--- MenuConstants ---\n";
    CHECK_EQ(MENU_START_GAME, 1, "MENU_START_GAME == 1");
    CHECK_EQ(MENU_LOAD_GAME, 2, "MENU_LOAD_GAME == 2");
    CHECK_EQ(MENU_SETTINGS, 3, "MENU_SETTINGS == 3");
    CHECK_EQ(MENU_ABOUT, 4, "MENU_ABOUT == 4");
    CHECK_EQ(MENU_EXIT, 5, "MENU_EXIT == 5");

    cout << "\n--- SettingsConstants ---\n";
    CHECK_EQ(SET_DIFFICULTY, 1, "SET_DIFFICULTY == 1");
    CHECK_EQ(SET_TOTAL, 2, "SET_TOTAL == 2");
    CHECK_EQ(SET_ALIVE, 3, "SET_ALIVE == 3");
    CHECK_EQ(SET_AI_INFO, 4, "SET_AI_INFO == 4");
    CHECK_EQ(SET_BACK, 5, "SET_BACK == 5");

    cout << "\n--- DefaultSettings ---\n";
    GameSettings d;
    CHECK_EQ(d.difficulty, 1, "сложность по умолчанию = 1");
    CHECK_EQ(d.totalPlayers, 2, "всего игроков по умолчанию = 2");
    CHECK_EQ(d.alivePlayers, 2, "живых по умолчанию = 2");
    CHECK_EQ(d.getAICount(), 0, "ИИ по умолчанию = 0");

    cout << "\n--- VersionAndStudent ---\n";
    CHECK_EQ(PROGRAM_VERSION, string("v1.0"), "версия = v1.0");
    CHECK_EQ(STUDENT_NAME, string("Жук И.В."), "студент = Жук И.В.");

    cout << "\n========================================\n";
    cout << "Пройдено:  " << g_passed << "\n";
    cout << "Провалено: " << g_failed << "\n";
    cout << "========================================\n";

    return (g_failed == 0) ? 0 : 1;
}