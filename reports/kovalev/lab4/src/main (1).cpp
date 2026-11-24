#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <set>

using namespace std;

const string STUDENT_NAME = "Александр Ковалев";
const string VERSION = "4.0";

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
string manualSaveFile = "save_manual.txt";
string autoSaveFile = "save_auto.txt";
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
const int ACTION_SAVE = 7;
const int ACTION_MAIN_MENU = 8;

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

    Animal(const string& species, const string& name, const string& foodName,
           int hunger, int mood)
        : species(species), name(name), foodName(foodName), hunger(hunger), mood(mood) {}

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

    Player(int money, int actionsLeft, const vector<Animal>& herd,
           const map<string, int>& foodStock, const map<string, int>& counters)
        : money(money), actionsLeft(actionsLeft), herd(herd),
          foodStock(foodStock), counters(counters) {}

    int getMoney() const { return money; }
    int getActionsLeft() const { return actionsLeft; }
    const vector<Animal>& getHerd() const { return herd; }
    const map<string, int>& getFoodStock() const { return foodStock; }
    const map<string, int>& getCounters() const { return counters; }

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
        string name;
        bool occupied;
        do
        {
            counters[species]++;
            name = species + " " + to_string(counters[species]);
            occupied = false;
            for (const Animal& animal : herd)
                if (animal.getName() == name)
                    occupied = true;
        } while (occupied);
        herd.push_back(Animal(species, name));
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

// Состояние одной партии не зависит от настроек следующей новой игры.
struct GameState
{
    int day;
    string level;
    GameRules rules;
    Player player;
    map<string, int> animalCatalog;

    GameState() : day(1), level(DEFAULT_DIFFICULTY),
                  rules(getRules(level)), player(0, 0) {}
};

bool isGameFinished(const GameState& state)
{
    return state.day > state.rules.days || state.player.getHerd().empty();
}

GameState createGameState()
{
    GameState state;
    state.level = difficulty;
    state.rules = getRules(state.level);
    state.animalCatalog = animals;
    state.player = Player(state.rules.startMoney, state.rules.actionsPerDay);
    for (const auto& entry : state.animalCatalog)
        for (int i = 0; i < entry.second; ++i)
            state.player.addAnimal(entry.first);
    for (const Food& food : foodList())
        state.player.addFood(food.getName(), START_FOOD);
    return state;
}

const int MAX_SAVE_RECORDS = 100000;
const int MAX_SAVE_VALUE = 1000000000;
const streamoff MAX_SAVE_BYTES = 16 * 1024 * 1024;

// Разделитель — |. Обратная косая черта защищает специальные символы.
string escapeSaveField(const string& value)
{
    const char* hex = "0123456789ABCDEF";
    string result;
    for (unsigned char c : value)
    {
        if (c == '\\') result += "\\\\";
        else if (c == '|') result += "\\|";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else if (c < 32 || c == 127)
        {
            result += "\\x";
            result += hex[c >> 4];
            result += hex[c & 15];
        }
        else result += static_cast<char>(c);
    }
    return result;
}

int hexDigit(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

vector<string> splitSaveFields(const string& line)
{
    vector<string> fields(1);
    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];
        if (c == '|') fields.push_back("");
        else if (c != '\\') fields.back() += c;
        else
        {
            if (++i == line.size()) throw runtime_error("незавершённое экранирование");
            char escaped = line[i];
            if (escaped == '\\' || escaped == '|') fields.back() += escaped;
            else if (escaped == 'n') fields.back() += '\n';
            else if (escaped == 'r') fields.back() += '\r';
            else if (escaped == 't') fields.back() += '\t';
            else if (escaped == 'x' && i + 2 < line.size())
            {
                int a = hexDigit(line[i + 1]);
                int b = hexDigit(line[i + 2]);
                if (a < 0 || b < 0) throw runtime_error("неверный код символа");
                fields.back() += static_cast<char>(a * 16 + b);
                i += 2;
            }
            else throw runtime_error("неизвестная escape-последовательность");
        }
    }
    return fields;
}

void writeSaveRecord(ostream& out, const vector<string>& fields)
{
    for (size_t i = 0; i < fields.size(); ++i)
    {
        if (i != 0) out << '|';
        out << escapeSaveField(fields[i]);
    }
    out << '\n';
}

int saveNumber(const string& text, int minimum, int maximum)
{
    if (text.empty()) throw runtime_error("пустое числовое поле");
    long long value = 0;
    for (char c : text)
    {
        if (c < '0' || c > '9') throw runtime_error("неверное числовое поле");
        value = value * 10 + (c - '0');
        if (value > maximum) throw runtime_error("число вне допустимого диапазона");
    }
    if (value < minimum) throw runtime_error("число вне допустимого диапазона");
    return static_cast<int>(value);
}

vector<string> readSaveRecord(istream& in, const string& key, size_t size)
{
    string line;
    if (!getline(in, line)) throw runtime_error("файл оборван: ожидается " + key);
    if (!line.empty() && line.back() == '\r') line.pop_back();
    vector<string> fields = splitSaveFields(line);
    if (fields.size() != size || fields[0] != key)
        throw runtime_error("неверная запись: ожидается " + key);
    return fields;
}

bool validSaveText(const string& text)
{
    return !trim(text).empty();
}

bool validateGameState(const GameState& state, string& error)
{
    error.clear();
    const GameRules expected = getRules(state.level);
    if (!isValidDifficulty(state.level) || state.rules.startMoney != expected.startMoney
        || state.rules.actionsPerDay != expected.actionsPerDay
        || state.rules.hungerPerDay != expected.hungerPerDay
        || state.rules.days != expected.days)
        error = "неверная сложность или правила игры";
    else if (state.day < 1 || state.day > state.rules.days + 1)
        error = "неверный номер дня";
    else if (state.player.getMoney() < 0 || state.player.getMoney() > MAX_SAVE_VALUE
             || state.player.getActionsLeft() < 0
             || state.player.getActionsLeft() > state.rules.actionsPerDay)
        error = "неверные деньги или количество действий";
    else if (isGameFinished(state) && state.player.getActionsLeft() != 0)
        error = "у завершённой игры не должно быть доступных действий";
    else if (state.animalCatalog.empty() || state.animalCatalog.size() > MAX_SAVE_RECORDS
             || state.player.getHerd().size() > MAX_SAVE_RECORDS)
        error = "неверный размер каталога или стада";
    if (!error.empty()) return false;

    const map<string, int>& stocks = state.player.getFoodStock();
    const map<string, int>& counters = state.player.getCounters();
    if (stocks.size() != foodList().size() || counters.size() != state.animalCatalog.size())
    {
        error = "неполные запасы корма или счётчики животных";
        return false;
    }
    for (const Food& food : foodList())
    {
        auto it = stocks.find(food.getName());
        if (it == stocks.end() || it->second < 0 || it->second > MAX_SAVE_VALUE)
        {
            error = "неверный запас корма";
            return false;
        }
    }
    for (const auto& entry : state.animalCatalog)
    {
        auto counter = counters.find(entry.first);
        if (!validSaveText(entry.first) || entry.second < MIN_COUNT || entry.second > MAX_COUNT
            || counter == counters.end() || counter->second < entry.second
            || counter->second > MAX_SAVE_VALUE)
        {
            error = "неверный каталог или счётчик вида";
            return false;
        }
    }
    set<string> names;
    map<string, int> aliveCounts;
    for (const Animal& animal : state.player.getHerd())
    {
        if (state.animalCatalog.count(animal.getSpecies()) == 0
            || !validSaveText(animal.getName())
            || !names.insert(animal.getName()).second
            || animal.getFoodName() != foodNameForSpecies(animal.getSpecies())
            || animal.getHunger() < 0 || animal.getHunger() >= MAX_STAT
            || animal.getMood() < 0 || animal.getMood() > MAX_STAT)
        {
            error = "неверные данные животного";
            return false;
        }
        ++aliveCounts[animal.getSpecies()];
    }
    for (const auto& entry : aliveCounts)
        if (entry.second > counters.at(entry.first))
        {
            error = "счётчик меньше количества животных";
            return false;
        }
    return true;
}

string currentSaveTime()
{
    time_t now = time(0);
    tm* value = localtime(&now);
    char text[32];
    if (!value || strftime(text, sizeof(text), "%Y-%m-%d %H:%M:%S", value) == 0)
        throw runtime_error("не удалось получить время сохранения");
    return text;
}

bool validSaveTime(const string& value)
{
    if (value.size() != 19) return false;
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (i == 4 || i == 7) { if (value[i] != '-') return false; }
        else if (i == 10) { if (value[i] != ' ') return false; }
        else if (i == 13 || i == 16) { if (value[i] != ':') return false; }
        else if (value[i] < '0' || value[i] > '9') return false;
    }
    try
    {
        int year = saveNumber(value.substr(0, 4), 1, 9999);
        int month = saveNumber(value.substr(5, 2), 1, 12);
        int day = saveNumber(value.substr(8, 2), 1, 31);
        const int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int maximumDay = monthDays[month - 1];
        if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
            ++maximumDay;
        if (day > maximumDay) return false;
        saveNumber(value.substr(11, 2), 0, 23);
        saveNumber(value.substr(14, 2), 0, 59);
        saveNumber(value.substr(17, 2), 0, 59);
    }
    catch (const exception&) { return false; }
    return true;
}

bool saveGame(const GameState& state, const string& path, string& error)
{
    if (!validateGameState(state, error)) return false;
    const string temporary = path + ".tmp";
    try
    {
        // Основной файл заменяется лишь после успешной записи временного.
        ofstream out(temporary.c_str(), ios::binary | ios::trunc);
        if (!out.is_open())
        {
            error = "не удалось открыть временный файл для записи";
            return false;
        }
        writeSaveRecord(out, {"FARM_SAVE", "1"});
        writeSaveRecord(out, {"saved_at", currentSaveTime()});
        writeSaveRecord(out, {"day", to_string(state.day)});
        writeSaveRecord(out, {"level", state.level});
        writeSaveRecord(out, {"rules", to_string(state.rules.startMoney),
            to_string(state.rules.actionsPerDay), to_string(state.rules.hungerPerDay),
            to_string(state.rules.days)});
        writeSaveRecord(out, {"player", to_string(state.player.getMoney()),
            to_string(state.player.getActionsLeft())});
        writeSaveRecord(out, {"catalog", to_string(state.animalCatalog.size())});
        for (const auto& entry : state.animalCatalog)
            writeSaveRecord(out, {"species", entry.first, to_string(entry.second)});
        writeSaveRecord(out, {"food", to_string(state.player.getFoodStock().size())});
        for (const auto& entry : state.player.getFoodStock())
            writeSaveRecord(out, {"stock", entry.first, to_string(entry.second)});
        writeSaveRecord(out, {"counters", to_string(state.player.getCounters().size())});
        for (const auto& entry : state.player.getCounters())
            writeSaveRecord(out, {"counter", entry.first, to_string(entry.second)});
        writeSaveRecord(out, {"herd", to_string(state.player.getHerd().size())});
        for (const Animal& animal : state.player.getHerd())
            writeSaveRecord(out, {"animal", animal.getSpecies(), animal.getName(),
                animal.getFoodName(), to_string(animal.getHunger()), to_string(animal.getMood())});
        writeSaveRecord(out, {"END"});
        if (out.tellp() > MAX_SAVE_BYTES)
            throw runtime_error("сохранение превышает допустимый размер файла");
        out.flush();
        bool written = static_cast<bool>(out);
        out.close();
        if (!written || out.fail()) throw runtime_error("ошибка записи сохранения");
        if (rename(temporary.c_str(), path.c_str()) != 0)
            throw runtime_error("не удалось заменить файл сохранения");
        error.clear();
        return true;
    }
    catch (const exception& ex)
    {
        remove(temporary.c_str());
        error = ex.what();
        return false;
    }
}

// Вспомогательная функция также возвращает время для меню выбора сохранения.
bool readSaveFile(const string& path, GameState& state, string& error, string* savedAt = 0)
{
    try
    {
        ifstream in(path.c_str(), ios::binary);
        if (!in.is_open()) throw runtime_error("файл не найден или недоступен для чтения");
        in.seekg(0, ios::end);
        streamoff bytes = in.tellg();
        if (bytes <= 0 || bytes > MAX_SAVE_BYTES) throw runtime_error("неверный размер файла");
        in.seekg(0, ios::beg);
        vector<string> fields = readSaveRecord(in, "FARM_SAVE", 2);
        if (fields[1] != "1") throw runtime_error("неподдерживаемая версия сохранения");
        string timestamp = readSaveRecord(in, "saved_at", 2)[1];
        if (!validSaveTime(timestamp)) throw runtime_error("неверное время сохранения");

        GameState loaded;
        loaded.day = saveNumber(readSaveRecord(in, "day", 2)[1], 1, 1000);
        loaded.level = readSaveRecord(in, "level", 2)[1];
        fields = readSaveRecord(in, "rules", 5);
        loaded.rules.startMoney = saveNumber(fields[1], 0, MAX_SAVE_VALUE);
        loaded.rules.actionsPerDay = saveNumber(fields[2], 1, 1000);
        loaded.rules.hungerPerDay = saveNumber(fields[3], 1, MAX_STAT);
        loaded.rules.days = saveNumber(fields[4], 1, 1000);
        fields = readSaveRecord(in, "player", 3);
        int money = saveNumber(fields[1], 0, MAX_SAVE_VALUE);
        int actions = saveNumber(fields[2], 0, 1000);

        int count = saveNumber(readSaveRecord(in, "catalog", 2)[1], 1, MAX_SAVE_RECORDS);
        for (int i = 0; i < count; ++i)
        {
            fields = readSaveRecord(in, "species", 3);
            int initial = saveNumber(fields[2], MIN_COUNT, MAX_COUNT);
            if (!loaded.animalCatalog.insert(make_pair(fields[1], initial)).second)
                throw runtime_error("повтор вида в каталоге");
        }
        map<string, int> stocks;
        count = saveNumber(readSaveRecord(in, "food", 2)[1], 0, MAX_SAVE_RECORDS);
        for (int i = 0; i < count; ++i)
        {
            fields = readSaveRecord(in, "stock", 3);
            int portions = saveNumber(fields[2], 0, MAX_SAVE_VALUE);
            if (!stocks.insert(make_pair(fields[1], portions)).second)
                throw runtime_error("повтор запаса корма");
        }
        map<string, int> counters;
        count = saveNumber(readSaveRecord(in, "counters", 2)[1], 1, MAX_SAVE_RECORDS);
        for (int i = 0; i < count; ++i)
        {
            fields = readSaveRecord(in, "counter", 3);
            int number = saveNumber(fields[2], 1, MAX_SAVE_VALUE);
            if (!counters.insert(make_pair(fields[1], number)).second)
                throw runtime_error("повтор счётчика животных");
        }
        vector<Animal> herd;
        count = saveNumber(readSaveRecord(in, "herd", 2)[1], 0, MAX_SAVE_RECORDS);
        for (int i = 0; i < count; ++i)
        {
            fields = readSaveRecord(in, "animal", 6);
            int hunger = saveNumber(fields[4], 0, MAX_STAT - 1);
            int mood = saveNumber(fields[5], 0, MAX_STAT);
            herd.push_back(Animal(fields[1], fields[2], fields[3], hunger, mood));
        }
        readSaveRecord(in, "END", 1);
        string extra;
        if (getline(in, extra)) throw runtime_error("лишние данные после конца сохранения");
        if (!in.eof()) throw runtime_error("ошибка чтения файла");
        loaded.player = Player(money, actions, herd, stocks, counters);
        if (!validateGameState(loaded, error)) return false;
        // При любой ошибке выше исходное состояние оставалось неизменным.
        state = loaded;
        if (savedAt) *savedAt = timestamp;
        error.clear();
        return true;
    }
    catch (const exception& ex)
    {
        error = ex.what();
        return false;
    }
}

bool loadGame(const string& path, GameState& state, string& error)
{
    return readSaveFile(path, state, error);
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

void buyAnimalAction(Player& player, const map<string, int>& catalog)
{
    cout << "\n";
    vector<string> species;
    for (const auto& entry : catalog)
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

void finishGameDay(GameState& state)
{
    if (isGameFinished(state)) return;
    vector<string> eaten = state.player.kitsuneEats();
    DayReport report = state.player.endDay(state.rules.hungerPerDay);
    cout << "\nДень " << state.day << " закончился.\n";
    for (const string& name : eaten)
        cout << "Кицунэ съела курицу: " << name << ".\n";
    cout << "Доход за день: " << report.income << "\n";
    for (const string& name : report.diedNames)
        cout << "Животное погибло от голода: " << name << "\n";

    ++state.day;
    if (!isGameFinished(state))
        state.player.startDay(state.rules.actionsPerDay);

    string error;
    if (saveGame(state, autoSaveFile, error))
        cout << "Автосохранение выполнено.\n";
    else
        cout << "Ошибка автосохранения: " << error << ".\n";
}

// false означает выход в главное меню без завершения текущего дня.
bool playDay(GameState& state)
{
    bool finished = false;
    while (state.player.getActionsLeft() > 0 && !finished)
    {
        showStatus(state.player, state.day, state.rules);
        cout << "1. Накормить животное\n";
        cout << "2. Позаботиться о животном\n";
        cout << "3. Купить еду\n";
        cout << "4. Купить новое животное\n";
        cout << "5. Закончить день\n";
        cout << "6. Дать имя / переименовать животное (без затрат действия)\n";
        cout << "7. Сохранить игру (без затрат действия)\n";
        cout << "8. В главное меню\n";
        cout << "Выберите действие: ";
        int choice = readInt();

        switch (choice)
        {
            case ACTION_FEED: feedAction(state.player); break;
            case ACTION_CARE: careAction(state.player); break;
            case ACTION_BUY_FOOD: buyFoodAction(state.player); break;
            case ACTION_BUY_ANIMAL: buyAnimalAction(state.player, state.animalCatalog); break;
            case ACTION_END_DAY: finished = true; break;
            case ACTION_RENAME: renameAnimalAction(state.player); break;
            case ACTION_SAVE:
            {
                string error;
                if (saveGame(state, manualSaveFile, error))
                    cout << "Ручное сохранение выполнено.\n";
                else
                    cout << "Ошибка сохранения: " << error << ".\n";
                break;
            }
            case ACTION_MAIN_MENU:
                cout << "Несохранённые изменения будут потеряны.\n"
                     << "1. Выйти в главное меню\n0. Остаться в игре\nВыберите пункт: ";
                if (readInt() == 1)
                {
                    cout << "Возврат в главное меню.\n";
                    return false;
                }
                break;
            default: cout << "Ошибка: такого действия нет.\n";
        }
    }

    finishGameDay(state);
    return true;
}

void continueGame(GameState& state)
{
    // Остаток действий уже содержится в state; startDay здесь вызывать нельзя.
    while (!isGameFinished(state))
        if (!playDay(state)) return;

    cout << "\n===== ИТОГИ ИГРЫ =====\n";
    if (state.player.getHerd().empty())
        cout << "Все животные погибли. Игра окончена.\n";
    cout << "Деньги: " << state.player.getMoney() << "\n";
    cout << "Животных на ферме: " << state.player.getHerd().size() << "\n";
    cout << "Итоговый счёт: " << state.player.getScore() << "\n";
    waitForInput();
}

void startGame()
{
    GameState state = createGameState();
    cout << "\n";
    cout << "Игра началась!\n";
    cout << "\n" << sideBySide(cowArt(), chickenArt()) << "\n";
    cout << "Уровень сложности: " << state.level << "\n";
    cout << "Животные из настроек:\n";
    for (const auto& entry : state.animalCatalog)
        cout << "  " << entry.first << ": " << entry.second << "\n";
    cout << "Дней: " << state.rules.days << ", действий в день: " << state.rules.actionsPerDay
         << ", стартовые деньги: " << state.rules.startMoney << "\n";
    continueGame(state);
}

void showSaveSlot(int number, const string& title, const string& path)
{
    GameState state;
    string error;
    string timestamp;
    cout << number << ". " << title << ": ";
    if (!readSaveFile(path, state, error, &timestamp))
    {
        cout << "недоступно (" << error << ")\n";
        return;
    }
    cout << timestamp << ", ";
    if (isGameFinished(state)) cout << "игра завершена";
    else cout << "день " << state.day << ", действий " << state.player.getActionsLeft();
    cout << ", денег " << state.player.getMoney() << "\n";
}

void loadGame()
{
    while (true)
    {
        cout << "\n===== ЗАГРУЗКА ИГРЫ =====\n";
        showSaveSlot(1, "Ручное сохранение", manualSaveFile);
        showSaveSlot(2, "Автосохранение", autoSaveFile);
        cout << "0. Назад\nВыберите сохранение: ";
        int choice = readInt();
        if (choice == 0) return;
        if (choice != 1 && choice != 2)
        {
            cout << "Ошибка: такого сохранения нет.\n";
            continue;
        }
        GameState state;
        string error;
        if (!loadGame(choice == 1 ? manualSaveFile : autoSaveFile, state, error))
        {
            cout << "Ошибка загрузки: " << error << ".\n";
            continue;
        }
        cout << "Игра загружена. Уровень сложности: " << state.level << "\n";
        continueGame(state);
        return;
    }
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
    cout << "Лабораторная работа №4. Вариант 2\n";
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
