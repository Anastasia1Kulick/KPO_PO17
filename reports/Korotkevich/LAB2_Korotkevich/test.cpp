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

int nextFromList(int current, const vector<int>& vals) {
    for (size_t i = 0; i < vals.size(); i++) {
        if (vals[i] == current) return vals[(i + 1) % vals.size()];
    }
    return vals[0];
}

struct Settings {
    int citySize = 1;
    int budget = 1000;
    int citizens = 100;
};

void saveSettings(const Settings& s, const string& file) {
    ofstream f(file);
    f << s.citySize << "\n" << s.budget << "\n" << s.citizens << "\n";
}

bool loadSettings(Settings& s, const string& file) {
    ifstream f(file);
    if (!f) return false;
    f >> s.citySize >> s.budget >> s.citizens;
    return true;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "=== Unit tests for lab2 ===\n\n";

    {
        Settings s;
        check(s.citySize == 1, "default citySize = 1");
        check(s.budget == 1000, "default budget = 1000");
        check(s.citizens == 100, "default citizens = 100");
    }

    {
        vector<int> v = { 1000, 5000, 10000 };
        check(nextFromList(1000, v) == 5000, "1000 -> 5000");
        check(nextFromList(5000, v) == 10000, "5000 -> 10000");
        check(nextFromList(10000, v) == 1000, "10000 -> 1000");
    }

    {
        Settings s;
        s.citySize = 2; s.budget = 5000; s.citizens = 300;
        saveSettings(s, "_test.txt");

        Settings t;
        bool ok = loadSettings(t, "_test.txt");
        check(ok, "loadSettings returns true");
        check(t.citySize == 2 && t.budget == 5000 && t.citizens == 300,
            "loaded values match saved");
        remove("_test.txt");
    }

    {
        Settings s;
        check(!loadSettings(s, "_no_such_file.txt"),
            "loadSettings false if file missing");
    }

    cout << "\n=== Result ===\n";
    cout << "Passed: " << passed << "\n";
    cout << "Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}