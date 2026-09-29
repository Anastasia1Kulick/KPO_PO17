#include <iostream>
using namespace std;

int passed = 0;
int failed = 0;

void check(bool condition, const char* name) {
    if (condition) {
        cout << "[OK]   " << name << "\n";
        passed++;
    }
    else {
        cout << "[FAIL] " << name << "\n";
        failed++;
    }
}

// ===== Копия логики настроек из lab1kpo.cpp =====
struct Settings {
    int citySize = 1;
    int budget = 1000;
    int citizens = 100;
};

void nextCitySize(Settings& s) {
    s.citySize++;
    if (s.citySize > 3) s.citySize = 1;
}

void nextBudget(Settings& s) {
    if (s.budget == 1000) s.budget = 5000;
    else if (s.budget == 5000) s.budget = 10000;
    else s.budget = 1000;
}

void nextCitizens(Settings& s) {
    s.citizens += 100;
    if (s.citizens > 500) s.citizens = 100;
}
// ================================================

void testDefaults() {
    Settings s;
    check(s.citySize == 1, "default: citySize = 1");
    check(s.budget == 1000, "default: budget = 1000");
    check(s.citizens == 100, "default: citizens = 100");
}

void testCitySize() {
    Settings s;
    nextCitySize(s); check(s.citySize == 2, "citySize: 1 -> 2");
    nextCitySize(s); check(s.citySize == 3, "citySize: 2 -> 3");
    nextCitySize(s); check(s.citySize == 1, "citySize: 3 -> 1 (cycle)");
}

void testBudget() {
    Settings s;
    nextBudget(s); check(s.budget == 5000, "budget: 1000 -> 5000");
    nextBudget(s); check(s.budget == 10000, "budget: 5000 -> 10000");
    nextBudget(s); check(s.budget == 1000, "budget: 10000 -> 1000 (cycle)");
}

void testCitizens() {
    Settings s;
    nextCitizens(s); check(s.citizens == 200, "citizens: 100 -> 200");
    nextCitizens(s); check(s.citizens == 300, "citizens: 200 -> 300");
    nextCitizens(s); check(s.citizens == 400, "citizens: 300 -> 400");
    nextCitizens(s); check(s.citizens == 500, "citizens: 400 -> 500");
    nextCitizens(s); check(s.citizens == 100, "citizens: 500 -> 100 (cycle)");
}

void testIndependence() {
    Settings a, b;
    nextCitySize(a);
    nextBudget(b);
    check(a.citySize == 2 && a.budget == 1000, "A independent from B");
    check(b.citySize == 1 && b.budget == 5000, "B independent from A");
}

int main() {
    cout << "=== Unit tests ===\n\n";

    testDefaults();
    testCitySize();
    testBudget();
    testCitizens();
    testIndependence();

    cout << "\n=== Result ===\n";
    cout << "Passed: " << passed << "\n";
    cout << "Failed: " << failed << "\n";

    return failed == 0 ? 0 : 1;
}