#define main lab2_disabled_main

#include "Lab2.cpp"

#undef main

#include <cassert>
#include <algorithm>
#include <clocale>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

static vector<int> toVector(queue<int> q)
{
    vector<int> v;
    while (!q.empty())
    {
        v.push_back(q.front());
        q.pop();
    }
    return v;
}

static void clearSettingsQueues()
{
    while (!citySizeValues.empty())   citySizeValues.pop();
    while (!budgetValues.empty())     budgetValues.pop();
    while (!populationValues.empty()) populationValues.pop();
}

class SettingsFileGuard
{
public:
    SettingsFileGuard()
    {
        ifstream in(SETTINGS_FILE);
        if (in.is_open())
        {
            hadFile_ = true;
            stringstream ss;
            ss << in.rdbuf();
            backup_ = ss.str();
        }
    }

    ~SettingsFileGuard()
    {
        if (hadFile_)
        {
            ofstream out(SETTINGS_FILE);
            out << backup_;
        }
        else
        {
            std::remove(SETTINGS_FILE.c_str());
        }
    }

private:
    bool hadFile_ = false;
    string backup_;
};

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

void test_next_value_simple_cycle()
{
    queue<int> q;
    q.push(1000); q.push(5000); q.push(10000);
    int cur = 1000;
    nextValue(q, cur); assert(cur == 5000);
    nextValue(q, cur); assert(cur == 10000);
    nextValue(q, cur); assert(cur == 1000);
    nextValue(q, cur); assert(cur == 5000);
    cout << "[OK] test_next_value_simple_cycle" << endl;
}

void test_next_value_single_element()
{
    queue<int> q; q.push(42);
    int cur = 42;
    nextValue(q, cur);
    assert(cur == 42);
    assert(q.size() == 1);
    cout << "[OK] test_next_value_single_element" << endl;
}

void test_next_value_empty_queue()
{
    queue<int> q;
    int cur = 777;
    nextValue(q, cur);
    assert(cur == 777);
    cout << "[OK] test_next_value_empty_queue" << endl;
}

void test_next_value_current_not_in_queue()
{
    queue<int> q;
    q.push(10); q.push(20); q.push(30);
    int cur = 999;
    nextValue(q, cur);
    assert(cur == 20);
    assert(q.size() == 3);
    cout << "[OK] test_next_value_current_not_in_queue" << endl;
}

void test_next_value_current_is_front()
{
    queue<int> q;
    q.push(10); q.push(20); q.push(30);
    int cur = 10;
    nextValue(q, cur);
    assert(cur == 20);
    assert(q.size() == 3);
    cout << "[OK] test_next_value_current_is_front" << endl;
}

void test_next_value_current_is_back()
{
    queue<int> q;
    q.push(10); q.push(20); q.push(30);
    int cur = 30;
    nextValue(q, cur);
    assert(cur == 10);
    assert(q.size() == 3);
    cout << "[OK] test_next_value_current_is_back" << endl;
}

void test_next_value_two_elements()
{
    queue<int> q;
    q.push(1); q.push(2);
    int cur = 1;
    nextValue(q, cur); assert(cur == 2);
    nextValue(q, cur); assert(cur == 1);
    assert(q.size() == 2);
    cout << "[OK] test_next_value_two_elements" << endl;
}

void test_next_value_negative_values()
{
    queue<int> q;
    q.push(-10); q.push(0); q.push(10);
    int cur = 0;
    nextValue(q, cur); assert(cur == 10);
    nextValue(q, cur); assert(cur == -10);
    cout << "[OK] test_next_value_negative_values" << endl;
}

void test_next_value_preserves_elements()
{
    queue<int> q;
    q.push(1); q.push(2); q.push(3); q.push(4);
    int cur = 2;
    nextValue(q, cur);
    vector<int> v = toVector(q);
    sort(v.begin(), v.end());
    assert((v == vector<int>{1, 2, 3, 4}));
    cout << "[OK] test_next_value_preserves_elements" << endl;
}

void test_next_value_full_cycle_restores_order()
{
    queue<int> q;
    q.push(1); q.push(2); q.push(3);
    int cur = 1;
    nextValue(q, cur);
    nextValue(q, cur);
    nextValue(q, cur);
    assert((toVector(q) == vector<int>{1, 2, 3}));
    assert(cur == 1);
    cout << "[OK] test_next_value_full_cycle_restores_order" << endl;
}

void test_next_value_budget_queue()
{
    queue<int> q;
    q.push(1000); q.push(5000); q.push(10000);
    int cur = 10000;
    nextValue(q, cur); assert(cur == 1000);
    nextValue(q, cur); assert(cur == 5000);
    cout << "[OK] test_next_value_budget_queue" << endl;
}

void test_next_value_population_queue()
{
    queue<int> q;
    q.push(100); q.push(500); q.push(1000);
    int cur = 500;
    nextValue(q, cur); assert(cur == 1000);
    nextValue(q, cur); assert(cur == 100);
    nextValue(q, cur); assert(cur == 500);
    cout << "[OK] test_next_value_population_queue" << endl;
}

void test_save_and_load_roundtrip()
{
    SettingsFileGuard guard;

    clearSettingsQueues();
    citySizeValues.push(1000); citySizeValues.push(5000); citySizeValues.push(10000);
    budgetValues.push(2000);   budgetValues.push(4000);
    populationValues.push(300); populationValues.push(600);
    populationValues.push(900); populationValues.push(1200);

    assert(saveSettingsToFile());

    clearSettingsQueues();
    assert(loadSettingsFromFile());

    assert((toVector(citySizeValues) == vector<int>{1000, 5000, 10000}));
    assert((toVector(budgetValues) == vector<int>{2000, 4000}));
    assert((toVector(populationValues) == vector<int>{300, 600, 900, 1200}));

    cout << "[OK] test_save_and_load_roundtrip" << endl;
}

void test_load_creates_defaults_when_missing()
{
    SettingsFileGuard guard;
    std::remove(SETTINGS_FILE.c_str());

    clearSettingsQueues();
    assert(loadSettingsFromFile());

    assert((toVector(citySizeValues) == vector<int>{1000, 5000, 10000}));
    assert((toVector(budgetValues) == vector<int>{1000, 5000, 10000}));
    assert((toVector(populationValues) == vector<int>{100, 500, 1000}));

    cout << "[OK] test_load_creates_defaults_when_missing" << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    cout << "===== гюосяй реярнб Lab2 =====" << endl;

    test_menu_enum();
    test_settings_menu_enum();

    test_next_value_simple_cycle();
    test_next_value_single_element();
    test_next_value_empty_queue();
    test_next_value_current_not_in_queue();
    test_next_value_current_is_front();
    test_next_value_current_is_back();
    test_next_value_two_elements();
    test_next_value_negative_values();
    test_next_value_preserves_elements();
    test_next_value_full_cycle_restores_order();
    test_next_value_budget_queue();
    test_next_value_population_queue();

    test_save_and_load_roundtrip();
    test_load_creates_defaults_when_missing();

    cout << "===== бяе реярш опнидемш =====" << endl;
    return 0;
}