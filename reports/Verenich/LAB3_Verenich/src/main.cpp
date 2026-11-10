#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <limits>
#include <algorithm>
#include <clocale>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

// ==================== ÊÎÍÑÒÀÍÒÛ ÏÎÄÌÅÍÞ ÍÀÑÒÐÎÅÊ ====================
const int SET_DIFFICULTY = 1;
const int SET_ANIMALS = 2;
const int SET_ADD_ANIMAL = 3;
const int SET_BACK = 4;

// ==================== ÊÎÍÑÒÀÍÒÛ ÏÎÄÌÅÍÞ ÆÈÂÎÒÍÛÕ ====================
const int ANIMAL_EDIT = 1;
const int ANIMAL_DELETE = 2;
const int ANIMAL_BACK = 3;

const int DIFF_COUNT = 3;

// ==================== ÊÎÍÑÒÀÍÒÛ ÈÃÐÎÂÎÃÎ ÏÐÎÖÅÑÑÀ ====================
const int ACTION_FEED = 1;
const int ACTION_CARE = 2;
const int ACTION_BUY_FOOD = 3;
const int ACTION_BUY_ANIMAL = 4;
const int ACTION_END_DAY = 5;

// ==================== ÂÅÐÑÈß ÏÐÎÃÐÀÌÌÛ ====================
const string PROGRAM_VERSION = "3.0";
const string STUDENT_NAME = "Viarenich Daniil";
const string SETTINGS_FILE = "settings.txt";

// ==================== ÃËÎÁÀËÜÍÛÅ ÍÀÑÒÐÎÉÊÈ ====================
string difficulty = "Medium";
map<string, int> startingAnimals;

// ==================== ÊËÀÑÑ FOOD ====================
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

// ==================== ÊËÀÑÑ ANIMAL ====================
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

    // --- Ãåòòåðû ---
    string getType()      const { return type; }
    string getName()      const { return name; }
    int    getHunger()    const { return hunger; }
    int    getHappiness() const { return happiness; }
    int    getHealth()    const { return health; }
    string getFoodType()  const { return foodType; }
    int    getBuyPrice()  const { return buyPrice; }
    int    getIncome()    const { return income; }

    bool isAlive() const { return health > 0; }

    // Êîðìëåíèå
    void feed(int nutrition)
    {
        hunger = max(0, hunger - nutrition);
        if (hunger < 20) happiness = min(100, happiness + 5);
    }

    // Çàáîòà
    void care()
    {
        happiness = min(100, happiness + 10);
        health = min(100, health + 5);
    }

    // Åæåäíåâíîå îáíîâëåíèå
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

    // Äîõîä çà äåíü
    int dailyIncome() const
    {
        if (health <= 0) return 0;
        return (income * happiness) / 100;
    }
};

// ==================== ÊËÀÑÑ PLAYER ====================
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

    // Èíèöèàëèçàöèÿ èç íàñòðîåê
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

    // Ñîçäàíèå æèâîòíîãî ïî òèïó
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

    // --- Äåéñòâèÿ ---

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
};

// ==================== ÂÑÏÎÌÎÃÀÒÅËÜÍÛÅ ÔÓÍÊÖÈÈ ====================
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

// ==================== ÔÀÉËÎÂÛÅ ÔÓÍÊÖÈÈ ====================
void setDefaults()
{
    difficulty = "Medium";
    startingAnimals.clear();
    startingAnimals["Cows"] = 2;
    startingAnimals["Chickens"] = 3;
}

void saveSettings()
{
    ofstream fout(SETTINGS_FILE);
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
        cout << "Ôàéë íàñòðîåê íå íàéäåí. Ñîçäàíû ïî óìîë÷àíèþ.\n";
        setDefaults();
        saveSettings();
    }
}

// ==================== ÈÃÐÎÂÎÉ ÏÐÎÖÅÑÑ ====================

void printGameState(const Player& p)
{
    cout << "\n+---------------------------------------+\n";
    cout << "| Äåíü: " << p.getDay()
        << " | Äåíüãè: " << p.getMoney()
        << " | Äåéñòâèé: " << p.getActionsLeft() << "\n";
    cout << "+---------------------------------------+\n";

    cout << "Æèâîòíûå (" << p.getAnimals().size() << "):\n";
    if (p.getAnimals().empty())
    {
        cout << "  (íåò æèâîòíûõ)\n";
    }
    else
    {
        int i = 1;
        for (const auto& a : p.getAnimals())
        {
            cout << "  " << i++ << ". " << a.getType()
                << " \"" << a.getName() << "\"  "
                << "ãîëîä:" << a.getHunger()
                << " ñ÷àñòüå:" << a.getHappiness()
                << " çäîðîâüå:" << a.getHealth() << "\n";
        }
    }

    cout << "Åäà: ";
    if (p.getFoodStock().empty()) cout << "(íåò)";
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
    if (p.getAnimals().empty()) { cout << "Íåò æèâîòíûõ.\n"; return; }

    cout << "Âûáåðèòå æèâîòíîå äëÿ êîðìëåíèÿ:\n";
    int i = 1;
    for (const auto& a : p.getAnimals())
    {
        cout << "  " << i++ << ". " << a.getType()
            << " \"" << a.getName() << "\" (åñò " << a.getFoodType()
            << ", ãîëîä " << a.getHunger() << ")\n";
    }
    cout << "Íîìåð (0  îòìåíà): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)p.getAnimals().size()) return;

    Animal& a = const_cast<Animal&>(p.getAnimals()[idx - 1]);
    string food = a.getFoodType();
    int stock = p.getFoodStock().count(food) ? p.getFoodStock().at(food) : 0;

    if (stock <= 0)
    {
        cout << "Íåò åäû \"" << food << "\" íà ñêëàäå!\n";
        return;
    }

    int nutrition = 20;
    for (const auto& f : foodCatalog())
        if (f.getName() == food) { nutrition = f.getNutrition(); break; }

    if (p.feedAnimal(idx - 1, nutrition))
        cout << "Æèâîòíîå ïîêîðìëåíî.\n";
    else
        cout << "Íå óäàëîñü ïîêîðìèòü.\n";
}

void actionCare(Player& p)
{
    if (p.getAnimals().empty()) { cout << "Íåò æèâîòíûõ.\n"; return; }

    cout << "Âûáåðèòå æèâîòíîå äëÿ çàáîòû:\n";
    int i = 1;
    for (const auto& a : p.getAnimals())
        cout << "  " << i++ << ". " << a.getType() << " \"" << a.getName() << "\"\n";
    cout << "Íîìåð (0  îòìåíà): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)p.getAnimals().size()) return;

    if (p.careAnimal(idx - 1))
        cout << "Æèâîòíîå îáëàñêàíî.\n";
    else
        cout << "Íåò äåéñòâèé!\n";
}

void actionBuyFood(Player& p)
{
    auto catalog = foodCatalog();
    cout << "Ìàãàçèí åäû:\n";
    int i = 1;
    for (const auto& f : catalog)
    {
        cout << "  " << i++ << ". " << f.getName()
            << "  öåíà " << f.getPrice()
            << ", ïèòàòåëüíîñòü " << f.getNutrition() << "\n";
    }
    cout << "Íîìåð (0  îòìåíà): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)catalog.size()) return;

    const Food& f = catalog[idx - 1];
    cout << "Ñêîëüêî êóïèòü? ";
    int amount; cin >> amount;
    if (cin.fail()) { clearInput(); return; }
    if (amount <= 0) return;

    if (p.buyFood(f.getName(), f.getPrice(), amount))
        cout << "Êóïëåíî " << amount << " x " << f.getName() << "\n";
    else
        cout << "Íåäîñòàòî÷íî äåíåã èëè äåéñòâèé!\n";
}

void actionBuyAnimal(Player& p)
{
    auto catalog = animalCatalog();
    cout << "Ìàãàçèí æèâîòíûõ:\n";
    int i = 1;
    for (const auto& a : catalog)
    {
        cout << "  " << i++ << ". " << a.getType()
            << "  öåíà " << a.getBuyPrice()
            << ", åñò " << a.getFoodType()
            << ", äîõîä " << a.getIncome() << "/äåíü\n";
    }
    cout << "Íîìåð (0  îòìåíà): ";
    int idx; cin >> idx;
    if (cin.fail()) { clearInput(); return; }
    if (idx < 1 || idx >(int)catalog.size()) return;

    Animal chosen = catalog[idx - 1];
    int id = (int)p.getAnimals().size() + 1;
    Animal fresh = Player::createAnimalByType(chosen.getType(), id);

    if (p.buyAnimal(fresh))
        cout << "Êóïëåíî: " << fresh.getType() << " \"" << fresh.getName() << "\"\n";
    else
        cout << "Íåäîñòàòî÷íî äåíåã èëè äåéñòâèé!\n";
}

// Ãëàâíûé öèêë èãðû
void startGame()
{
    Player player;
    player.initFromSettings(startingAnimals, startMoney(), actionsPerDay());

    cout << "\n=== ÍÀ×ÀËÎ ÈÃÐÛ ===\n";
    cout << "Ñëîæíîñòü: " << difficulty << "\n";
    cout << "Íà÷àëüíûå äåíüãè: " << player.getMoney() << "\n";
    cout << "Äåéñòâèé â äåíü: " << player.getActionsLeft() << "\n";

    while (true)
    {
        printGameState(player);

        if (player.getActionsLeft() <= 0)
        {
            cout << "\nÄåéñòâèÿ íà ñåãîäíÿ çàêîí÷èëèñü.\n";
            cout << "1. Çàâåðøèòü äåíü\n";
            cout << "2. Âûéòè â ãëàâíîå ìåíþ\n";
            cout << "Âûáîð: ";
            int c; cin >> c;
            if (cin.fail()) { clearInput(); continue; }

            if (c == 1)
            {
                player.endDay();
                player.resetActions(actionsPerDay());
                cout << "--- Äåíü " << player.getDay() << " íà÷àëñÿ ---\n";
            }
            else if (c == 2) return;
            continue;
        }

        cout << "\n--- Ìåíþ äíÿ ---\n";
        cout << ACTION_FEED << ". Ïîêîðìèòü æèâîòíîå\n";
        cout << ACTION_CARE << ". Ïîçàáîòèòüñÿ î æèâîòíîì\n";
        cout << ACTION_BUY_FOOD << ". Êóïèòü åäó\n";
        cout << ACTION_BUY_ANIMAL << ". Êóïèòü æèâîòíîå\n";
        cout << ACTION_END_DAY << ". Çàâåðøèòü äåíü\n";
        cout << "Âûáåðèòå äåéñòâèå: ";

        int choice;
        cin >> choice;
        if (cin.fail()) { clearInput(); continue; }

        switch (choice)
        {
        case ACTION_FEED:       actionFeed(player);       break;
        case ACTION_CARE:       actionCare(player);       break;
        case ACTION_BUY_FOOD:   actionBuyFood(player);    break;
        case ACTION_BUY_ANIMAL: actionBuyAnimal(player);  break;
        case ACTION_END_DAY:
            player.endDay();
            player.resetActions(actionsPerDay());
            cout << "--- Äåíü " << player.getDay() << " íà÷àëñÿ ---\n";
            break;
        default:
            cout << "Íåâåðíûé ïóíêò!\n";
        }
    }
}

// ==================== ÏÎÄÌÅÍÞ ÍÀÑÒÐÎÅÊ ====================
void difficultySubmenu()
{
    const string options[DIFF_COUNT] = { "Easy", "Medium", "Hard" };
    while (true)
    {
        cout << "\n===== ÓÐÎÂÅÍÜ ÑËÎÆÍÎÑÒÈ =====\n";
        for (int i = 0; i < DIFF_COUNT; i++)
            cout << (i + 1) << ". " << options[i] << "\n";
        cout << (DIFF_COUNT + 1) << ". Íàçàä\n";
        cout << "Òåêóùèé: " << difficulty << "\n";
        cout << "Âûáåðèòå: ";

        int c; cin >> c;
        if (cin.fail()) { clearInput(); continue; }

        if (c >= 1 && c <= DIFF_COUNT)
        {
            difficulty = options[c - 1];
            saveSettings();
            cout << "Óñòàíîâëåíî: " << difficulty << "\n";
        }
        else if (c == DIFF_COUNT + 1) return;
    }
}

void animalsSubmenu()
{
    while (true)
    {
        cout << "\n===== ÆÈÂÎÒÍÛÅ (ñòàðòîâûå) =====\n";
        int i = 1;
        for (const auto& p : startingAnimals)
            cout << i++ << ". " << p.first << "  " << p.second << "\n";
        cout << "0. Íàçàä\n";
        cout << "Âûáåðèòå: ";

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
            cout << ANIMAL_EDIT << ". Èçìåíèòü êîëè÷åñòâî\n";
            cout << ANIMAL_DELETE << ". Óäàëèòü\n";
            cout << ANIMAL_BACK << ". Íàçàä\n";
            cout << "Âûáîð: ";

            int a; cin >> a;
            if (cin.fail()) { clearInput(); continue; }

            if (a == ANIMAL_EDIT)
            {
                cout << "Íîâîå êîëè÷åñòâî (0..1000): ";
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
    cout << "Íàçâàíèå æèâîòíîãî: ";
    string name; cin >> name;
    if (name.empty() || startingAnimals.count(name)) return;

    cout << "Êîëè÷åñòâî: ";
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
        cout << "\n===== ÍÀÑÒÐÎÉÊÈ =====\n";
        cout << SET_DIFFICULTY << ". Ñëîæíîñòü (" << difficulty << ")\n";
        cout << SET_ANIMALS << ". Æèâîòíûå (" << startingAnimals.size() << ")\n";
        cout << SET_ADD_ANIMAL << ". Äîáàâèòü æèâîòíîå\n";
        cout << SET_BACK << ". Íàçàä\n";
        cout << "Âûáåðèòå: ";

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

// ==================== ÏÐÎ×ÈÅ ÏÓÍÊÒÛ ====================
void loadGame()
{
    cout << "\n=== ÇÀÃÐÓÇÊÀ ÈÃÐÛ ===\n";
    cout << "Ôóíêöèÿ ïîêà íåäîñòóïíà.\n";
    cout << "Enter äëÿ âîçâðàòà...";
    clearInput(); cin.get();
}

void aboutProgram()
{
    cout << "\n=== Î ÏÐÎÃÐÀÌÌÅ ===\n";
    cout << "Íàçâàíèå: Âåñåëàÿ ôåðìà\n";
    cout << "Âåðñèÿ: " << PROGRAM_VERSION << "\n";
    cout << "Ñòóäåíò: " << STUDENT_NAME << "\n";
    cout << "Enter äëÿ âîçâðàòà...";
    clearInput(); cin.get();
}

void showMainMenu()
{
    cout << "\n******************************\n";
    cout << "*        ÂÅÑÅËÀß ÔÅÐÌÀ       *\n";
    cout << "******************************\n";
    cout << MENU_START_GAME << ". Íà÷àòü èãðó\n";
    cout << MENU_LOAD_GAME << ". Çàãðóçèòü èãðó\n";
    cout << MENU_SETTINGS << ". Íàñòðîéêè\n";
    cout << MENU_ABOUT << ". Î ïðîãðàììå\n";
    cout << MENU_EXIT << ". Âûõîä\n";
    cout << "Âûáåðèòå ïóíêò ìåíþ: ";
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
        if (cin.fail()) { clearInput(); cout << "Îøèáêà ââîäà!\n"; continue; }

        switch (c)
        {
        case MENU_START_GAME: startGame();       break;
        case MENU_LOAD_GAME:  loadGame();        break;
        case MENU_SETTINGS:   settingsMenu();    break;
        case MENU_ABOUT:      aboutProgram();    break;
        case MENU_EXIT:
            cout << "Äî ñâèäàíèÿ!\n";
            return 0;
        default:
            cout << "Íåâåðíûé ïóíêò!\n";
        }
    }
}
