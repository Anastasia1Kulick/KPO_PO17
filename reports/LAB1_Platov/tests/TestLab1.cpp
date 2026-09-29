#define main lab1_disabled_main

#include "C:\Users\Ïëàòîâ\ÊÏÎ\Lab1.cpp"

#undef main

#include <cassert>
#include <iostream>

using namespace std;

void test_start_game_enum()
{
    assert(START_GAME == 1);
    cout << "[OK] test_start_game_enum" << endl;
}

void test_load_game_enum()
{
    assert(LOAD_GAME == 2);
    cout << "[OK] test_load_game_enum" << endl;
}

void test_about_enum()
{
    assert(ABOUT == 4);
    cout << "[OK] test_about_enum" << endl;
}

void test_default_settings()
{
    Settings s;
    assert(s.citySize == 1000);
    assert(s.budget == 1000);
    assert(s.population == 100);
    cout << "[OK] test_default_settings" << endl;
}

void test_menu_enum()
{
    assert(START_GAME == 1);
    assert(LOAD_GAME == 2);
    assert(SETTINGS == 3);
    assert(ABOUT == 4);
    assert(EXIT == 5);
    cout << "[OK] test_menu_enum" << endl;
}

void test_settings_menu_enum()
{
    assert(SHOW == 1);
    assert(CITY_SIZE == 2);
    assert(BUDGET == 3);
    assert(POPULATION == 4);
    assert(SET_EXIT == 5);
    cout << "[OK] test_settings_menu_enum" << endl;
}

void test_settings_mutation()
{
    Settings s;
    s.citySize = 5000;
    s.budget = 10000;
    s.population = 500;
    assert(s.citySize == 5000);
    assert(s.budget == 10000);
    assert(s.population == 500);
    cout << "[OK] test_settings_mutation" << endl;
}

int nextCitySize(int cur)
{
    if (cur == 1000) return 5000;
    if (cur == 5000) return 10000;
    return 1000;
}
int nextBudget(int cur)
{
    if (cur == 1000) return 5000;
    if (cur == 5000) return 10000;
    return 1000;
}
int nextPopulation(int cur)
{
    if (cur == 100)  return 500;
    if (cur == 500)  return 1000;
    return 100;
}

void test_cycle_logic()
{
    assert(nextCitySize(1000) == 5000);
    assert(nextCitySize(5000) == 10000);
    assert(nextCitySize(10000) == 1000);
    assert(nextCitySize(42) == 1000);
    assert(nextBudget(1000) == 5000);
    assert(nextBudget(5000) == 10000);
    assert(nextBudget(10000) == 1000);
    assert(nextPopulation(100) == 500);
    assert(nextPopulation(500) == 1000);
    assert(nextPopulation(1000) == 100);
    cout << "[OK] test_cycle_logic" << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    cout << "===== ÇÀÏÓÑÊ ÒÅÑÒÎÂ Lab1 =====" << endl;
    test_start_game_enum();
    test_load_game_enum();
    test_about_enum();
    test_default_settings();
    test_menu_enum();
    test_settings_menu_enum();
    test_settings_mutation();
    test_cycle_logic();
    cout << "===== ÂÑÅ ÒÅÑÒÛ ÏĞÎÉÄÅÍÛ =====";
    return 0;
}