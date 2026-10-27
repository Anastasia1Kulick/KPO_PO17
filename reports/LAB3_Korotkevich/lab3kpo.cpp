#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <ctime>
using namespace std;
class Building {
public:
    string name;
    int cost;
    int incomeBonus;
    int citizensBonus;
    bool isBuilt;
    Building(string n, int c, int inc, int cit)
        : name(n), cost(c), incomeBonus(inc), citizensBonus(cit), isBuilt(false) {
    }
};
class Town {
public:
    int size;
    int budget;
    int citizens;
    vector<Building> availableBuildings;
    Town(int s, int b, int c) : size(s), budget(b), citizens(c) {
        availableBuildings.push_back(Building("Жилой дом", 500, 0, 50));
        availableBuildings.push_back(Building("Магазин", 1000, 300, 10));
        availableBuildings.push_back(Building("Фабрика", 2500, 800, 20));
        availableBuildings.push_back(Building("Парк", 1500, 100, 100));
        ifstream f("custom_buildings.txt");
        if (f) {
            string n;
            int c, inc, cit;
            while (f >> n >> c >> inc >> cit) {
                availableBuildings.push_back(Building(n, c, inc, cit));
            }
        }
    }
};
class Mayor {
public:
    bool build(Town& t, int index) {
        if (index < 0 || index >= (int)t.availableBuildings.size()) return false;
        Building& b = t.availableBuildings[index];
        if (b.isBuilt) {
            cout << "Это здание уже построено!\n";
            return false;
        }
        if (t.budget < b.cost) {
            cout << "Недостаточно бюджета! Нужно: " << b.cost << "\n";
            return false;
        }
        t.budget -= b.cost;
        t.citizens += b.citizensBonus;
        b.isBuilt = true;
        cout << "Успешно построено: " << b.name << "!\n";
        return true;
    }
    void playWeek(Town& t) {
        cout << "\n--- Итоги недели ---\n";
        int baseIncome = t.citizens * 2;
        int buildingsIncome = 0;
        for (size_t i = 0; i < t.availableBuildings.size(); ++i) {
            if (t.availableBuildings[i].isBuilt) {
                buildingsIncome += t.availableBuildings[i].incomeBonus;
            }
        }
        int totalIncome = baseIncome + buildingsIncome;
        t.budget += totalIncome;
        cout << "Доход от налогов (" << t.citizens << " жителей): " << baseIncome << "\n";
        cout << "Доход от зданий: " << buildingsIncome << "\n";
        cout << "Итого пополнено в бюджет: " << totalIncome << "\n";
        cout << "Текущий бюджет: " << t.budget << "\n";
    }
};
const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;
const string SETTINGS_FILE = "settings.txt";
const string TOWN_STATE_FILE = "town_state.txt";
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
    if (!(f >> s.citySize >> s.budget >> s.citizens)) return false;
    return true;
}
void saveTownState(const Town& t) {
    ofstream f(TOWN_STATE_FILE);
    if (!f) return;
    f << t.budget << "\n" << t.citizens << "\n";
    for (size_t i = 0; i < t.availableBuildings.size(); ++i) {
        f << t.availableBuildings[i].isBuilt << "\n";
    }
}
bool loadTownState(Town& t) {
    ifstream f(TOWN_STATE_FILE);
    if (!f) return false;
    if (!(f >> t.budget >> t.citizens)) return false;
    for (size_t i = 0; i < t.availableBuildings.size(); ++i) {
        int built;
        if (!(f >> built)) return false;
        t.availableBuildings[i].isBuilt = (built != 0);
    }
    return true;
}
void createDefaultValuesFile() {
    if (fileExists(VALUES_FILE)) return;
    ofstream f(VALUES_FILE);
    f << "1 2 3\n";
    f << "1000 5000 10000\n";
    f << "100 200 300 400 500\n";
}
void loadValues(vector<int>& cityVals, vector<int>& budgetVals, vector<int>& citizenVals) {
    ifstream f(VALUES_FILE);
    if (!f) return;
    string line;
    getline(f, line); { istringstream ss(line); int v; while (ss >> v) cityVals.push_back(v); }
    getline(f, line); { istringstream ss(line); int v; while (ss >> v) budgetVals.push_back(v); }
    getline(f, line); { istringstream ss(line); int v; while (ss >> v) citizenVals.push_back(v); }
}
int nextFromList(int current, const vector<int>& vals) {
    for (size_t i = 0; i < vals.size(); i++) {
        if (vals[i] == current) return vals[(i + 1) % vals.size()];
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
void startGame(Settings& s) {
    cout << "\n=== Добро пожаловать, Мэр! ===\n";
    Town town(s.citySize, s.budget, s.citizens);
    if (loadTownState(town)) {
        cout << "[Загружено сохранение города]\n";
    }
    Mayor mayor;
    while (true) {
        cout << "\n--- Управление городом ---\n";
        cout << "Бюджет: " << town.budget << " | Жители: " << town.citizens << "\n";
        cout << "Доступные действия:\n";
        for (size_t i = 0; i < town.availableBuildings.size(); ++i) {
            cout << i + 1 << ". Построить: " << town.availableBuildings[i].name;
            cout << " (Цена: " << town.availableBuildings[i].cost << ")";
            if (town.availableBuildings[i].isBuilt) cout << " [ПОСТРОЕНО]";
            cout << "\n";
        }
        cout << "0. Закончить неделю (получить доход)\n";
        cout << "-1. Выйти в главное меню\n";
        cout << "Выбор: ";
        int choice = readInt();
        if (choice == -1) {
            saveTownState(town);
            break;
        }
        else if (choice == 0) {
            mayor.playWeek(town);
            saveTownState(town);
            cout << "\nНеделя прошла! Нажмите Enter...";
            cin.ignore(10000, '\n');
            cin.get();
        }
        else {
            mayor.build(town, choice - 1);
            saveTownState(town);
        }
    }
    pause();
}
void loadGame() { cout << "\nЗагрузка пока недоступна.\n"; pause(); }
void createCustomBuilding() {
    cout << "\n--- Создание своего здания ---\n";
    cout << "Введите название (без пробелов): ";
    string name;
    cin >> name;
    cout << "Введите цену постройки: ";
    int cost = readInt();
    cout << "Введите доход от здания за неделю: ";
    int income = readInt();
    cout << "Введите бонус к жителям: ";
    int cit = readInt();
    ofstream f("custom_buildings.txt", ios::app);
    if (f) {
        f << name << " " << cost << " " << income << " " << cit << "\n";
        cout << "Здание успешно добавлено в игру!\n";
    }
    else {
        cout << "Ошибка при сохранении здания.\n";
    }
}
void showSettings(Settings& s, const vector<int>& cv, const vector<int>& bv, const vector<int>& civ) {
    while (true) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Размер города: " << s.citySize << "\n";
        cout << "2. Начальный бюджет: " << s.budget << "\n";
        cout << "3. Количество жителей: " << s.citizens << "\n";
        cout << "4. Создать свое здание\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";
        int choice = readInt();
        if (choice == 0) {
            saveSettings(s);
            remove(TOWN_STATE_FILE.c_str());
            return;
        }
        if (choice == 1) nextCitySize(s, cv);
        else if (choice == 2) nextBudget(s, bv);
        else if (choice == 3) nextCitizens(s, civ);
        else if (choice == 4) createCustomBuilding();
        else cout << "Нет такого пункта.\n";
    }
}
void showAbout() {
    cout << "\n=== О программе ===\n";
    cout << "Версия программы: 3.0\n";
    cout << "Студент: Иванов Иван\n";
    cout << "\nСаундтреки:\n";
    cout << "1. Городская суета - Main Theme\n";
    cout << "2. Стройка века - Building Theme\n";
    cout << "3. Налоговая инспекция - Event Theme\n";
    pause();
}
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(time(0));
    createDefaultValuesFile();
    Settings settings;
    if (!loadSettings(settings)) saveSettings(settings);
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
        if (choice == MENU_START) startGame(settings);
        else if (choice == MENU_LOAD) loadGame();
        else if (choice == MENU_SETTINGS) showSettings(settings, cityVals, budgetVals, citizenVals);
        else if (choice == MENU_ABOUT) showAbout();
        else if (choice == MENU_EXIT) break;
        else cout << "Нет такого пункта.\n";
    }
    return 0;
}
