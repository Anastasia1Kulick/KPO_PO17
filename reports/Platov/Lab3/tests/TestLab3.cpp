#define main lab3_disabled_main

#include "../src/Lab3.cpp"

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

void test_building_default_ctor()
{
    Building b;
    assert(b.getName() == "");
    assert(b.getCost() == 0);
    assert(b.getWeeklyIncome() == 0);
    assert(b.getPopulationBonus() == 0);
    assert(b.getMaxCount() == 0);
    assert(b.getArea() == 0);
    cout << "[OK] test_building_default_ctor" << endl;
}

void test_building_param_ctor()
{
    Building b("House", 1000, 80, 20, 5, 10);
    assert(b.getName() == "House");
    assert(b.getCost() == 1000);
    assert(b.getWeeklyIncome() == 80);
    assert(b.getPopulationBonus() == 20);
    assert(b.getMaxCount() == 5);
    assert(b.getArea() == 10);
    cout << "[OK] test_building_param_ctor" << endl;
}

void test_buildings_catalog_size()
{
    vector<Building> list = getAvailableBuildings();
    assert(list.size() == 10);
    cout << "[OK] test_buildings_catalog_size" << endl;
}

void test_buildings_catalog_positive_fields()
{
    vector<Building> list = getAvailableBuildings();
    for (size_t i = 0; i < list.size(); ++i)
    {
        const Building& b = list[i];
        assert(!b.getName().empty());
        assert(b.getCost() > 0);
        assert(b.getArea() > 0);
        assert(b.getMaxCount() > 0);
        assert(b.getWeeklyIncome() >= 0);
    }
    cout << "[OK] test_buildings_catalog_positive_fields" << endl;
}

void test_buildings_catalog_unique_names()
{
    vector<Building> list = getAvailableBuildings();
    for (size_t i = 0; i < list.size(); ++i)
        for (size_t j = i + 1; j < list.size(); ++j)
            assert(list[i].getName() != list[j].getName());
    cout << "[OK] test_buildings_catalog_unique_names" << endl;
}

void test_buildings_catalog_contains_required()
{
    vector<Building> list = getAvailableBuildings();
    vector<string> required = {
        "Частный дом", "Квартира", "Общежитие", "Школа", "Больница",
        "Магазин", "Парк", "Бизнес-центр", "Банк", "Завод"
    };
    for (size_t i = 0; i < required.size(); ++i)
    {
        bool found = false;
        for (size_t j = 0; j < list.size(); ++j)
            if (list[j].getName() == required[i]) { found = true; break; }
        assert(found);
    }
    cout << "[OK] test_buildings_catalog_contains_required" << endl;
}

void test_town_initial_state()
{
    Town t(500, 3000, 200);
    assert(t.getCityArea() == 500);
    assert(t.getUsedArea() == 0);
    assert(t.getFreeArea() == 500);
    assert(t.getBudget() == 3000);
    assert(t.getPopulation() == 200);
    assert(t.getBuildingsCount() == 0);
    assert(t.getIncomeBonusPercent() == 0);
    cout << "[OK] test_town_initial_state" << endl;
}

void test_town_build_success()
{
    Town t(100, 5000, 100);
    Building b("House", 1000, 80, 20, 5, 10);
    assert(t.canBuild(b));
    assert(t.buildBuilding(b));
    assert(t.getBudget() == 4000);
    assert(t.getUsedArea() == 10);
    assert(t.getFreeArea() == 90);
    assert(t.getPopulation() == 120);
    assert(t.getBuildingsCount() == 1);
    cout << "[OK] test_town_build_success" << endl;
}

void test_town_build_no_money()
{
    Town t(100, 500, 100);
    Building b("House", 1000, 80, 20, 5, 10);
    assert(!t.canBuild(b));
    assert(!t.buildBuilding(b));
    assert(t.getBudget() == 500);
    assert(t.getUsedArea() == 0);
    assert(t.getBuildingsCount() == 0);
    cout << "[OK] test_town_build_no_money" << endl;
}

void test_town_build_no_space()
{
    Town t(5, 5000, 100);
    Building b("Factory", 4000, 450, 10, 2, 60);
    assert(!t.canBuild(b));
    assert(!t.buildBuilding(b));
    assert(t.getBudget() == 5000);
    cout << "[OK] test_town_build_no_space" << endl;
}

void test_town_build_max_count()
{
    Town t(1000, 100000, 0);
    Building b("Hospital", 3500, 180, 20, 2, 30);
    assert(t.buildBuilding(b));
    assert(t.buildBuilding(b));
    assert(!t.canBuild(b));
    assert(!t.buildBuilding(b));
    assert(t.getBuildingsCount() == 2);
    cout << "[OK] test_town_build_max_count" << endl;
}

void test_town_build_exactly_full_area()
{
    Town t(20, 5000, 0);
    Building b("House", 1000, 80, 20, 5, 10);
    assert(t.buildBuilding(b));
    assert(t.buildBuilding(b));
    assert(t.getUsedArea() == 20);
    assert(t.getFreeArea() == 0);
    assert(!t.canBuild(b));
    cout << "[OK] test_town_build_exactly_full_area" << endl;
}

void test_town_apply_event_no_income_bonus()
{
    Town t(100, 1000, 100);
    Event e{ "Fireworks", 300, 5, 0 };
    t.applyEvent(e);
    assert(t.getBudget() == 700);
    assert(t.getIncomeBonusPercent() == 0);
    cout << "[OK] test_town_apply_event_no_income_bonus" << endl;
}

void test_town_apply_event_with_income_bonus()
{
    Town t(100, 1000, 100);
    Event e{ "Fair", 500, 10, 25 };
    t.applyEvent(e);
    assert(t.getBudget() == 500);
    assert(t.getIncomeBonusPercent() == 25);
    cout << "[OK] test_town_apply_event_with_income_bonus" << endl;
}

void test_town_apply_event_accumulates_percent()
{
    Town t(100, 5000, 100);
    t.applyEvent(Event{ "Fair", 500, 10, 25 });
    t.applyEvent(Event{ "City Day", 800, 20, 15 });
    assert(t.getIncomeBonusPercent() == 40);
    cout << "[OK] test_town_apply_event_accumulates_percent" << endl;
}

void test_town_end_week_only_population()
{
    Town t(100, 1000, 500);
    int income = t.endWeek();
    assert(income == 50);
    assert(t.getBudget() == 1050);
    cout << "[OK] test_town_end_week_only_population" << endl;
}

void test_town_end_week_with_buildings()
{
    Town t(1000, 5000, 100);
    t.buildBuilding(Building("House", 1000, 80, 20, 5, 10));
    t.buildBuilding(Building("Shop", 1500, 220, 10, 6, 12));
    int income = t.endWeek();
    assert(income == 313);
    cout << "[OK] test_town_end_week_with_buildings" << endl;
}

void test_town_end_week_with_income_bonus()
{
    Town t(1000, 5000, 100);
    t.buildBuilding(Building("House", 1000, 100, 0, 5, 10));
    t.applyEvent(Event{ "Fair", 500, 10, 25 });
    int income = t.endWeek();
    assert(income == 137);
    cout << "[OK] test_town_end_week_with_income_bonus" << endl;
}

void test_town_end_week_resets_bonus()
{
    Town t(1000, 5000, 100);
    t.applyEvent(Event{ "Fair", 500, 10, 25 });
    t.endWeek();
    assert(t.getIncomeBonusPercent() == 0);
    cout << "[OK] test_town_end_week_resets_bonus" << endl;
}

void test_mayor_initial()
{
    Mayor m("Ivan");
    assert(m.getName() == "Ivan");
    assert(m.getReputation() == 50);
    cout << "[OK] test_mayor_initial" << endl;
}

void test_mayor_default_name()
{
    Mayor m;
    assert(m.getName() == "Мэр");
    cout << "[OK] test_mayor_default_name" << endl;
}

void test_mayor_add_reputation()
{
    Mayor m;
    m.addReputation(10);
    assert(m.getReputation() == 60);
    m.addReputation(-5);
    assert(m.getReputation() == 55);
    cout << "[OK] test_mayor_add_reputation" << endl;
}

void test_mayor_clamps_lower_bound()
{
    Mayor m;
    m.addReputation(-1000);
    assert(m.getReputation() == 0);
    cout << "[OK] test_mayor_clamps_lower_bound" << endl;
}

void test_mayor_clamps_upper_bound()
{
    Mayor m;
    m.addReputation(1000);
    assert(m.getReputation() == 100);
    cout << "[OK] test_mayor_clamps_upper_bound" << endl;
}

void test_mayor_add_reputation_percent()
{
    Mayor m;
    m.addReputationPercent(10);
    assert(m.getReputation() == 55);
    cout << "[OK] test_mayor_add_reputation_percent" << endl;
}

void test_mayor_add_reputation_percent_min_one()
{
    Mayor m;
    m.addReputationPercent(1);
    assert(m.getReputation() == 51);
    cout << "[OK] test_mayor_add_reputation_percent_min_one" << endl;
}

void test_mayor_add_reputation_percent_clamped()
{
    Mayor m;
    m.addReputation(50);
    assert(m.getReputation() == 100);
    m.addReputationPercent(50);
    assert(m.getReputation() == 100);
    cout << "[OK] test_mayor_add_reputation_percent_clamped" << endl;
}

void test_events_catalog_size()
{
    vector<Event> list = getAvailableEvents();
    assert(list.size() == 4);
    cout << "[OK] test_events_catalog_size" << endl;
}

void test_events_catalog_names()
{
    vector<Event> list = getAvailableEvents();
    assert(list[0].name == "Салют");
    assert(list[1].name == "Ярмарка");
    assert(list[2].name == "День города");
    assert(list[3].name == "Парад");
    cout << "[OK] test_events_catalog_names" << endl;
}

void test_events_catalog_cost_ascending()
{
    vector<Event> list = getAvailableEvents();
    for (size_t i = 1; i < list.size(); ++i)
        assert(list[i].cost >= list[i - 1].cost);
    cout << "[OK] test_events_catalog_cost_ascending" << endl;
}

void test_events_catalog_positive_cost()
{
    vector<Event> list = getAvailableEvents();
    for (size_t i = 0; i < list.size(); ++i)
    {
        assert(list[i].cost > 0);
        assert(list[i].reputationBonus > 0);
        assert(list[i].incomeBoostPercent >= 0);
    }
    cout << "[OK] test_events_catalog_positive_cost" << endl;
}

void test_settings_save_load_roundtrip()
{
    SettingsFileGuard guard;

    clearSettingsQueues();
    citySizeValues.push(500);
    citySizeValues.push(1500);
    budgetValues.push(2000);
    budgetValues.push(8000);
    populationValues.push(200);
    populationValues.push(800);
    populationValues.push(1600);

    assert(saveSettingsToFile());

    clearSettingsQueues();
    assert(loadSettingsFromFile());

    assert((toVector(citySizeValues) == vector<int>{500, 1500}));
    assert((toVector(budgetValues) == vector<int>{2000, 8000}));
    assert((toVector(populationValues) == vector<int>{200, 800, 1600}));

    cout << "[OK] test_settings_save_load_roundtrip" << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    cout << "===== Lab3 test run =====" << endl;

    test_building_default_ctor();
    test_building_param_ctor();
    test_buildings_catalog_size();
    test_buildings_catalog_positive_fields();
    test_buildings_catalog_unique_names();
    test_buildings_catalog_contains_required();

    test_town_initial_state();
    test_town_build_success();
    test_town_build_no_money();
    test_town_build_no_space();
    test_town_build_max_count();
    test_town_build_exactly_full_area();
    test_town_apply_event_no_income_bonus();
    test_town_apply_event_with_income_bonus();
    test_town_apply_event_accumulates_percent();
    test_town_end_week_only_population();
    test_town_end_week_with_buildings();
    test_town_end_week_with_income_bonus();
    test_town_end_week_resets_bonus();

    test_mayor_initial();
    test_mayor_default_name();
    test_mayor_add_reputation();
    test_mayor_clamps_lower_bound();
    test_mayor_clamps_upper_bound();
    test_mayor_add_reputation_percent();
    test_mayor_add_reputation_percent_min_one();
    test_mayor_add_reputation_percent_clamped();

    test_events_catalog_size();
    test_events_catalog_names();
    test_events_catalog_cost_ascending();
    test_events_catalog_positive_cost();

    test_settings_save_load_roundtrip();

    cout << "===== All tests passed =====" << endl;
    return 0;
}