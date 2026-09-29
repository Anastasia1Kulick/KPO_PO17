#include <cassert>
#include <cstring>
#include <iostream>
#include "../src/game.h"

static bool contains(const char* haystack, const char* needle) {
    return haystack != nullptr && needle != nullptr &&
           std::strstr(haystack, needle) != nullptr;
}

// Тест 1
static void test_defaultSettings() {
    GameSettings s;
    assert(s.citySize   == 10);
    assert(s.budget     == 1000);
    assert(s.population == 100);
    std::cout << "  [OK] Тест 1: значения по умолчанию\n";
}

// Тест 2
static void test_switchCitySize() {
    int size = 10;
    switchCitySize(size); assert(size == 20);
    switchCitySize(size); assert(size == 30);
    switchCitySize(size); assert(size == 10);
    std::cout << "  [OK] Тест 2: размер города 10->20->30->10\n";
}

// Тест 3
static void test_switchBudget() {
    int budget = 1000;
    switchBudget(budget); assert(budget == 5000);
    switchBudget(budget); assert(budget == 10000);
    switchBudget(budget); assert(budget == 1000);
    std::cout << "  [OK] Тест 3: бюджет 1000->5000->10000->1000\n";
}

// Тест 4
static void test_switchPopulation() {
    int pop = 100;
    switchPopulation(pop); assert(pop == 500);
    switchPopulation(pop); assert(pop == 1000);
    switchPopulation(pop); assert(pop == 100);
    std::cout << "  [OK] Тест 4: жители 100->500->1000->100\n";
}

// Тест 5
static void test_playlistRecommendation() {
    const char* rec = getPlaylistRecommendation();
    assert(rec != nullptr);
    assert(contains(rec, "City Builder"));
    assert(contains(rec, "SimCity"));
    assert(contains(rec, "Lo-Fi"));
    assert(contains(rec, "Minecraft"));
    std::cout << "  [OK] Тест 5: рекомендация плейлиста\n";
}

// Тест 6
static void test_settingsAreSaved() {
    GameSettings s;
    switchCitySize(s.citySize);
    switchBudget(s.budget);
    switchPopulation(s.population);
    assert(s.citySize   == 20);
    assert(s.budget     == 5000);
    assert(s.population == 500);
    std::cout << "  [OK] Тест 6: настройки сохраняются\n";
}

//  Тест 7
static void test_fullCycle() {
    int size = 10, budget = 1000, pop = 100;
    for (int i = 0; i < 3; ++i) {
        switchCitySize(size);
        switchBudget(budget);
        switchPopulation(pop);
    }
    assert(size == 10);
    assert(budget == 1000);
    assert(pop == 100);
    std::cout << "  [OK] Тест 7: полный цикл возвращает исходные значения\n";
}

int main() {
    std::cout << "=== Запуск C++-юнит-тестов ===\n";

    test_defaultSettings();
    test_switchCitySize();
    test_switchBudget();
    test_switchPopulation();
    test_playlistRecommendation();
    test_settingsAreSaved();
    test_fullCycle();

    std::cout << "=== Все C++-тесты пройдены ===\n";
    return 0;
}
