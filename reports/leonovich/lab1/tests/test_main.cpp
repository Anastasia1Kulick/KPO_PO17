#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cstdlib>

using namespace std;

const string DEFAULT_DIFFICULTY   = "Легкий";
const int    DEFAULT_ALL_PLAYERS  = 4;
const int    DEFAULT_HUMAN_PLAYERS = 2;

const int MIN_PLAYERS = 2;
const int MAX_PLAYERS = 5;
const int MIN_HUMAN   = 1;

struct Settings {
    string difficulty   = DEFAULT_DIFFICULTY;
    int    allPlayers   = DEFAULT_ALL_PLAYERS;
    int    humanPlayers = DEFAULT_HUMAN_PLAYERS;

    int aiPlayers() const { return allPlayers - humanPlayers; }

    void reset() {
        difficulty   = DEFAULT_DIFFICULTY;
        allPlayers   = DEFAULT_ALL_PLAYERS;
        humanPlayers = DEFAULT_HUMAN_PLAYERS;
    }

    bool setDifficulty(int diffChoice) {
        if (diffChoice == 1) { difficulty = "Легкий";  return true; }
        if (diffChoice == 2) { difficulty = "Средний"; return true; }
        if (diffChoice == 3) { difficulty = "Сложный"; return true; }
        return false;
    }

    bool setAllPlayers(int n) {
        if (n < MIN_PLAYERS || n > MAX_PLAYERS) return false;
        allPlayers = n;
        if (humanPlayers > allPlayers) humanPlayers = allPlayers;
        return true;
    }

    bool setHumanPlayers(int n) {
        if (n < MIN_HUMAN || n > allPlayers) return false;
        humanPlayers = n;
        return true;
    }
};

static int g_passed = 0;
static int g_failed = 0;
static vector<string> g_failures;

void check(bool cond, const string& name) {
    if (cond) {
        ++g_passed;
        cout << "  [ OK ] " << name << "\n";
    } else {
        ++g_failed;
        g_failures.push_back(name);
        cout << "  [FAIL] " << name << "\n";
    }
}

template <typename T>
void checkEq(const T& actual, const T& expected, const string& name) {
    if (actual == expected) {
        ++g_passed;
        cout << "  [ OK ] " << name << "\n";
    } else {
        ++g_failed;
        g_failures.push_back(name);
        cout << "  [FAIL] " << name
             << "  (получено: " << actual
             << ", ожидалось: " << expected << ")\n";
    }
}

void testDefaults() {
    cout << "\n[1] Значения по умолчанию\n";
    Settings s;
    checkEq(s.difficulty,   string("Легкий"), "сложность по умолчанию = Легкий");
    checkEq(s.allPlayers,   4,                "всего игроков по умолчанию = 4");
    checkEq(s.humanPlayers, 2,                "живых игроков по умолчанию = 2");
    checkEq(s.aiPlayers(),  2,                "ИИ-игроков по умолчанию = 2");
}

void testDifficulty() {
    cout << "\n[2] Установка сложности\n";
    Settings s;
    check(s.setDifficulty(2),                "выбор 2 (Средний) — успех");
    checkEq(s.difficulty, string("Средний"), "сложность = Средний");
    check(s.setDifficulty(3),                "выбор 3 (Сложный) — успех");
    checkEq(s.difficulty, string("Сложный"), "сложность = Сложный");
    check(!s.setDifficulty(0),               "выбор 0 — ошибка");
    check(!s.setDifficulty(4),               "выбор 4 — ошибка");
    checkEq(s.difficulty, string("Сложный"), "сложность не изменилась после ошибки");
}

void testAllPlayersBoundaries() {
    cout << "\n[3] Границы общего числа игроков (2..5)\n";
    Settings s;
    check(!s.setAllPlayers(1), "1 — отклонено (ниже минимума)");
    check(!s.setAllPlayers(6), "6 — отклонено (выше максимума)");
    check(!s.setAllPlayers(0), "0 — отклонено");
    check(!s.setAllPlayers(-3), "-3 — отклонено");

    check(s.setAllPlayers(2), "2 — принято (нижняя граница)");
    checkEq(s.allPlayers, 2, "всего игроков = 2");
    check(s.setAllPlayers(5), "5 — принято (верхняя граница)");
    checkEq(s.allPlayers, 5, "всего игроков = 5");
}

void testHumanPlayersBoundaries() {
    cout << "\n[4] Границы живых игроков (1..allPlayers)\n";
    Settings s;
    s.allPlayers = 4;

    check(!s.setHumanPlayers(0), "0 — отклонено");
    check(!s.setHumanPlayers(5), "5 при всего=4 — отклонено");
    check(s.setHumanPlayers(1),  "1 — принято");
    checkEq(s.humanPlayers, 1, "живых игроков = 1");
    check(s.setHumanPlayers(4),  "4 при всего=4 — принято (граница)");
    checkEq(s.humanPlayers, 4, "живых игроков = 4");
    checkEq(s.aiPlayers(), 0, "ИИ-игроков = 0 (все живые)");
}

void testHumanClampOnAllPlayersReduction() {
    cout << "\n[5] Автокоррекция humanPlayers при уменьшении allPlayers\n";
    Settings s;
    s.humanPlayers = 4;
    s.allPlayers   = 4;

    check(s.setAllPlayers(2), "allPlayers = 2 — принято");
    checkEq(s.allPlayers,   2, "всего игроков = 2");
    checkEq(s.humanPlayers, 2, "живых игроков автоматически = 2");

    s.humanPlayers = 4;
    s.allPlayers   = 4;
    s.setAllPlayers(3);
    checkEq(s.humanPlayers, 3, "clamp: humanPlayers = 3 при allPlayers = 3");
}

void testAiPlayersComputation() {
    cout << "\n[6] Вычисление числа ИИ-игроков\n";
    Settings s;
    s.allPlayers = 5; s.humanPlayers = 2;
    checkEq(s.aiPlayers(), 3, "5 - 2 = 3 ИИ");
    s.allPlayers = 5; s.humanPlayers = 5;
    checkEq(s.aiPlayers(), 0, "5 - 5 = 0 ИИ");
    s.allPlayers = 2; s.humanPlayers = 1;
    checkEq(s.aiPlayers(), 1, "2 - 1 = 1 ИИ");
}

void testReset() {
    cout << "\n[7] Сброс настроек\n";
    Settings s;
    s.difficulty   = "Сложный";
    s.allPlayers   = 5;
    s.humanPlayers = 5;

    s.reset();

    checkEq(s.difficulty,   string("Легкий"), "сложность после сброса = Легкий");
    checkEq(s.allPlayers,   4,                "всего игроков после сброса = 4");
    checkEq(s.humanPlayers, 2,                "живых после сброса = 2");
    checkEq(s.aiPlayers(),  2,                "ИИ после сброса = 2");
}

void testInvariantAiNonNegative() {
    cout << "\n[8] Инвариант: ИИ-игроков >= 0 (перебор всех валидных комбинаций)\n";
    bool allOk = true;
    for (int total = MIN_PLAYERS; total <= MAX_PLAYERS; ++total) {
        for (int human = MIN_HUMAN; human <= total; ++human) {
            Settings s;
            s.allPlayers   = total;
            s.humanPlayers = human;
            if (s.aiPlayers() < 0) {
                allOk = false;
                cout << "    нарушение: total=" << total
                     << " human=" << human
                     << " ai=" << s.aiPlayers() << "\n";
            }
        }
    }
    check(allOk, "для всех (total in [2..5], human in [1..total]) ai >= 0");
}

int main() {
    cout << " Запуск тестов: Соревнования армий\n";

    testDefaults();
    testDifficulty();
    testAllPlayersBoundaries();
    testHumanPlayersBoundaries();
    testHumanClampOnAllPlayersReduction();
    testAiPlayersComputation();
    testReset();
    testInvariantAiNonNegative();

    cout << " Итог: пройдено " << g_passed
         << ", провалено " << g_failed << "\n";

    if (g_failed > 0) {
        cout << "\nПроваленные тесты:\n";
        for (const auto& f : g_failures) cout << "  - " << f << "\n";
        return 1;
    }
    cout << "\nВсе тесты пройдены успешно.\n";
    return 0;
}
