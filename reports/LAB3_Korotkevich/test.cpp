#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;
int passed = 0;
int failed = 0;
void check(bool cond, const string& name) {
    if (cond) { cout << "[OK]   " << name << "\n"; passed++; }
    else { cout << "[FAIL] " << name << "\n"; failed++; }
}
class Building {
public:
    string name; int cost; int incomeBonus; int citizensBonus; bool isBuilt;
    Building(string n, int c, int inc, int cit) : name(n), cost(c), incomeBonus(inc), citizensBonus(cit), isBuilt(false) {}
};
class Town {
public:
    int size, budget, citizens;
    vector<Building> availableBuildings;
    Town(int s, int b, int c) : size(s), budget(b), citizens(c) {
        availableBuildings.push_back(Building("Жилой дом", 500, 0, 50));
    }
};
class Mayor {
public:
    bool build(Town& t, int index) {
        if (index < 0 || index >= (int)t.availableBuildings.size()) return false;
        Building& b = t.availableBuildings[index];
        if (b.isBuilt) return false;
        if (t.budget < b.cost) return false;
        t.budget -= b.cost;
        t.citizens += b.citizensBonus;
        b.isBuilt = true;
        return true;
    }
};
int nextFromList(int current, const vector<int>& vals) {
    for (size_t i = 0; i < vals.size(); i++) {
        if (vals[i] == current) return vals[(i + 1) % vals.size()];
    }
    return vals[0];
}
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    cout << "=== Unit tests for lab3 ===\n\n";
    {
        vector<int> v = { 1000, 5000, 10000 };
        check(nextFromList(1000, v) == 5000, "nextFromList works");
    }
    {
        Town town(2, 2000, 100);
        Mayor mayor;
        check(town.budget == 2000, "initial budget = 2000");
        bool success = mayor.build(town, 0);
        check(success, "build house success");
        check(town.budget == 1500, "budget reduced by 500");
        check(town.citizens == 150, "citizens increased by 50");
        bool fail = mayor.build(town, 0);
        check(!fail, "cannot build twice");
    }
    {
        Town town(2, 100, 100);
        Mayor mayor;
        bool fail = mayor.build(town, 0);
        check(!fail, "not enough budget fails");
        check(town.budget == 100, "budget not changed on fail");
    }
    {
        Building custom("МойДом", 800, 200, 30);
        check(custom.name == "МойДом", "custom building name");
        check(custom.cost == 800, "custom building cost");
        check(custom.incomeBonus == 200, "custom building income");
        check(custom.citizensBonus == 30, "custom building citizens bonus");
    }
    cout << "\n=== Result ===\n";
    cout << "Passed: " << passed << "\n";
    cout << "Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
