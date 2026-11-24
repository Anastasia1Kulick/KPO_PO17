#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>
#include <clocale>
#include <chrono>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;

// ==================== MAIN MENU CONSTANTS ====================
const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

// ==================== SETTINGS SUBMENU CONSTANTS ====================
const int SET_DIFFICULTY = 1;
const int SET_ANIMALS = 2;
const int SET_ADD_ANIMAL = 3;
const int SET_BACK = 4;

// ==================== ANIMALS SUBMENU CONSTANTS ====================
const int ANIMAL_EDIT = 1;
const int ANIMAL_DELETE = 2;
const int ANIMAL_BACK = 3;

const int DIFF_COUNT = 3;

// ==================== GAME PROCESS CONSTANTS ====================
const int ACTION_FEED = 1;
const int ACTION_CARE = 2;
const int ACTION_BUY_FOOD = 3;
const int ACTION_BUY_ANIMAL = 4;
const int ACTION_END_DAY = 5;
const int ACTION_SAVE_GAME = 6;

// ==================== PROGRAM VERSION ====================
const string PROGRAM_VERSION = "4.0";
const string STUDENT_NAME = "Viarenich Daniil";
const string SETTINGS_FILE = "settings.txt";
const string SAVE_FILE = "savegame.txt";
const string LASTSAVE_FILE = "lastsave.txt";

// ==================== AUTO-SAVE SETTINGS ====================
const int AUTO_SAVE_INTERVAL_MIN = 30; // минут между сохранени€ми

// ==================== GLOBAL SETTINGS ====================
string difficulty = "Medium";
map<string, int> startingAnimals;

// ==================== CLASS FOOD ====================
class Food
{
private:
    string name;
    int price;
    int nutrition;

public:
    Food() : name(""), price(0), nutrition(0) {}
    Food(const string& n, int p, int nut) : name(n), price(p), nutrition(nut) {}

    string getName()      const { return name; }
    int    getPrice()     const { return price; }
    int    getNutrition() const { return nutrition; }
};

// ==================== CLASS ANIMAL ====================
class Animal
{
private:
    string type;
    string name;
    int    hunger;
    int    happiness;
    int    health;
    string foodType;
    int    buyPrice;
    int    income;

public:
    Animal()
        : type(""), name(""), hunger(50), happiness(50), health(100),
        foodType(""), buyPrice(0), income(0) {
    }

    Animal(const string& t, const string& n, const string& food,
        int buyP, int inc)
        : type(t), name(n), hunger(50), happiness(50), health(100),
        foodType(food), buyPrice(buyP), income(inc) {
    }

    // --- Getters ---
    string getType()      const { return type; }
    string getName()      const { return name; }
    int    getHunger()    const { return hunger; }
    int    getHappiness() const { return happiness; }
    int    getHealth()    const { return health; }
    string getFoodType()  const { return foodType; }
    int    getBuyPrice()  const { return buyPrice; }
    int    getIncome()    const { return income; }

    bool isAlive() const { return health > 0; }

    // Feeding
    void feed(int nutrition)
    {
        hunger = max(0, hunger - nutrition);
        if (hunger < 20) happiness = min(100, happiness + 5);
    }

    // Care
    void care()
    {
        happiness = min(100, happiness + 10);
        health = min(100, health + 5);
    }

    // Daily update
    void dailyUpdate()
    {
        hunger = min(100, hunger + 20);
        if (hunger > 80)
        {
            happiness = max(0, happiness - 10);
            health = max(0, health - 5);
        }
        if (health == 0) happiness = 0;
    }

    // Daily income
    int dailyIncome() const
    {
        if (health <= 0) return 0;
        return (income * happiness) / 100;
    }

    // ¬осстановление полного состо€ни€ (дл€ загрузки)
    void restore(const string& t, const string& n,
        int h, int hp, int hlth,
        const string& food, int bp, int inc)
    {
        type = t;
        name = n;
        hunger = h;
        happiness = hp;
        health = hlth;
        foodType = food;
        buyPrice = bp;
        income = inc;
    }
};

// ==================== CLASS PLAYER ====================
class Player
{
private:
    int money;
    int actionsLeft;
    int day;
    vector<Animal> animals;
    map<string, int> foodStock;

public:
    Player() : money(0), actionsLeft(0), day(1) {}

    int getMoney()       const { return money; }
    int getActionsLeft() const { return actionsLeft; }
    int getDay()         const { return day; }
    const vector<Animal>& getAnimals() const { return animals; }
    const map<string, int>& getFoodStock() const { return foodStock; }

    // Initialization from settings
    void initFromSettings(const map<string, int>& startAnimals,
        int startMoney, int actionsPerDay)
    {
        money = startMoney;
        actionsLeft = actionsPerDay;
        day = 1;
        animals.clear();
        foodStock.clear();

        int counter = 1;
        for (const auto& pair : startAnimals)
        {
            for (int i = 0; i < pair.second; i++)
            {
                animals.push_back(createAnimalByType(pair.first, counter++));
            }
        }

        foodStock["Hay"] = 5;
        foodStock["Grain"] = 5;
        foodStock["Swill"] = 5;
    }

    // Create animal by type
    static Animal createAnimalByType(const string& type, int id)
    {
        string name = type + "_" + to_string(id);

        if (type == "Cows")     return Animal(type, name, "Hay", 100, 15);
        if (type == "Chickens") return Animal(type, name, "Grain", 50, 5);
        if (type == "Sheep")    return Animal(type, name, "Hay", 80, 10);
        if (type == "Goats")    return Animal(type, name, "Hay", 70, 8);
        if (type == "Pigs")     return Animal(type, name, "Swill", 120, 18);

        return Animal(type, name, "Hay", 60, 7);
    }

    // --- Actions ---

    bool feedAnimal(int index, int nutrition)
    {
        if (index < 0 || index >= (int)animals.size()) return false;
        if (actionsLeft <= 0) return false;

        Animal& a = animals[index];
        string food = a.getFoodType();

        if (foodStock[food] <= 0) return false;

        foodStock[food]--;
        a.feed(nutrition);
        actionsLeft--;
        return true;
    }

    bool careAnimal(int index)
    {
        if (index < 0 || index >= (int)animals.size()) return false;
        if (actionsLeft <= 0) return false;

        animals[index].care();
        actionsLeft--;
        return true;
    }

    bool buyFood(const string& foodName, int price, int amount)
    {
        if (actionsLeft <= 0) return false;
        int cost = price * amount;
        if (money < cost) return false;

        money -= cost;
        foodStock[foodName] += amount;
        actionsLeft--;
        return true;
    }

    bool buyAnimal(const Animal& a)
    {
        if (actionsLeft <= 0) return false;
        if (money < a.getBuyPrice()) return false;

        money -= a.getBuyPrice();
        animals.push_back(a);
        actionsLeft--;
        return true;
    }

    void endDay()
    {
        int income = 0;
        for (auto& a : animals)
        {
            if (a.isAlive())
            {
                income += a.dailyIncome();
                a.dailyUpdate();
            }
        }
        money += income;

        animals.erase(
            remove_if(animals.begin(), animals.end(),
                [](const Animal& a) { return !a.isAlive(); }),
            animals.end());

        day++;
    }

    void resetActions(int perDay) { actionsLeft = perDay; }

    // ¬осстановление всего состо€ни€ (дл€ загрузки)
    void restoreState(int m, int acts, int d,
        const vector<Animal>& a,
        const map<string, int>& foods)
    {
        money = m;
        actionsLeft = acts;
        day = d;
        animals = a;
        foodStock = foods;
    }
};

// ==================== HELPER FUNCTIONS ====================
void setupLocale()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
}

void clearInput()
{
    if (cin.eof()) exit(0);
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidDifficulty(const string& d)
{
    return d == "Easy" || d == "Medium" || d == "Hard";
}

int actionsPerDay()
{
    if (difficulty == "Easy") return 5;
    if (difficulty == "Hard") return 3;
    return 4;
}

int startMoney()
{
    if (difficulty == "Easy") return 2000;
    if (difficulty == "Hard") return 500;
    return 1000;
}

// ==================== TIME HELPERS ====================
long long nowSeconds()
{
    return chrono::duration_cast<chrono::seconds>(
        chrono::system_clock::now().time_since_epoch()).count();
}

long long getLastSaveTime()
{
    ifstream fin(LASTSAVE_FILE);
    if (!fin.is_open()) return 0;
    long long t = 0;
    fin >> t;
    return t;
}

void setLastSaveTime(long long t)
{
    ofstream fout(LASTSAVE_FILE, ios::trunc);
    if (fout.is_open()) fout << t;
}

long long secondsUntilSaveAllowed()
{
    long long elapsed = nowSeconds() - getLastSaveTime();
    long long need = (long long)AUTO_SAVE_INTERVAL_MIN * 60;
    if (elapsed >= need) return 0;
    return need - elapsed;
}

// ==================== FILE FUNCTIONS (SETTINGS) ====================
void setDefaults()
{
    difficulty = "Medium";
    startingAnimals.clear();
    startingAnimals["Cows"] = 2;
    startingAnimals["Chickens"] = 3;
}

void saveSettings()
{
    ofstream fout(SETTINGS_FILE, ios::trunc);
    if (!fout.is_open()) return;

    fout << "difficulty=" << difficulty << "\n";
    for (const auto& pair : startingAnimals)
        fout << "animals=" << pair.first << ":" << pair.second << "\n";
}

bool loadSettings()
{
    ifstream fin(SETTINGS_FILE);
    if (!fin.is_open()) return false;

    string line;
    string loadedDifficulty;
    map<string, int> loadedAnimals;

    while (getline(fin, line))
    {
        if (line.empty()) continue;
        size_t eqPos = line.find('=');
        if (eqPos == string::npos) continue;

        string key = line.substr(0, eqPos);
        string value = line.substr(eqPos + 1);

        if (key == "difficulty")
        {
            if (isValidDifficulty(value)) loadedDifficulty = value;
        }
        else if (key == "animals")
        {
            size_t colonPos = value.find(':');
            if (colonPos == string::npos) continue;
            try {
                int count = stoi(value.substr(colonPos + 1));
                if (count >= 0 && count <= 1000)
                    loadedAnimals[value.substr(0, colonPos)] = count;
            }
            catch (...) {}
        }
    }
    fin.close();

    if (!loadedDifficulty.empty()) difficulty = loadedDifficulty;
    if (!loadedAnimals.empty())    startingAnimals = loadedAnimals;
    return true;
}

void initSettings()
{
    if (!loadSettings())
    {
        cout << "Settings file not found. Defaults created.\n";
        setDefaults();
        saveSettings();
    }
}

// ==================== SAVE / LOAD GAME ====================
bool saveGame(const Player& p)
{
    ofstream fout(SAVE_FILE, ios::trunc);
    if (!fout.is_open()) return false;

    fout << "version=" << PROGRAM_VERSION << "\n";
    fout << "money=" << p.getMoney() << "\n";
    fout << "actions=" << p.getActionsLeft() << "\n";
    fout << "day=" << p.getDay() << "\n";
    fout << "difficulty=" << difficulty << "\n";

    for (const auto& f : p.getFoodStock())
        fout << "food=" << f.first << ":" << f.second << "\n";

    for (const auto& a : p.getAnimals())
    {
        fout << "animal=" << a.getType() << ";"
            << a.getName() << ";"
            << a.getHunger() << ";"
            << a.getHappiness() << ";"
            << a.getHealth() << ";"
            << a.getFoodType() << ";"
            << a.getBuyPrice() << ";"
            << a.getIncome() << "\n";
    }

    fout.close();
    setLastSaveTime(nowSeconds());
    return true;
}

bool loadGameFromFile(Player& p)
{
    ifstream fin(SAVE_FILE);
    if (!fin.is_open()) return false;

    int money = 0;
    int actions = 0;
    int day = 1;
    string loadedDiff;
    map<string, int> foods;
    vector<Animal>   animals;

    string line;
    while (getline(fin, line))
    {
        if (line.empty()) continue;
        size_t eq = line.find('=');
        if (eq == string::npos) continue;
        string key = line.substr(0, eq);
        string val = line.substr(eq + 1);

        try {
            if (key == "money")           money = stoi(val);
            else if (key == "actions")    actions = stoi(val);
            else if (key == "day")        day = stoi(val);
            else if (key == "difficulty") {
                if (isValidDifficulty(val)) loadedDiff = val;
            }
            else if (key == "food") {
                size_t c = val.find(':');
                if (c != string::npos)
                    foods[val.substr(0, c)] = stoi(val.substr(c + 1));
            }
            else if (key == "animal") {
                vector<string> parts;
                stringstream ss(val);
                string token;
                while (getline(ss, token, ';')) parts.push_back(token);
                if (parts.size() != 8) continue;

                Animal a;
                a.restore(parts[0], parts[1],
                    stoi(parts[2]), stoi(parts[3]), stoi(parts[4]),
                    parts[5], stoi(parts[6]), stoi(parts[7]));
                animals.push_back(a);
            }
        }
        catch (...) { /* пропускаем битую строку */ }
    }
    fin.close();

    if (!loadedDiff.empty()) difficulty = loadedDiff;
    p.restoreState(money, actions, day, animals, foods);
    return true;
}

// ==================== GAME PROCESS ====================

void printGameState(const Player& p)
{
    cout << "\n+---------------------------------------+\n";
    cout << "| Day: " << p.getDay()
        << " | Money: " << p.getMoney()
        << " | Actions: " << p.getActionsLeft() << "\n";
    cout << "+---------------------------------------+\n";

    cout << "Animals (" << p.getAnimals().size() << "):\n";
    if (p.getAnimals().empty())
    {
        cout << "  (no animals)\n";
    }
    else
    {
        int i = 1;
        for (const auto& a : p.getAnimals())
        {
            cout << "  " << i++ << ". " << a.getType()
                << " \"" << a.getName() << "\" Ч "
                << "hunger:" << a.getHunger()
                << " happiness:" << a.getHappiness()
                << " health:" << a.getHealth() << "\n";
        }
    }

    cout << "Food: ";
    if (p.getFoodStock().empty()) cout << "(none)";
    else for (const auto& f : p.getFoodStock())
        cout << f.first << "=" << f.second << " ";
    cout << "\n";
}

vector<Food> foodCatalog()
{
    return {
        Food("Hay",   10, 30),
        Food("Grain",  5, 20),
        Food("Swill", 15, 40)
    };
}

vector<Animal> animalCatalog()
{
    return {
        Player::createAnimalByType("Cows",     0),
        Player::createAnimalByType("Chickens", 0),
        Player::createAnimalByType("Sheep",    0),
        Player::createAnimalByType("Goats",    0),
        Player::createAnimalByType("Pigs",     0)
    };
}

void actionFeed(Player& p)
{
    if (p.getAnimals().empty()) { cout << "No animals.\n"; return; }

    cout << "Choose an animal to feed:\n";
    int i = 1;
    for (const auto& a : p.getAnimals())
    {
        cout << "  " << i++ << ". " << a.getType()
            << " \"" << a.getName() << "\" (eats " << a.getFoodType()
            << ", hunger " << a.getHunger() << ")\n";
    }
    cout << "Number (0 Ч cancel): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)p.getAnimals().size()) return;

    Animal& a = const_cast<Animal&>(p.getAnimals()[idx - 1]);
    string food = a.getFoodType();
    int stock = p.getFoodStock().count(food) ? p.getFoodStock().at(food) : 0;

    if (stock <= 0)
    {
        cout << "No food \"" << food << "\" in stock!\n";
        return;
    }

    int nutrition = 20;
    for (const auto& f : foodCatalog())
        if (f.getName() == food) { nutrition = f.getNutrition(); break; }

    if (p.feedAnimal(idx - 1, nutrition))
        cout << "Animal fed.\n";
    else
        cout << "Failed to feed.\n";
}

void actionCare(Player& p)
{
    if (p.getAnimals().empty()) { cout << "No animals.\n"; return; }

    cout << "Choose an animal to care for:\n";
    int i = 1;
    for (const auto& a : p.getAnimals())
        cout << "  " << i++ << ". " << a.getType() << " \"" << a.getName() << "\"\n";
    cout << "Number (0 Ч cancel): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)p.getAnimals().size()) return;

    if (p.careAnimal(idx - 1))
        cout << "Animal cared for.\n";
    else
        cout << "No actions left!\n";
}

void actionBuyFood(Player& p)
{
    auto catalog = foodCatalog();
    cout << "Food shop:\n";
    int i = 1;
    for (const auto& f : catalog)
    {
        cout << "  " << i++ << ". " << f.getName()
            << " Ч price " << f.getPrice()
            << ", nutrition " << f.getNutrition() << "\n";
    }
    cout << "Number (0 Ч cancel): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)catalog.size()) return;

    const Food& f = catalog[idx - 1];
    cout << "How much to buy? ";
    int amount; cin >> amount;
    if (cin.fail()) { clearInput(); return; }
    if (amount <= 0) return;

    if (p.buyFood(f.getName(), f.getPrice(), amount))
        cout << "Bought " << amount << " x " << f.getName() << "\n";
    else
        cout << "Not enough money or actions!\n";
}

void actionBuyAnimal(Player& p)
{
    auto catalog = animalCatalog();
    cout << "Animal shop:\n";
    int i = 1;
    for (const auto& a : catalog)
    {
        cout << "  " << i++ << ". " << a.getType()
            << " Ч price " << a.getBuyPrice()
            << ", eats " << a.getFoodType()
            << ", income " << a.getIncome() << "/day\n";
    }
    cout << "Number (0 Ч cancel): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)catalog.size()) return;

    Animal chosen = catalog[idx - 1];
    int id = (int)p.getAnimals().size() + 1;
    Animal fresh = Player::createAnimalByType(chosen.getType(), id);

    if (p.buyAnimal(fresh))
        cout << "Bought: " << fresh.getType() << " \"" << fresh.getName() << "\"\n";
    else
        cout << "Not enough money or actions!\n";
}

// ==================== SAVE HELPERS (GAME) ====================
void trySaveManually(const Player& p)
{
    long long wait = secondsUntilSaveAllowed();
    if (wait > 0)
    {
        cout << "Save is on cooldown. Wait "
            << (wait / 60) << " min " << (wait % 60) << " sec.\n";
        return;
    }
    if (saveGame(p)) cout << "Game saved.\n";
    else             cout << "Save failed!\n";
}

void maybeAutoSave(const Player& p)
{
    if (secondsUntilSaveAllowed() > 0) return;
    if (saveGame(p)) cout << "[autosave] Game saved.\n";
}

// ==================== GAME LOOP ====================
void runGame(Player& player)
{
    while (true)
    {
        printGameState(player);

        if (player.getActionsLeft() <= 0)
        {
            cout << "\nNo actions left for today.\n";
            cout << "1. End day\n";
            cout << "2. Save game\n";
            cout << "3. Return to main menu\n";
            cout << "Choice: ";
            int c; cin >> c;
            if (cin.fail()) { clearInput(); continue; }

            if (c == 1)
            {
                player.endDay();
                player.resetActions(actionsPerDay());
                cout << "--- Day " << player.getDay() << " started ---\n";
                maybeAutoSave(player);
            }
            else if (c == 2) trySaveManually(player);
            else if (c == 3) return;
            continue;
        }

        cout << "\n--- Day menu ---\n";
        cout << ACTION_FEED << ". Feed an animal\n";
        cout << ACTION_CARE << ". Care for an animal\n";
        cout << ACTION_BUY_FOOD << ". Buy food\n";
        cout << ACTION_BUY_ANIMAL << ". Buy an animal\n";
        cout << ACTION_END_DAY << ". End day\n";
        cout << ACTION_SAVE_GAME << ". Save game\n";
        cout << "Choose an action: ";

        int choice;
        cin >> choice;
        if (cin.fail()) { clearInput(); continue; }

        switch (choice)
        {
        case ACTION_FEED:       actionFeed(player);      break;
        case ACTION_CARE:       actionCare(player);      break;
        case ACTION_BUY_FOOD:   actionBuyFood(player);   break;
        case ACTION_BUY_ANIMAL: actionBuyAnimal(player); break;
        case ACTION_END_DAY:
            player.endDay();
            player.resetActions(actionsPerDay());
            cout << "--- Day " << player.getDay() << " started ---\n";
            break;
        case ACTION_SAVE_GAME:  trySaveManually(player); break;
        default:
            cout << "Invalid option!\n";
        }

        // јвтосохранение Ч раз в 30 минут реального времени
        maybeAutoSave(player);
    }
}

void startGame()
{
    Player player;
    player.initFromSettings(startingAnimals, startMoney(), actionsPerDay());

    cout << "\n=== GAME START ===\n";
    cout << "Difficulty: " << difficulty << "\n";
    cout << "Starting money: " << player.getMoney() << "\n";
    cout << "Actions per day: " << player.getActionsLeft() << "\n";

    runGame(player);
}

// ==================== SETTINGS SUBMENU ====================
void difficultySubmenu()
{
    const string options[DIFF_COUNT] = { "Easy", "Medium", "Hard" };
    while (true)
    {
        cout << "\n===== DIFFICULTY LEVEL =====\n";
        for (int i = 0; i < DIFF_COUNT; i++)
            cout << (i + 1) << ". " << options[i] << "\n";
        cout << (DIFF_COUNT + 1) << ". Back\n";
        cout << "Current: " << difficulty << "\n";
        cout << "Choose: ";

        int c; cin >> c;
        if (cin.fail()) { clearInput(); continue; }

        if (c >= 1 && c <= DIFF_COUNT)
        {
            difficulty = options[c - 1];
            saveSettings();
            cout << "Set: " << difficulty << "\n";
        }
        else if (c == DIFF_COUNT + 1) return;
    }
}

void animalsSubmenu()
{
    while (true)
    {
        cout << "\n===== ANIMALS (starting) =====\n";
        int i = 1;
        for (const auto& p : startingAnimals)
            cout << i++ << ". " << p.first << " Ч " << p.second << "\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int c; cin >> c;
        if (cin.fail()) { clearInput(); continue; }
        if (c == 0) return;
        if (c < 1 || c >(int)startingAnimals.size()) continue;

        auto it = startingAnimals.begin();
        advance(it, c - 1);
        string name = it->first;

        while (true)
        {
            cout << "\n--- " << name << " (" << startingAnimals[name] << ") ---\n";
            cout << ANIMAL_EDIT << ". Change quantity\n";
            cout << ANIMAL_DELETE << ". Delete\n";
            cout << ANIMAL_BACK << ". Back\n";
            cout << "Choice: ";

            int a; cin >> a;
            if (cin.fail()) { clearInput(); continue; }

            if (a == ANIMAL_EDIT)
            {
                cout << "New quantity (0..1000): ";
                int v; cin >> v;
                if (cin.fail()) { clearInput(); continue; }
                if (v < 0 || v > 1000) continue;
                startingAnimals[name] = v;
                saveSettings();
            }
            else if (a == ANIMAL_DELETE)
            {
                startingAnimals.erase(name);
                saveSettings();
                break;
            }
            else if (a == ANIMAL_BACK) break;
        }
    }
}

void addAnimalSubmenu()
{
    cout << "Animal name: ";
    string name; cin >> name;
    if (name.empty() || startingAnimals.count(name)) return;

    cout << "Quantity: ";
    int c; cin >> c;
    if (cin.fail()) { clearInput(); return; }
    if (c < 0 || c > 1000) return;

    startingAnimals[name] = c;
    saveSettings();
}

void settingsMenu()
{
    while (true)
    {
        cout << "\n===== SETTINGS =====\n";
        cout << SET_DIFFICULTY << ". Difficulty (" << difficulty << ")\n";
        cout << SET_ANIMALS << ". Animals (" << startingAnimals.size() << ")\n";
        cout << SET_ADD_ANIMAL << ". Add animal\n";
        cout << SET_BACK << ". Back\n";
        cout << "Choose: ";

        int c; cin >> c;
        if (cin.fail()) { clearInput(); continue; }

        switch (c)
        {
        case SET_DIFFICULTY: difficultySubmenu(); break;
        case SET_ANIMALS:    animalsSubmenu();    break;
        case SET_ADD_ANIMAL: addAnimalSubmenu();  break;
        case SET_BACK:       return;
        }
    }
}

// ==================== OTHER ITEMS ====================
void loadGame()
{
    cout << "\n=== LOAD GAME ===\n";
    Player player;
    if (!loadGameFromFile(player))
    {
        cout << "No save file found or it is corrupted.\n";
        cout << "Enter to return...";
        clearInput(); cin.get();
        return;
    }
    cout << "Save loaded. Starting game...\n";
    runGame(player);
}

void aboutProgram()
{
    cout << "\n=== ABOUT PROGRAM ===\n";
    cout << "Name: Happy Farm\n";
    cout << "Version: " << PROGRAM_VERSION << "\n";
    cout << "Student: " << STUDENT_NAME << "\n";
    cout << "Enter to return...";
    clearInput(); cin.get();
}

void showMainMenu()
{
    cout << "\n******************************\n";
    cout << "*        HAPPY FARM          *\n";
    cout << "******************************\n";
    cout << MENU_START_GAME << ". Start game\n";
    cout << MENU_LOAD_GAME << ". Load game\n";
    cout << MENU_SETTINGS << ". Settings\n";
    cout << MENU_ABOUT << ". About program\n";
    cout << MENU_EXIT << ". Exit\n";
    cout << "Choose a menu item: ";
}

// ==================== MAIN ====================
int main()
{
    setupLocale();
    initSettings();

    while (true)
    {
        showMainMenu();

        int c;
        cin >> c;
        if (cin.fail()) { clearInput(); cout << "Input error!\n"; continue; }

        switch (c)
        {
        case MENU_START_GAME: startGame();    break;
        case MENU_LOAD_GAME:  loadGame();     break;
        case MENU_SETTINGS:   settingsMenu(); break;
        case MENU_ABOUT:      aboutProgram(); break;
        case MENU_EXIT:
            cout << "Goodbye!\n";
            return 0;
        default:
            cout << "Invalid option!\n";
        }
    }
}