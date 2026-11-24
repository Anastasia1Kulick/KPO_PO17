#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstdlib>

using namespace std;

const string STUDENT_NAME = "Александр Ковалев";
const string VERSION = "3.1";

const int START_GAME = 1;
const int LOAD_GAME = 2;
const int SETTINGS = 3;
const int ABOUT = 4;
const int EXIT = 5;

const int DIFFICULTY = 1;
const int ANIMALS = 2;
const int ADD_ANIMAL = 3;
const int BACK = 4;

const string DEFAULT_DIFFICULTY = "Нормально";
const int MIN_COUNT = 1;
const int MAX_COUNT = 100;
const string ANIMAL_PREFIX = "animal.";
const string CHICKEN_SPECIES = "Курица";
const string KITSUNE_SPECIES = "Кицунэ";

string settingsFile = "settings.txt";
string difficulty = DEFAULT_DIFFICULTY;
map<string, int> animals;

void setDefaults()
{
    difficulty = DEFAULT_DIFFICULTY;
    animals.clear();
    animals["Корова"] = 3;
    animals["Курица"] = 5;
}

bool saveSettings()
{
    ofstream out(settingsFile.c_str());
    if (!out.is_open())
    {
        cout << "Ошибка: не удалось записать файл настроек " << settingsFile << ".\n";
        return false;
    }

    out << "difficulty=" << difficulty << "\n";
    for (const auto& animal : animals)
        out << ANIMAL_PREFIX << animal.first << "=" << animal.second << "\n";
    return true;
}

bool isValidDifficulty(const string& s)
{
    return s == "Легко" || s == "Нормально" || s == "Сложно";
}

void loadSettings()
{
    setDefaults();

    ifstream in(settingsFile.c_str());
    if (!in.is_open())
    {
        cout << "Файл настроек не найден. Создан " << settingsFile
             << " с настройками по умолчанию.\n";
        saveSettings();
        return;
    }

    string loadedDifficulty;
    map<string, int> loadedAnimals;
    bool hasErrors = false;
    string line;

    while (getline(in, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;

        size_t pos = line.find('=');
        if (pos == string::npos)
        {
            cout << "Предупреждение: пропущена неверная строка в файле настроек: " << line << "\n";
            hasErrors = true;
            continue;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        if (key == "difficulty")
        {
            loadedDifficulty = value;
        }
        else if (key.size() > ANIMAL_PREFIX.size()
                 && key.compare(0, ANIMAL_PREFIX.size(), ANIMAL_PREFIX) == 0)
        {
            try
            {
                size_t used = 0;
                int count = stoi(value, &used);
                if (used != value.size() || count < MIN_COUNT || count > MAX_COUNT)
                    throw out_of_range("count");
                loadedAnimals[key.substr(ANIMAL_PREFIX.size())] = count;
            }
            catch (const exception&)
            {
                cout << "Предупреждение: пропущена неверная строка в файле настроек: " << line << "\n";
                hasErrors = true;
            }
        }
        else
        {
            cout << "Предупреждение: пропущена неизвестная строка в файле настроек: " << line << "\n";
            hasErrors = true;
        }
    }

    if (isValidDifficulty(loadedDifficulty))
    {
        difficulty = loadedDifficulty;
    }
    else
    {
        cout << "Предупреждение: уровень сложности в файле неверный, взят по умолчанию.\n";
        hasErrors = true;
    }

    if (!loadedAnimals.empty())
        animals = loadedAnimals;
    else
        hasErrors = true;

    if (hasErrors)
        saveSettings();
}

int readInt()
{
    string line;
    if (!getline(cin, line))
        exit(0);

    stringstream ss(line);
    int value;
    char extra;
    if (!(ss >> value) || (ss >> extra))
        return -1;
    return value;
}

string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void waitForInput()
{
    cout << "\nНажмите Enter, чтобы вернуться в главное меню...";
    string line;
    if (!getline(cin, line))
        exit(0);
}

string farmArt()
{
    const char* art = R"ART(
    \ | /                                           .--.
   -- O --                 .--.                    (    ).
    / | \               .-(    ).        v        (___.__)
                v   v   (___.__)__)           v  v
                                                                v
                                /\
                              /    \         .-"-.
                            /   []   \      /     \
           ,@@@,           /__________\     |-----|         ,@@,
          @@@@@@@          |  |\  /|  |     |     |        @@@@@@
          `@@@@@'          |  | \/ |  |     |-----|        `@@@@'
             ||            |  | /\ |  |     |     |          ||
             ||            |__|/__\|__|     |-----|          ||
|-|-|-|-|-|-|-|-|-|-|-|-|-|-||      |-|-|-|-|_____|-|-|-|-|-|-|-|-|-|-
|-|-|-|-|-|-|-|-|-|-|-|-|-|-||      |-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-
"',."'"',."'"',."'"',."'"',."'::::::"',."'"',."'"',."'"',."'"',."'"',.
  *   @    *    @   *     @   ::::::     *   @   *     @   *     @  *
)ART";
    return string(art + 1);
}

string cowArt()
{
    const char* art = R"ART(
   ^__^
   (oo)\_________
   (__)\  @   @  )\/\
       ||-------w |
       ||       ||
)ART";
    return string(art + 1);
}

string chickenArt()
{
    const char* art = R"ART(
  \\       ,~.
   \\     (o >
    ) \___/  |
   (        /
    \______/
      || ||
)ART";
    return string(art + 1);
}

string kitsuneArt()
{
    const char* art = R"ART(
      /\_/\      ,-"-.  ,-"-.  ,-"-.
     ( o.o )----(  ~  )(  ~  )(  ~  )
      > ^ <     `-._.'  `-._.'  `-._.'
     /|   |\
)ART";
    return string(art + 1);
}

string animalArt(const string& species)
{
    if (species == "Корова")
        return cowArt();
    if (species == "Курица")
        return chickenArt();
    if (species == KITSUNE_SPECIES)
        return kitsuneArt();
    return "";
}

vector<string> splitLines(const string& text)
{
    vector<string> lines;
    stringstream ss(text);
    string line;
    while (getline(ss, line))
        lines.push_back(line);
    return lines;
}

string sideBySide(const string& left, const string& right)
{
    vector<string> a = splitLines(left);
    vector<string> b = splitLines(right);

    size_t width = 0;
    for (const string& line : a)
        if (line.size() > width)
            width = line.size();

    size_t count = a.size() > b.size() ? a.size() : b.size();
    string result;
    for (size_t i = 0; i < count; i++)
    {
        size_t shiftA = count - a.size();
        size_t shiftB = count - b.size();
        string l = i >= shiftA ? a[i - shiftA] : "";
        string r = i >= shiftB ? b[i - shiftB] : "";
        l.resize(width + 4, ' ');
        string line = l + r;
        while (!line.empty() && line[line.size() - 1] == ' ')
            line.erase(line.size() - 1);
        result += line + "\n";
    }
    return result;
}

void mainMenu()
{
    cout << "\n";
    cout << "=============================\n";
    cout << "        ВЕСЕЛАЯ ФЕРМА\n";
    cout << "=============================\n";
    cout << "1. Начать игру\n";
    cout << "2. Загрузить игру\n";
    cout << "3. Настройки\n";
    cout << "4. О программе\n";
    cout << "5. Выход\n";
    cout << "=============================\n";
}

void showWelcome()
{
    cout << "\n" << farmArt();
    cout << "\n";
    cout << "        ДОБРО ПОЖАЛОВАТЬ НА ФЕРМУ!\n";
}

const int MAX_STAT = 100;
const int START_HUNGER = 20;
const int START_MOOD = 70;
const int FEED_AMOUNT = 50;
const int CARE_AMOUNT = 30;
const int MOOD_LOSS_PER_DAY = 10;
const int INCOME_MAX_HUNGER = 50;
const int INCOME_MIN_MOOD = 50;
const int FOOD_PACK = 5;
const int START_FOOD = 2;
const int ANIMAL_SCORE = 20;

const int ACTION_FEED = 1;
const int ACTION_CARE = 2;
const int ACTION_BUY_FOOD = 3;
const int ACTION_BUY_ANIMAL = 4;
const int ACTION_END_DAY = 5;
const int ACTION_RENAME = 6;

class Food
{
public:
    Food(const string& name, int price) : name(name), price(price) {}

    string getName() const { return name; }
    int getPrice() const { return price; }

private:
    string name;
    int price;
};

const vector<Food>& foodList()
{
    static const vector<Food> foods = {
        Food("Сено", 5),
        Food("Зерно", 4),
        Food("Овощи", 6),
        Food("Трава", 3),
        Food("Тофу", 8)
    };
    return foods;
}

string foodNameForSpecies(const string& species)
{
    static const map<string, string> table = {
        {"Корова", "Сено"},
        {"Курица", "Зерно"},
        {"Свинья", "Овощи"},
        {"Овца", "Трава"},
        {"Кицунэ", "Тофу"}
    };

    map<string, string>::const_iterator it = table.find(species);
    if (it != table.end())
        return it->second;

    unsigned int sum = 0;
    for (unsigned char c : species)
        sum += c;
    return foodList()[sum % foodList().size()].getName();
}

int priceForSpecies(const string& species)
{
    static const map<string, int> prices = {
        {"Корова", 60}, {"Курица", 20}, {"Свинья", 40}, {"Овца", 35}, {"Кицунэ", 80}
    };
    map<string, int>::const_iterator it = prices.find(species);
    return it != prices.end() ? it->second : 30;
}

int incomeForSpecies(const string& species)
{
    static const map<string, int> incomes = {
        {"Корова", 12}, {"Курица", 5}, {"Свинья", 8}, {"Овца", 7}, {"Кицунэ", 15}
    };
    map<string, int>::const_iterator it = incomes.find(species);
    return it != incomes.end() ? it->second : 6;
}

class Animal
{
public:
    Animal(const string& species, const string& name)
        : species(species), name(name), foodName(foodNameForSpecies(species)),
          hunger(START_HUNGER), mood(START_MOOD) {}

    string getSpecies() const { return species; }
    string getName() const { return name; }
    void setName(const string& newName) { name = newName; }
    string getFoodName() const { return foodName; }
    int getHunger() const { return hunger; }
    int getMood() const { return mood; }

    void feed()
    {
        hunger -= FEED_AMOUNT;
        if (hunger < 0)
            hunger = 0;
    }

    void care()
    {
        mood += CARE_AMOUNT;
        if (mood > MAX_STAT)
            mood = MAX_STAT;
    }

    void passDay(int hungerPerDay)
    {
        hunger += hungerPerDay;
        if (hunger > MAX_STAT)
            hunger = MAX_STAT;
        mood -= MOOD_LOSS_PER_DAY;
        if (mood < 0)
            mood = 0;
    }

    bool isAlive() const { return hunger < MAX_STAT; }

    int getDailyIncome() const
    {
        if (hunger < INCOME_MAX_HUNGER && mood >= INCOME_MIN_MOOD)
            return incomeForSpecies(species);
        return 0;
    }

private:
    string species;
    string name;
    string foodName;
    int hunger;
    int mood;
};

struct DayReport
{
    int income;
    vector<string> diedNames;
};

class Player
{
public:
    Player(int money, int actionsLeft) : money(money), actionsLeft(actionsLeft) {}

    int getMoney() const { return money; }
    int getActionsLeft() const { return actionsLeft; }
    const vector<Animal>& getHerd() const { return herd; }

    int getFood(const string& foodName) const
    {
        map<string, int>::const_iterator it = foodStock.find(foodName);
        return it != foodStock.end() ? it->second : 0;
    }

    int getScore() const { return money + (int)herd.size() * ANIMAL_SCORE; }

    void startDay(int actions) { actionsLeft = actions; }

    void addFood(const string& foodName, int portions) { foodStock[foodName] += portions; }

    void addAnimal(const string& species)
    {
        counters[species]++;
        herd.push_back(Animal(species, species + " " + to_string(counters[species])));
    }

    bool renameAnimal(int index, const string& proposedName)
    {
        if (index < 0 || index >= (int)herd.size())
            return false;
        string newName = trim(proposedName);
        if (newName.empty() || newName.size() > 60 || newName.find('\n') != string::npos)
            return false;
        for (size_t i = 0; i < herd.size(); ++i)
            if ((int)i != index && herd[i].getName() == newName)
                return false;
        herd[index].setName(newName);
        return true;
    }

    bool feedAnimal(int index)
    {
        if (actionsLeft <= 0 || index < 0 || index >= (int)herd.size())
            return false;
        string foodName = herd[index].getFoodName();
        if (getFood(foodName) <= 0)
            return false;
        foodStock[foodName]--;
        herd[index].feed();
        actionsLeft--;
        return true;
    }

    bool careAnimal(int index)
    {
        if (actionsLeft <= 0 || index < 0 || index >= (int)herd.size())
            return false;
        herd[index].care();
        actionsLeft--;
        return true;
    }

    bool buyFood(const Food& food)
    {
        int cost = food.getPrice() * FOOD_PACK;
        if (actionsLeft <= 0 || money < cost)
            return false;
        money -= cost;
        addFood(food.getName(), FOOD_PACK);
        actionsLeft--;
        return true;
    }

    bool buyAnimal(const string& species)
    {
        int price = priceForSpecies(species);
        if (actionsLeft <= 0 || money < price)
            return false;
        money -= price;
        addAnimal(species);
        actionsLeft--;
        return true;
    }

    vector<string> kitsuneEats()
    {
        int kitsunes = 0;
        for (const Animal& animal : herd)
            if (animal.getSpecies() == KITSUNE_SPECIES)
                kitsunes++;

        vector<string> eaten;
        vector<Animal> rest;
        for (const Animal& animal : herd)
        {
            if (animal.getSpecies() == CHICKEN_SPECIES && (int)eaten.size() < kitsunes)
                eaten.push_back(animal.getName());
            else
                rest.push_back(animal);
        }

        int toFeed = (int)eaten.size();
        for (Animal& animal : rest)
        {
            if (toFeed > 0 && animal.getSpecies() == KITSUNE_SPECIES)
            {
                animal.feed();
                toFeed--;
            }
        }
        herd = rest;
        return eaten;
    }

    DayReport endDay(int hungerPerDay)
    {
        DayReport report;
        report.income = 0;
        for (const Animal& animal : herd)
            report.income += animal.getDailyIncome();
        money += report.income;

        vector<Animal> alive;
        for (Animal animal : herd)
        {
            animal.passDay(hungerPerDay);
            if (animal.isAlive())
                alive.push_back(animal);
            else
                report.diedNames.push_back(animal.getName());
        }
        herd = alive;
        actionsLeft = 0;
        return report;
    }

private:
    int money;
    int actionsLeft;
    vector<Animal> herd;
    map<string, int> foodStock;
    map<string, int> counters;
};

struct GameRules
{
    int startMoney;
    int actionsPerDay;
    int hungerPerDay;
    int days;
};

GameRules getRules(const string& level)
{
    GameRules rules;
    if (level == "Легко")
        rules = {250, 8, 15, 7};
    else if (level == "Сложно")
        rules = {120, 5, 25, 7};
    else
        rules = {180, 6, 20, 7};
    return rules;
}

void showStatus(const Player& player, int day, const GameRules& rules)
{
    cout << "\n";
    cout << "=============================\n";
    cout << "День " << day << " из " << rules.days
         << ". Действий осталось: " << player.getActionsLeft() << "\n";
    cout << "Деньги: " << player.getMoney() << "\n";
    cout << "Корм:";
    for (const Food& food : foodList())
        cout << " " << food.getName() << " " << player.getFood(food.getName()) << ";";
    cout << "\n";
    cout << "Животные:\n";
    for (const Animal& animal : player.getHerd())
        cout << "  " << animal.getName() << " (голод: " << animal.getHunger()
             << ", настроение: " << animal.getMood() << ")\n";
    cout << "=============================\n";
}

int chooseAnimal(const Player& player)
{
    cout << "\n";
    int number = 1;
    for (const Animal& animal : player.getHerd())
    {
        cout << number << ". " << animal.getName() << " (голод: " << animal.getHunger()
             << ", настроение: " << animal.getMood() << ", корм: " << animal.getFoodName() << ")\n";
        number++;
    }
    cout << "Выберите животное: ";
    int choice = readInt();
    if (choice < 1 || choice > (int)player.getHerd().size())
    {
        cout << "Ошибка: такого животного нет.\n";
        return -1;
    }
    return choice - 1;
}

void feedAction(Player& player)
{
    int index = chooseAnimal(player);
    if (index < 0)
        return;

    string name = player.getHerd()[index].getName();
    string species = player.getHerd()[index].getSpecies();
    string foodName = player.getHerd()[index].getFoodName();
    if (!player.feedAnimal(index))
    {
        cout << "Нет корма \"" << foodName << "\". Сначала купите его.\n";
        return;
    }
    cout << "\n" << animalArt(species);
    cout << name << " накормлено.\n";
}

void careAction(Player& player)
{
    int index = chooseAnimal(player);
    if (index < 0)
        return;

    string name = player.getHerd()[index].getName();
    string species = player.getHerd()[index].getSpecies();
    player.careAnimal(index);
    cout << "\n" << animalArt(species);
    cout << "Вы позаботились о животном: " << name << ".\n";
}

void renameAnimalAction(Player& player)
{
    int index = chooseAnimal(player);
    if (index < 0)
        return;
    cout << "Введите новое имя (не более 60 байт): ";
    string newName;
    if (!getline(cin, newName))
        exit(0);
    if (!player.renameAnimal(index, newName))
    {
        cout << "Ошибка: имя пустое, слишком длинное или уже занято.\n";
        return;
    }
    cout << "Новое имя животного: " << player.getHerd()[index].getName() << ".\n";
}

void buyFoodAction(Player& player)
{
    cout << "\n";
    const vector<Food>& foods = foodList();
    for (size_t i = 0; i < foods.size(); i++)
        cout << i + 1 << ". " << foods[i].getName() << " (" << FOOD_PACK << " порций за "
             << foods[i].getPrice() * FOOD_PACK << ")\n";
    cout << "Выберите корм: ";
    int choice = readInt();

    if (choice < 1 || choice > (int)foods.size())
    {
        cout << "Ошибка: такого корма нет.\n";
        return;
    }
    if (!player.buyFood(foods[choice - 1]))
    {
        cout << "Недостаточно денег.\n";
        return;
    }
    cout << "Куплено: " << foods[choice - 1].getName() << ", " << FOOD_PACK << " порций.\n";
}

void buyAnimalAction(Player& player)
{
    cout << "\n";
    vector<string> species;
    for (const auto& entry : animals)
    {
        species.push_back(entry.first);
        cout << species.size() << ". " << entry.first << " (цена: "
             << priceForSpecies(entry.first) << ")\n";
    }
    cout << "Выберите вид животного: ";
    int choice = readInt();

    if (choice < 1 || choice > (int)species.size())
    {
        cout << "Ошибка: такого вида нет.\n";
        return;
    }
    if (!player.buyAnimal(species[choice - 1]))
    {
        cout << "Недостаточно денег.\n";
        return;
    }
    cout << "Куплено новое животное: " << player.getHerd().back().getName() << ".\n";
}

void playDay(Player& player, const GameRules& rules, int day)
{
    player.startDay(rules.actionsPerDay);
    bool finished = false;

    while (player.getActionsLeft() > 0 && !finished)
    {
        showStatus(player, day, rules);
        cout << "1. Накормить животное\n";
        cout << "2. Позаботиться о животном\n";
        cout << "3. Купить еду\n";
        cout << "4. Купить новое животное\n";
        cout << "5. Закончить день\n";
        cout << "6. Дать имя / переименовать животное (без затрат действия)\n";
        cout << "Выберите действие: ";
        int choice = readInt();

        switch (choice)
        {
            case ACTION_FEED: feedAction(player); break;
            case ACTION_CARE: careAction(player); break;
            case ACTION_BUY_FOOD: buyFoodAction(player); break;
            case ACTION_BUY_ANIMAL: buyAnimalAction(player); break;
            case ACTION_END_DAY: finished = true; break;
            case ACTION_RENAME: renameAnimalAction(player); break;
            default: cout << "Ошибка: такого действия нет.\n";
        }
    }

    vector<string> eaten = player.kitsuneEats();
    DayReport report = player.endDay(rules.hungerPerDay);
    cout << "\nДень " << day << " закончился.\n";
    for (const string& name : eaten)
        cout << "Кицунэ съела курицу: " << name << ".\n";
    cout << "Доход за день: " << report.income << "\n";
    for (const string& name : report.diedNames)
        cout << "Животное погибло от голода: " << name << "\n";
}

void startGame()
{
    GameRules rules = getRules(difficulty);
    Player player(rules.startMoney, rules.actionsPerDay);

    for (const auto& entry : animals)
        for (int i = 0; i < entry.second; i++)
            player.addAnimal(entry.first);
    for (const Food& food : foodList())
        player.addFood(food.getName(), START_FOOD);

    cout << "\n";
    cout << "Игра началась!\n";
    cout << "\n" << sideBySide(cowArt(), chickenArt()) << "\n";
    cout << "Уровень сложности: " << difficulty << "\n";
    cout << "Животные из настроек:\n";
    for (const auto& entry : animals)
        cout << "  " << entry.first << ": " << entry.second << "\n";
    cout << "Дней: " << rules.days << ", действий в день: " << rules.actionsPerDay
         << ", стартовые деньги: " << rules.startMoney << "\n";

    int day = 1;
    while (day <= rules.days && !player.getHerd().empty())
    {
        playDay(player, rules, day);
        day++;
    }

    cout << "\n";
    cout << "===== ИТОГИ ИГРЫ =====\n";
    if (player.getHerd().empty())
        cout << "Все животные погибли. Игра окончена.\n";
    cout << "Деньги: " << player.getMoney() << "\n";
    cout << "Животных на ферме: " << player.getHerd().size() << "\n";
    cout << "Итоговый счёт: " << player.getScore() << "\n";
    waitForInput();
}

void loadGame()
{
    cout << "\n";
    cout << "Данная функция пока недоступна.\n";
    cout << "Она появится в ближайших обновлениях.\n";
    waitForInput();
}

void changeDifficulty()
{
    cout << "\n";
    cout << "===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
    cout << "1. Легко\n";
    cout << "2. Нормально\n";
    cout << "3. Сложно\n";
    cout << "Выберите уровень: ";
    int choice = readInt();

    switch (choice)
    {
        case 1: difficulty = "Легко"; break;
        case 2: difficulty = "Нормально"; break;
        case 3: difficulty = "Сложно"; break;
        default:
            cout << "Ошибка: такого варианта нет.\n";
            return;
    }
    saveSettings();
    cout << "Уровень сложности изменён.\n";
}

void changeAnimalCount(const string& name)
{
    cout << "\n";
    cout << "===== " << name << " =====\n";
    cout << "Введите количество (от " << MIN_COUNT << " до " << MAX_COUNT << "): ";
    int count = readInt();

    if (count < MIN_COUNT || count > MAX_COUNT)
    {
        cout << "Ошибка: количество должно быть от " << MIN_COUNT << " до " << MAX_COUNT << ".\n";
        return;
    }
    animals[name] = count;
    saveSettings();
    cout << "Количество изменено.\n";
}

void animalsMenu()
{
    vector<string> names;
    cout << "\n";
    cout << "===== ЖИВОТНЫЕ =====\n";
    for (const auto& animal : animals)
    {
        names.push_back(animal.first);
        cout << names.size() << ". " << animal.first << " (сейчас: " << animal.second << ")\n";
    }
    cout << "Выберите животное: ";
    int choice = readInt();

    if (choice < 1 || choice > (int)names.size())
    {
        cout << "Ошибка: такого варианта нет.\n";
        return;
    }
    changeAnimalCount(names[choice - 1]);
}

void addAnimal()
{
    cout << "\n";
    cout << "===== НОВОЕ ЖИВОТНОЕ =====\n";
    cout << "Подсказка: Кицунэ съедает по одной курице в день.\n";
    cout << "Введите название вида: ";
    string name;
    if (!getline(cin, name))
        exit(0);
    name = trim(name);

    if (name.empty() || name.find('=') != string::npos)
    {
        cout << "Ошибка: неверное название.\n";
        return;
    }
    if (animals.count(name) > 0)
    {
        cout << "Ошибка: такой вид уже есть.\n";
        return;
    }

    cout << "Введите начальное количество (от " << MIN_COUNT << " до " << MAX_COUNT << "): ";
    int count = readInt();
    if (count < MIN_COUNT || count > MAX_COUNT)
    {
        cout << "Ошибка: количество должно быть от " << MIN_COUNT << " до " << MAX_COUNT << ".\n";
        return;
    }

    animals[name] = count;
    saveSettings();
    cout << "Вид добавлен и записан в файл настроек.\n";
}

void settingsMenu()
{
    int choice = 0;
    while (choice != BACK)
    {
        cout << "\n";
        cout << "=============================\n";
        cout << "          НАСТРОЙКИ\n";
        cout << "=============================\n";
        cout << "1. Уровень сложности\n";
        cout << "2. Количество животных\n";
        cout << "3. Добавить новый вид животного\n";
        cout << "4. Назад\n";
        cout << "=============================\n";
        cout << "Выберите пункт: ";
        choice = readInt();

        switch (choice)
        {
            case DIFFICULTY: changeDifficulty(); break;
            case ANIMALS: animalsMenu(); break;
            case ADD_ANIMAL: addAnimal(); break;
            case BACK: cout << "Возврат в главное меню.\n"; break;
            default: cout << "Ошибка: такого пункта нет.\n";
        }
    }
}

void about()
{
    cout << "\n";
    cout << "=============================\n";
    cout << "        О ПРОГРАММЕ\n";
    cout << "=============================\n";
    cout << "Название: Весёлая ферма\n";
    cout << "Версия: " << VERSION << "\n";
    cout << "Лабораторная работа №3\n";
    cout << "Автор: " << STUDENT_NAME << "\n";
    waitForInput();
}

int main()
{
    loadSettings();
    showWelcome();

    int choice = 0;
    while (choice != EXIT)
    {
        mainMenu();
        cout << "Выберите пункт: ";
        choice = readInt();

        switch (choice)
        {
            case START_GAME: startGame(); break;
            case LOAD_GAME: loadGame(); break;
            case SETTINGS: settingsMenu(); break;
            case ABOUT: about(); break;
            case EXIT: cout << "\nПрограмма завершена.\n"; break;
            default: cout << "\nОшибка: такого пункта нет.\n";
        }
    }
    return 0;
}
