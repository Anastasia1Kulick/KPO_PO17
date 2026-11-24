#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const string SETTINGS_FILE = "settings.txt";
const string VALUES_FILE = "values.txt";

struct Settings {
    int citySize = 1;
    int budget = 1000;
    int citizens = 100;
};

bool fileExists(const string& name) {
    ifstream f(name);
    return f.good();
}

void saveSettings(const Settings& s) {
    ofstream f(SETTINGS_FILE);
    if (!f) return;
    f << s.citySize << "\n" << s.budget << "\n" << s.citizens << "\n";
}

bool loadSettings(Settings& s) {
    ifstream f(SETTINGS_FILE);
    if (!f) return false;
    f >> s.citySize >> s.budget >> s.citizens;
    return true;
}

void createDefaultValuesFile() {
    if (fileExists(VALUES_FILE)) return;
    ofstream f(VALUES_FILE);
    f << "1 2 3\n";
    f << "1000 5000 10000\n";
    f << "100 200 300 400 500\n";
}

void loadValues(vector<int>& cityVals,
    vector<int>& budgetVals,
    vector<int>& citizenVals) {
    ifstream f(VALUES_FILE);
    if (!f) return;

    string line;

    getline(f, line);
    {
        istringstream ss(line);
        int v;
        while (ss >> v) cityVals.push_back(v);
    }

    getline(f, line);
    {
        istringstream ss(line);
        int v;
        while (ss >> v) budgetVals.push_back(v);
    }

    getline(f, line);
    {
        istringstream ss(line);
        int v;
        while (ss >> v) citizenVals.push_back(v);
    }
}

int nextFromList(int current, const vector<int>& vals) {
    for (size_t i = 0; i < vals.size(); i++) {
        if (vals[i] == current) {
            return vals[(i + 1) % vals.size()];
        }
    }
    return vals[0];
}

void nextCitySize(Settings& s, const vector<int>& v) { s.citySize = nextFromList(s.citySize, v); }
void nextBudget(Settings& s, const vector<int>& v) { s.budget = nextFromList(s.budget, v); }
void nextCitizens(Settings& s, const vector<int>& v) { s.citizens = nextFromList(s.citizens, v); }

int readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Ошибка ввода. Повторите: ";
    }
    return value;
}

void pause() {
    cout << "\nНажмите Enter...";
    cin.ignore(10000, '\n');
    cin.get();
}

void startGame() { cout << "\nИгра началась!\n"; pause(); }
void loadGame() { cout << "\nЗагрузка пока недоступна.\n"; pause(); }

void showSettings(Settings& s,
    const vector<int>& cv,
    const vector<int>& bv,
    const vector<int>& civ) {
    while (true) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Размер города: " << s.citySize << "\n";
        cout << "2. Начальный бюджет: " << s.budget << "\n";
        cout << "3. Количество жителей: " << s.citizens << "\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";

        int choice = readInt();
        if (choice == 0) return;
        if (choice == 1) nextCitySize(s, cv);
        else if (choice == 2) nextBudget(s, bv);
        else if (choice == 3) nextCitizens(s, civ);
        else cout << "Нет такого пункта.\n";

        saveSettings(s);
    }
}

void showAbout() {
    cout << "\nВерсия программы: 2.0\n";
    cout << "Студент: Иванов Иван\n";
    pause();
}

int main() {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    createDefaultValuesFile();

    Settings settings;
    if (!loadSettings(settings)) {
        saveSettings(settings);
    }

    vector<int> cityVals, budgetVals, citizenVals;
    loadValues(cityVals, budgetVals, citizenVals);

    while (true) {
        cout << "\n=== Город мечты ===\n";
        cout << MENU_START << ". Начать игру\n";
        cout << MENU_LOAD << ". Загрузить игру\n";
        cout << MENU_SETTINGS << ". Настройки\n";
        cout << MENU_ABOUT << ". О программе\n";
        cout << MENU_EXIT << ". Выход\n";
        cout << "Выбор: ";

        int choice = readInt();
        if (choice == MENU_START) startGame();
        else if (choice == MENU_LOAD) loadGame();
        else if (choice == MENU_SETTINGS) showSettings(settings, cityVals, budgetVals, citizenVals);
        else if (choice == MENU_ABOUT) showAbout();
        else if (choice == MENU_EXIT) break;
        else cout << "Нет такого пункта.\n";
    }
    return 0;
}