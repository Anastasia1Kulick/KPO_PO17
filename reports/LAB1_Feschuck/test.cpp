#include <cstring>
#include <iostream>
#include "../src/game.h"
using namespace std;
struct TestReport {
    int passed;
    int failed;
    TestReport() : passed(0), failed(0) {}
};
void check(bool condition, const char* message, TestReport &report) {
    if (condition) {
        cout << "  [OK]   " << message << "\n";
        report.passed++;
    } else {
        cout << "  [FAIL] " << message << "\n";
        report.failed++;
    }
}

bool contains(const char* haystack, const char* needle) {
    return haystack != nullptr && needle != nullptr &&
           strstr(haystack, needle) != nullptr;
}

// ТЕСТ 1
void test_defaultSettings(TestReport &report) {
    cout << "\nТест 1: значения по умолчанию\n";

    GameSettings s;
    check(s.citySize   == 10,   "размер города по умолчанию = 10",   report);
    check(s.budget     == 1000, "бюджет по умолчанию = 1000",        report);
    check(s.population == 100,  "жителей по умолчанию = 100",        report);

    check(s.citySize == 999,    "ОШИБКА-ТЕСТ: 10 == 999 (ожидается FAIL)", report);
}

// ТЕСТ 2
void test_switchCitySize(TestReport &report) {
    cout << "\nТест 2: размер города 10 -> 20 -> 30 -> 10\n";

    int size = 10;
    switchCitySize(size);
    check(size == 20, "после 1-го переключения size == 20", report);
    switchCitySize(size);
    check(size == 30, "после 2-го переключения size == 30", report);
    switchCitySize(size);
    check(size == 10, "после 3-го переключения size == 10", report);

    switchCitySize(size);
    check(size == 20, "после 4-го переключения size == 20", report);
}

// ТЕСТ 3
void test_switchBudget(TestReport &report) {
    cout << "\nТест 3: бюджет 1000 -> 5000 -> 10000 -> 1000\n";

    int budget = 1000;
    switchBudget(budget);
    check(budget == 5000,  "после 1-го переключения budget == 5000",  report);
    switchBudget(budget);
    check(budget == 10000, "после 2-го переключения budget == 10000", report);
    switchBudget(budget);
    check(budget == 1000,  "после 3-го переключения budget == 1000",  report);
}

// ТЕСТ 4
void test_switchPopulation(TestReport &report) {
    cout << "\nТест 4: жители 100 -> 500 -> 1000 -> 100\n";

    int pop = 100;
    switchPopulation(pop);
    check(pop == 500,  "после 1-го переключения population == 500",  report);
    switchPopulation(pop);
    check(pop == 1000, "после 2-го переключения population == 1000", report);
    switchPopulation(pop);
    check(pop == 100,  "после 3-го переключения population == 100",  report);
}

// ТЕСТ 5
void test_playlistRecommendation(TestReport &report) {
    cout << "\nТест 5: рекомендация плейлиста\n";

    const char* rec = getPlaylistRecommendation();
    check(rec != nullptr,                "строка рекомендации не пустая",      report);
    check(contains(rec, "City Builder"), "содержит 'City Builder'",            report);
    check(contains(rec, "SimCity"),      "содержит 'SimCity'",                 report);
    check(contains(rec, "Lo-Fi"),        "содержит 'Lo-Fi'",                   report);
    check(contains(rec, "Minecraft"),    "содержит 'Minecraft'",               report);

    check(!contains(rec, "Rap"),         "не содержит 'Rap'",                  report);
    check(!contains(rec, "Metal"),       "не содержит 'Metal'",                report);
}

// ТЕСТ 6
void test_settingsAreSaved(TestReport &report) {
    cout << "\nТест 6: настройки сохраняются\n";

    GameSettings s;
    switchCitySize(s.citySize);
    switchBudget(s.budget);
    switchPopulation(s.population);

    check(s.citySize   == 20,   "размер города сохранился = 20",   report);
    check(s.budget     == 5000, "бюджет сохранился = 5000",        report);
    check(s.population == 500,  "жителей сохранилось = 500",       report);
}

// ТЕСТ 7
void test_fullCycle(TestReport &report) {
    cout << "\nТест 7: полный цикл переключений\n";

    int size = 10, budget = 1000, pop = 100;
    for (int i = 0; i < 3; ++i) {
        switchCitySize(size);
        switchBudget(budget);
        switchPopulation(pop);
    }
    check(size   == 10,   "после 3 циклов size == 10",   report);
    check(budget == 1000, "после 3 циклов budget == 1000", report);
    check(pop    == 100,  "после 3 циклов pop == 100",   report);
}

int main() {
    cout << "=====================================\n";
    cout << "  Запуск C++-юнит-тестов\n";
    cout << "=====================================\n";

    TestReport report;

    test_defaultSettings(report);
    test_switchCitySize(report);
    test_switchBudget(report);
    test_switchPopulation(report);
    test_playlistRecommendation(report);
    test_settingsAreSaved(report);
    test_fullCycle(report);

    cout << "\n=====================================\n";
    cout << "  ИТОГИ\n";
    cout << "=====================================\n";
    cout << "  Пройдено: " << report.passed << "\n";
    cout << "  Провалено: " << report.failed << "\n";
    cout << "=====================================\n";

    if (report.failed == 0) {
        cout << "  ВСЕ ТЕСТЫ ПРОЙДЕНЫ\n";
        return 0;
    } else {
        cout << "  ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ\n";
        return 1;
    }
}
