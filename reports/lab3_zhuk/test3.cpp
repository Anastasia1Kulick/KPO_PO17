#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <cstdio>
using namespace std;
const string DIFF_EASY = "Легкий";
const string DIFF_MEDIUM = "Средний";
const string DIFF_HARD = "Сложный";
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
    int strength;
    int endurance;
    int agility;
    int intellect;
    vector<Equipment> gear;
    Sportsman(string n, int baseStat)
        : name(n), strength(baseStat), endurance(baseStat), agility(baseStat), intellect(baseStat) {
    }
    int getTotalStrength() const {
        int total = strength;
        for (const auto& e : gear) total += e.bonusStrength;
        return total;
    }
    int getTotalEndurance() const {
        int total = endurance;
        for (const auto& e : gear) total += e.bonusEndurance;
        return total;
    }
    int getTotalAgility() const {
        int total = agility;
        for (const auto& e : gear) total += e.bonusAgility;
        return total;
    }
    int getTotalIntellect() const {
        int total = intellect;
        for (const auto& e : gear) total += e.bonusIntellect;
        return total;
    }
};
static int g_passed = 0;
static int g_failed = 0;
#define CHECK_EQ(a, e, msg) \
    do { \
        auto _a = (a); auto _e = (e); \
        if (_a == _e) { ++g_passed; cout << "  [PASS] " << msg << "\n"; } \
        else { \
            ++g_failed; \
            cout << "  [FAIL] " << msg << " (ожидалось: " << _e << ", получено: " << _a << ")\n"; \
        } \
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
}
void testSportsmanBase() {
    cout << "\n--- testSportsmanBase ---\n";
    Sportsman s("Test", 5);
    CHECK_EQ(s.getTotalStrength(), 5, "Базовая сила = 5");
    CHECK_EQ(s.getTotalEndurance(), 5, "Базовая выносливость = 5");
    CHECK_EQ(s.getTotalAgility(), 5, "Базовая ловкость = 5");
    CHECK_EQ(s.getTotalIntellect(), 5, "Базовый интеллект = 5");
}
void testSportsmanWithGear() {
    cout << "\n--- testSportsmanWithGear ---\n";
    Sportsman s("Test", 5);
    s.gear.push_back(Equipment("Штанга", 150, 4, 0, 0, 0));
    s.gear.push_back(Equipment("Кроссовки", 100, 0, 0, 3, 0));
    CHECK_EQ(s.getTotalStrength(), 9, "Сила с экипировкой = 9");
    CHECK_EQ(s.getTotalEndurance(), 5, "Выносливость без бонусов = 5");
    CHECK_EQ(s.getTotalAgility(), 8, "Ловкость с экипировкой = 8");
}
void testTrainTeamLogic() {
    cout << "\n--- testTrainTeamLogic ---\n";
    Sportsman s("Test", 5);
    s.agility += 2; s.endurance += 2;
    CHECK_EQ(s.getTotalAgility(), 7, "Пробежка: Ловкость 5+2=7");
    CHECK_EQ(s.getTotalEndurance(), 7, "Пробежка: Выносливость 5+2=7");
    s.strength += 3;
    CHECK_EQ(s.getTotalStrength(), 8, "Силовая: Сила 5+3=8");
    s.intellect += 2;
    CHECK_EQ(s.getTotalIntellect(), 7, "Техника: Интеллект 5+2=7");
}
void testCustomTrainingLogic() {
    cout << "\n--- testCustomTrainingLogic ---\n";
    Sportsman s("Test", 5);
    int stat = 1; int amount = 10;
    if (stat == 1) s.strength += amount;
    CHECK_EQ(s.getTotalStrength(), 15, "Кастомная: Сила 5+10=15");
    stat = 3; amount = 7;
    if (stat == 3) s.agility += amount;
    CHECK_EQ(s.getTotalAgility(), 12, "Кастомная: Ловкость 5+7=12");
}
void testCompetitionScore() {
    cout << "\n--- testCompetitionScore ---\n";
    Sportsman s("Test", 5);
    int score = s.getTotalStrength() + s.getTotalEndurance() + s.getTotalAgility() + s.getTotalIntellect();
    CHECK_EQ(score, 20, "Счёт спортсмена 5+5+5+5 = 20");
    CHECK_EQ(score * 3, 60, "Счёт на легком (x3) = 60");
    CHECK_EQ(score * 2, 40, "Счёт на среднем (x2) = 40");
    CHECK_EQ(score * 1, 20, "Счёт на сложном (x1) = 20");
}
int main() {
    setlocale(LC_ALL, "Russian");
    testAICount();
    testSportsmanBase();
    testSportsmanWithGear();
    testTrainTeamLogic();
    testCustomTrainingLogic();
    testCompetitionScore();
    cout << "\nПройдено:  " << g_passed << "\n";
    cout << "Провалено: " << g_failed << "\n";
    return (g_failed == 0) ? 0 : 1;
}