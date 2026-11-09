#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <set>
#include <vector>
#include <sstream>
#include <ctime>
#include <cstdio>

using namespace std;

const string DIFF_EASY = "Легкий";
const string SAVEGAME_FILE = "test_savegame.txt";
const int SAVE_COOLDOWN_SECONDS = 1800;

struct GameSettings {
    string difficulty = DIFF_EASY;
    int totalPlayers = 2;
    int alivePlayers = 2;
    int getAICount() const {
        int ai = totalPlayers - alivePlayers;
        return (ai < 0) ? 0 : ai;
    }
};

class Equipment {
public:
    string name;
    int price;
    int bonusStrength;
    int bonusEndurance;
    int bonusAgility;
    int bonusIntellect;
    Equipment(string n, int p, int s, int e, int a, int i)
        : name(n), price(p), bonusStrength(s), bonusEndurance(e), bonusAgility(a), bonusIntellect(i) {
    }
};

class Sportsman {
public:
    string name;
    int strength, endurance, agility, intellect;
    vector<Equipment> gear;
    Sportsman(string n, int baseStat)
        : name(n), strength(baseStat), endurance(baseStat), agility(baseStat), intellect(baseStat) {
    }
};

class Captain {
public:
    string name;
    int money;
    vector<Sportsman> team;
    vector<Equipment> shop;
    time_t lastSaveTime = 0;
    Captain(string n, int startMoney) : name(n), money(startMoney) {}

    bool saveGame(const GameSettings& settings) {
        time_t now = time(nullptr);
        if (lastSaveTime != 0 && difftime(now, lastSaveTime) < SAVE_COOLDOWN_SECONDS) {
            return false;
        }
        ofstream f(SAVEGAME_FILE);
        if (!f.is_open()) return false;
        f << settings.difficulty << ";" << settings.totalPlayers << ";" << settings.alivePlayers << ";"
            << name << ";" << money << ";"
            << team.size() << ";" << shop.size() << ";"
            << now << "\n";
        for (const auto& s : team) {
            f << s.name << ";" << s.strength << ";" << s.endurance << ";" << s.agility << ";" << s.intellect << ";"
                << s.gear.size() << ";";
            for (const auto& e : s.gear) {
                f << e.name << ";" << e.price << ";" << e.bonusStrength << ";" << e.bonusEndurance << ";"
                    << e.bonusAgility << ";" << e.bonusIntellect << ";";
            }
            f << "\n";
        }
        for (const auto& e : shop) {
            f << e.name << ";" << e.price << ";" << e.bonusStrength << ";" << e.bonusEndurance << ";"
                << e.bonusAgility << ";" << e.bonusIntellect << ";\n";
        }
        lastSaveTime = now;
        return true;
    }

    bool loadGame(GameSettings& settings) {
        ifstream f(SAVEGAME_FILE);
        if (!f.is_open()) return false;
        string line;
        getline(f, line);
        stringstream ss(line);
        string item;
        getline(ss, settings.difficulty, ';');
        getline(ss, item, ';'); settings.totalPlayers = stoi(item);
        getline(ss, item, ';'); settings.alivePlayers = stoi(item);
        getline(ss, name, ';');
        getline(ss, item, ';'); money = stoi(item);
        getline(ss, item, ';'); int teamSize = stoi(item);
        getline(ss, item, ';'); int shopSize = stoi(item);
        getline(ss, item, ';'); lastSaveTime = stoi(item);
        team.clear(); shop.clear();
        for (int i = 0; i < teamSize; ++i) {
            getline(f, line);
            stringstream tss(line);
            string sName; int str, end, agi, intel, gearCount;
            getline(tss, sName, ';');
            getline(tss, item, ';'); str = stoi(item);
            getline(tss, item, ';'); end = stoi(item);
            getline(tss, item, ';'); agi = stoi(item);
            getline(tss, item, ';'); intel = stoi(item);
            getline(tss, item, ';'); gearCount = stoi(item);
            Sportsman s(sName, str);
            s.endurance = end; s.agility = agi; s.intellect = intel;
            for (int j = 0; j < gearCount; ++j) {
                string eName; int p, bs, be, ba, bi;
                getline(tss, eName, ';');
                getline(tss, item, ';'); p = stoi(item);
                getline(tss, item, ';'); bs = stoi(item);
                getline(tss, item, ';'); be = stoi(item);
                getline(tss, item, ';'); ba = stoi(item);
                getline(tss, item, ';'); bi = stoi(item);
                s.gear.push_back(Equipment(eName, p, bs, be, ba, bi));
            }
            team.push_back(s);
        }
        for (int i = 0; i < shopSize; ++i) {
            getline(f, line);
            stringstream sss(line);
            string eName; int p, bs, be, ba, bi;
            getline(sss, eName, ';');
            getline(sss, item, ';'); p = stoi(item);
            getline(sss, item, ';'); bs = stoi(item);
            getline(sss, item, ';'); be = stoi(item);
            getline(sss, item, ';'); ba = stoi(item);
            getline(sss, item, ';'); bi = stoi(item);
            shop.push_back(Equipment(eName, p, bs, be, ba, bi));
        }
        return true;
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

void testSaveLoad() {
    cout << "--- testSaveLoad ---\n";
    GameSettings s;
    s.difficulty = "Сложный";
    s.totalPlayers = 5;
    s.alivePlayers = 3;

    Captain c("Иван", 450);
    Sportsman sp1("Бегун", 5);
    sp1.strength = 10; sp1.agility = 8;
    sp1.gear.push_back(Equipment("Кроссовки", 100, 1, 2, 3, 0));
    c.team.push_back(sp1);
    c.shop.push_back(Equipment("Штанга", 150, 4, 1, 0, 0));

    CHECK(c.saveGame(s), "Сохранение прошло успешно");

    GameSettings s2;
    Captain c2("Temp", 0);
    CHECK(c2.loadGame(s2), "Загрузка прошла успешно");

    CHECK_EQ(s2.difficulty, string("Сложный"), "Сложность восстановлена");
    CHECK_EQ(s2.totalPlayers, 5, "Всего игроков восстановлено");
    CHECK_EQ(s2.alivePlayers, 3, "Живых восстановлено");
    CHECK_EQ(c2.name, string("Иван"), "Имя капитана восстановлено");
    CHECK_EQ(c2.money, 450, "Бюджет восстановлен");
    CHECK_EQ(c2.team.size(), 1u, "Размер команды восстановлен");
    CHECK_EQ(c2.shop.size(), 1u, "Размер магазина восстановлен");

    if (c2.team.size() > 0) {
        CHECK_EQ(c2.team[0].name, string("Бегун"), "Имя спортсмена");
        CHECK_EQ(c2.team[0].strength, 10, "Сила спортсмена");
        CHECK_EQ(c2.team[0].gear.size(), 1u, "Количество экипировки");
        CHECK_EQ(c2.team[0].gear[0].name, string("Кроссовки"), "Имя экипировки");
    }
    remove(SAVEGAME_FILE.c_str());
}

void testCooldown() {
    cout << "\n--- testCooldown ---\n";
    GameSettings s;
    Captain c("Тест", 100);
    CHECK(c.saveGame(s), "Первое сохранение работает");
    CHECK(!c.saveGame(s), "Второе сохранение подряд заблокировано кулдауном");
    c.lastSaveTime -= 1801;
    CHECK(c.saveGame(s), "Спустя 30 минут сохранение снова работает");
    remove(SAVEGAME_FILE.c_str());
}

void testAICount() {
    cout << "\n--- testAICount ---\n";
    GameSettings s;
    s.totalPlayers = 8; s.alivePlayers = 3;
    CHECK_EQ(s.getAICount(), 5, "8 всего, 3 живых -> 5 ИИ");
    s.totalPlayers = 2; s.alivePlayers = 5;
    CHECK_EQ(s.getAICount(), 0, "защита от отрицательного");
}

int main() {
    setlocale(LC_ALL, "Russian");
    testSaveLoad();
    testCooldown();
    testAICount();
    cout << "\nПройдено:  " << g_passed << "\n";
    cout << "Провалено: " << g_failed << "\n";
    return (g_failed == 0) ? 0 : 1;
}