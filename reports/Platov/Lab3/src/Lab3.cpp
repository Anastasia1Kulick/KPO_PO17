#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>

using namespace std;

enum Menu
{
    START_GAME = 1,
    LOAD_GAME,
    SETTINGS,
    ABOUT,
    EXIT
};

enum SettingsMenu
{
    SHOW = 1,
    CITY_SIZE,
    BUDGET,
    POPULATION,
    SET_EXIT
};

enum LoadMenu
{
    LOAD_MANUAL = 1,
    LOAD_AUTO,
    LOAD_CANCEL
};

queue<int> citySizeValues;
queue<int> budgetValues;
queue<int> populationValues;
const string SETTINGS_FILE = "settings.txt";
const string SAVE_FILE = "savegame.txt";
const string AUTOSAVE_FILE = "autosave.txt";

class Building
{
private:
    string name;
    int cost;
    int weeklyIncome;
    int populationBonus;
    int maxCount;
    int area;

public:
    Building() : name(""), cost(0), weeklyIncome(0), populationBonus(0), maxCount(0), area(0) {}
    Building(string n, int c, int inc, int pop, int maxC, int a)
        : name(n), cost(c), weeklyIncome(inc), populationBonus(pop), maxCount(maxC), area(a) {}

    string getName() const { return name; }
    int getCost() const { return cost; }
    int getWeeklyIncome() const { return weeklyIncome; }
    int getPopulationBonus() const { return populationBonus; }
    int getMaxCount() const { return maxCount; }
    int getArea() const { return area; }
};

struct Event
{
    string name;
    int cost;
    int reputationBonus;
    int incomeBoostPercent;
};

class Town
{
private:
    int cityArea;
    int usedArea;
    int budget;
    int population;
    vector<Building> builtBuildings;
    int nextWeekIncomeBonusPercent;
public:
    Town(int area, int startBudget, int startPop)
        : cityArea(area), usedArea(0), budget(startBudget), population(startPop),
        nextWeekIncomeBonusPercent(0) {}
    Town(int area, int used, int bud, int pop, int bonus,
        const vector<Building>& buildings)
        : cityArea(area), usedArea(used), budget(bud), population(pop),
        builtBuildings(buildings), nextWeekIncomeBonusPercent(bonus) {}
    int getCityArea() const { return cityArea; }
    int getUsedArea() const { return usedArea; }
    int getFreeArea() const { return cityArea - usedArea; }
    int getBudget() const { return budget; }
    int getPopulation() const { return population; }
    int getBuildingsCount() const { return (int)builtBuildings.size(); }
    int getIncomeBonusPercent() const { return nextWeekIncomeBonusPercent; }
    const vector<Building>& getBuiltBuildings() const { return builtBuildings; }
    bool canBuild(const Building& b) const
    {
        if (budget < b.getCost()) return false;
        if (usedArea + b.getArea() > cityArea) return false;
        int already = 0;
        for (size_t i = 0; i < builtBuildings.size(); ++i)
            if (builtBuildings[i].getName() == b.getName()) already++;
        if (already >= b.getMaxCount()) return false;
        return true;
    }
    bool buildBuilding(const Building& b)
    {
        if (!canBuild(b)) return false;
        budget -= b.getCost();
        usedArea += b.getArea();
        builtBuildings.push_back(b);
        population += b.getPopulationBonus();
        return true;
    }
    void applyEvent(const Event& e)
    {
        budget -= e.cost;
        nextWeekIncomeBonusPercent += e.incomeBoostPercent;
    }
    int endWeek()
    {
        int baseIncome = 0;
        for (size_t i = 0; i < builtBuildings.size(); ++i)
            baseIncome += builtBuildings[i].getWeeklyIncome();
        baseIncome += population / 10;

        int bonus = baseIncome * nextWeekIncomeBonusPercent / 100;
        int income = baseIncome + bonus;

        budget += income;
        nextWeekIncomeBonusPercent = 0;
        return income;
    }
};

class Mayor
{
private:
    string name;
    int reputation;
public:
    Mayor(string n = "Мэр") : name(n), reputation(50) {}
    Mayor(string n, int rep) : name(n), reputation(rep) {}
    string getName() const { return name; }
    int getReputation() const { return reputation; }
    void addReputation(int delta)
    {
        reputation += delta;
        if (reputation < 0) reputation = 0;
        if (reputation > 100) reputation = 100;
    }
    void addReputationPercent(int percent)
    {
        int delta = reputation * percent / 100;
        if (delta < 1) delta = 1;
        reputation += delta;
        if (reputation > 100) reputation = 100;
    }
};

vector<Building> getAvailableBuildings()
{
    vector<Building> list;
    list.push_back(Building("Частный дом", 800, 60, 10, 20, 10));
    list.push_back(Building("Квартира", 1200, 90, 25, 15, 8));
    list.push_back(Building("Общежитие", 2000, 120, 60, 8, 20));
    list.push_back(Building("Школа", 2500, 150, 15, 3, 25));
    list.push_back(Building("Больница", 3500, 180, 20, 2, 30));
    list.push_back(Building("Магазин", 1500, 220, 10, 6, 12));
    list.push_back(Building("Парк", 800, 50, 40, 6, 40));
    list.push_back(Building("Бизнес-центр", 6000, 600, 30, 2, 45));
    list.push_back(Building("Банк", 5000, 500, 15, 2, 30));
    list.push_back(Building("Завод", 4000, 450, 10, 3, 60));
    return list;
}

vector<Event> getAvailableEvents()
{
    vector<Event> list;
    list.push_back(Event{ "Салют",       300,  5,  0 });
    list.push_back(Event{ "Ярмарка",     500, 10, 25 });
    list.push_back(Event{ "День города", 800, 20, 15 });
    list.push_back(Event{ "Парад",      1200, 15, 10 });
    return list;
}

void createDefaultSettingsFile()
{
    ofstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Не удалось создать файл " << SETTINGS_FILE << endl;
        return;
    }
    file << "1000 5000 10000" << endl;
    file << "1000 5000 10000" << endl;
    file << "100 500 1000" << endl;
    file.close();
    cout << "Создан файл настроек по умолчанию: " << SETTINGS_FILE << endl;
}

void saveQueue(ofstream& file, queue<int> q)
{
    bool first = true;
    while (!q.empty())
    {
        if (!first) file << " ";
        file << q.front();
        q.pop();
        first = false;
    }
    file << "\n";
}

bool saveSettingsToFile()
{
    ofstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Не удалось сохранить файл " << SETTINGS_FILE << endl;
        return false;
    }
    saveQueue(file, citySizeValues);
    saveQueue(file, budgetValues);
    saveQueue(file, populationValues);
    file.close();
    return true;
}

bool loadSettingsFromFile()
{
    ifstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Файл " << SETTINGS_FILE
            << " не найден. Создаём настройки по умолчанию..." << endl;
        createDefaultSettingsFile();
        file.open(SETTINGS_FILE);
        if (!file.is_open())
        {
            cout << "Не удалось открыть файл " << SETTINGS_FILE << endl;
            return false;
        }
    }
    int value = 0;
    while (file.peek() != '\n' && file >> value)
    {
        citySizeValues.push(value);
    }
    file.ignore(10000, '\n');
    while (file.peek() != '\n' && file >> value)
    {
        budgetValues.push(value);
    }
    file.ignore(10000, '\n');
    while (file >> value)
    {
        populationValues.push(value);
    }
    file.close();
    if (citySizeValues.empty() || budgetValues.empty() || populationValues.empty())
    {
        cout << "Файл настроек пуст или имеет неверный формат." << endl;
        return false;
    }
    return true;
}

void nextValue(queue<int>& q, int& current)
{
    if (q.empty()) return;
    size_t n = q.size();
    while (n-- > 0 && q.front() != current)
    {
        q.push(q.front());
        q.pop();
    }
    int oldValue = q.front();
    q.pop();
    current = q.front();
    q.push(oldValue);
}

bool fileExists(const string& filename)
{
    ifstream f(filename);
    return f.good();
}

bool saveGame(const Town& town, const Mayor& mayor, int week,
    const string& filename, bool isAutosave)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cout << "Не удалось открыть файл для записи: " << filename << endl;
        return false;
    }
    file << (isAutosave ? "AUTO" : "MANUAL") << "\n";
    file << week << "\n";
    file << town.getCityArea() << " "
        << town.getUsedArea() << " "
        << town.getBudget() << " "
        << town.getPopulation() << " "
        << town.getIncomeBonusPercent() << "\n";
    file << town.getBuildingsCount() << "\n";
    const vector<Building>& built = town.getBuiltBuildings();
    for (size_t i = 0; i < built.size(); ++i)
        file << built[i].getName() << "\n";
    file << mayor.getName() << "\n";
    file << mayor.getReputation() << "\n";
    file.close();
    return true;
}

bool loadGame(const string& filename, Town& town, Mayor& mayor, int& week,
    bool& isAutosave)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Файл сохранения не найден: " << filename << endl;
        return false;
    }
    string modeLine;
    if (!getline(file, modeLine)) return false;
    isAutosave = (modeLine == "AUTO");
    if (!(file >> week)) return false;
    int area, used, budget, pop, bonus;
    if (!(file >> area >> used >> budget >> pop >> bonus)) return false;
    int count = 0;
    if (!(file >> count)) return false;
    file.ignore(10000, '\n');
    vector<Building> catalog = getAvailableBuildings();
    vector<Building> built;
    for (int i = 0; i < count; ++i)
    {
        string bname;
        if (!getline(file, bname)) return false;
        if (!bname.empty() && bname.back() == '\r') bname.pop_back();
        bool found = false;
        for (size_t j = 0; j < catalog.size(); ++j)
        {
            if (catalog[j].getName() == bname)
            {
                built.push_back(catalog[j]);
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "Предупреждение: здание \"" << bname
                << "\" не найдено в каталоге, пропущено." << endl;
        }
    }
    string mayorName;
    if (!getline(file, mayorName)) return false;
    if (!mayorName.empty() && mayorName.back() == '\r') mayorName.pop_back();
    int reputation = 50;
    if (!(file >> reputation)) return false;
    file.close();
    town = Town(area, used, budget, pop, bonus, built);
    mayor = Mayor(mayorName, reputation);
    return true;
}

void Menu()
{
    cout << "==== ГОРОД МЕЧТЫ ====" << endl;
    cout << START_GAME << ". Начать игру" << endl;
    cout << LOAD_GAME << ". Загрузить игру" << endl;
    cout << SETTINGS << ". Настройки" << endl;
    cout << ABOUT << ". О программе" << endl;
    cout << EXIT << ". Выход" << endl;
    cout << "=====================" << endl;
}

void MenuSettings()
{
    cout << "========= НАСТРОЙКИ =========" << endl;
    cout << SHOW << ". Просмотр текущих настроек" << endl;
    cout << CITY_SIZE << ". Размер города" << endl;
    cout << BUDGET << ". Начальный бюджет" << endl;
    cout << POPULATION << ". Количество жителей" << endl;
    cout << SET_EXIT << ". Выход" << endl;
    cout << "=============================" << endl;
}

void MenuLoad()
{
    cout << "========= ЗАГРУЗКА ИГРЫ =========" << endl;
    cout << LOAD_MANUAL << ". Загрузить обычное сохранение (savegame.txt)";
    if (!fileExists(SAVE_FILE)) cout << " [нет файла]";
    cout << endl;
    cout << LOAD_AUTO << ". Загрузить автосохранение (autosave.txt)";
    if (!fileExists(AUTOSAVE_FILE)) cout << " [нет файла]";
    cout << endl;
    cout << LOAD_CANCEL << ". Отмена" << endl;
    cout << "=================================" << endl;
}

void aboutProgramm()
{
    cout << "====== О ПРОГРАММЕ ======" << endl;
    cout << "Игра:    «Город мечты»" << endl;
    cout << "Версия:  3.1" << endl;
    cout << "Студент: Платов Александр" << endl;
    cout << "=========================" << endl;
}

void showSettings(int citySize, int budget, int population)
{
    cout << "Текущий размер города: " << citySize << endl;
    cout << "Текущий бюджет города: " << budget << endl;
    cout << "Текущее количество жителей города: " << population << endl;
}

void gameLoop(Town& town, Mayor& mayor, int& week, bool loadedGame)
{
    vector<Building> catalog = getAvailableBuildings();

    if (!loadedGame)
    {
        cout << "===== ДОБРО ПОЖАЛОВАТЬ В «ГОРОД МЕЧТЫ» =====" << endl;
        cout << "Мэр: " << mayor.getName() << endl;
        cout << "Площадь города: " << town.getCityArea() << " клеток" << endl;
        cout << "Стартовый бюджет: " << town.getBudget() << endl;
        cout << "Население: " << town.getPopulation() << endl;
    }
    else
    {
        cout << "===== ИГРА ЗАГРУЖЕНА =====" << endl;
        cout << "Мэр: " << mayor.getName() << endl;
        cout << "Неделя: " << week << endl;
        cout << "Площадь: " << town.getCityArea()
            << " | занято: " << town.getUsedArea() << endl;
        cout << "Бюджет: " << town.getBudget()
            << " | население: " << town.getPopulation()
            << " | зданий: " << town.getBuildingsCount() << endl;
        cout << "Репутация мэра: " << mayor.getReputation() << endl;
    }
    system("pause");
    system("cls");
    bool running = true;
    while (running)
    {
        cout << "========== НЕДЕЛЯ " << week << " ==========" << endl;
        cout << "Бюджет: " << town.getBudget()
            << " | Население: " << town.getPopulation()
            << " | Зданий: " << town.getBuildingsCount() << endl;
        cout << "Площадь: занято " << town.getUsedArea()
            << " из " << town.getCityArea()
            << " (свободно " << town.getFreeArea() << ")" << endl;
        cout << "Репутация мэра: " << mayor.getReputation();
        if (town.getIncomeBonusPercent() > 0)
            cout << " | бонус к доходу: +" << town.getIncomeBonusPercent() << "%";
        cout << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Построить здание" << endl;
        cout << "2. Провести мероприятие" << endl;
        cout << "3. Завершить неделю" << endl;
        cout << "4. Покинуть игру (с сохранением)" << endl;
        cout << "Выбор: ";
        int choice;
        cin >> choice;
        system("cls");
        switch (choice)
        {
        case 1:
        {
            cout << "Доступные здания (свободно " << town.getFreeArea() << " клеток):" << endl;
            for (size_t i = 0; i < catalog.size(); ++i)
            {
                const Building& b = catalog[i];
                cout << i + 1 << ". " << b.getName()
                    << " | цена: " << b.getCost()
                    << " | площадь: " << b.getArea()
                    << " | доход/нед: " << b.getWeeklyIncome()
                    << " | +жителей: " << b.getPopulationBonus()
                    << " | максимум: " << b.getMaxCount() << endl;
            }
            cout << "0. Отмена" << endl;
            cout << "Выберите здание: ";
            int idx;
            cin >> idx;
            if (idx >= 1 && idx <= (int)catalog.size())
            {
                const Building& b = catalog[idx - 1];
                if (town.buildBuilding(b))
                {
                    cout << "Построено: " << b.getName()
                        << " (занято " << b.getArea() << " клеток)" << endl;
                    mayor.addReputation(2);
                    if (saveGame(town, mayor, week, AUTOSAVE_FILE, true))
                        cout << "[Автосохранение выполнено]" << endl;
                }
                else
                {
                    if (town.getBudget() < b.getCost())
                        cout << "Недостаточно бюджета." << endl;
                    else if (town.getFreeArea() < b.getArea())
                        cout << "Не хватает места: нужно " << b.getArea()
                        << ", свободно " << town.getFreeArea() << "." << endl;
                    else
                        cout << "Достигнут максимум для этого типа здания." << endl;
                }
            }
            system("pause");
            system("cls");
            break;
        }
        case 2:
        {
            vector<Event> events = getAvailableEvents();
            cout << "Доступные мероприятия:" << endl;
            for (size_t i = 0; i < events.size(); ++i)
            {
                const Event& e = events[i];
                cout << i + 1 << ". " << e.name
                    << " | цена: " << e.cost
                    << " | +" << e.reputationBonus << "% репутации";
                if (e.incomeBoostPercent > 0)
                    cout << " | +" << e.incomeBoostPercent << "% к доходу след. недели";
                cout << endl;
            }
            cout << "0. Отмена" << endl;
            cout << "Выбор: ";
            int e;
            cin >> e;

            if (e >= 1 && e <= (int)events.size())
            {
                const Event& ev = events[e - 1];
                if (town.getBudget() < ev.cost)
                {
                    cout << "Недостаточно бюджета: нужно " << ev.cost
                        << ", у вас " << town.getBudget() << "." << endl;
                }
                else
                {
                    town.applyEvent(ev);
                    int repBefore = mayor.getReputation();
                    mayor.addReputationPercent(ev.reputationBonus);
                    int repGain = mayor.getReputation() - repBefore;
                    cout << "Проведено мероприятие: " << ev.name << endl;
                    cout << "Репутация: " << mayor.getReputation()
                        << " (+" << ev.reputationBonus << "%, факт +" << repGain << ")";
                    if (ev.incomeBoostPercent > 0)
                        cout << " | бонус к доходу: +" << town.getIncomeBonusPercent() << "%";
                    cout << endl;
                    if (saveGame(town, mayor, week, AUTOSAVE_FILE, true))
                        cout << "[Автосохранение выполнено]" << endl;
                }
            }
            system("pause");
            system("cls");
            break;
        }
        case 3:
        {
            int income = town.endWeek();
            cout << "===== ИТОГИ НЕДЕЛИ " << week << " =====" << endl;
            cout << "Доход за неделю: " << income << endl;
            cout << "Бюджет: " << town.getBudget() << endl;
            cout << "Население: " << town.getPopulation() << endl;
            cout << "Зданий всего: " << town.getBuildingsCount()
                << " (занято " << town.getUsedArea()
                << "/" << town.getCityArea() << " клеток)" << endl;
            mayor.addReputation(1);
            week++;
            if (saveGame(town, mayor, week, AUTOSAVE_FILE, true))
                cout << "[Автосохранение выполнено]" << endl;
            system("pause");
            system("cls");
            break;
        }
        case 4:
        {
            running = false;
            if (saveGame(town, mayor, week, SAVE_FILE, false))
                cout << "Игра сохранена в " << SAVE_FILE << endl;
            else
                cout << "Не удалось сохранить игру." << endl;
            cout << "Игра завершена. Итоговый бюджет: " << town.getBudget()
                << ", население: " << town.getPopulation()
                << ", зданий: " << town.getBuildingsCount()
                << ", занято площади: " << town.getUsedArea()
                << "/" << town.getCityArea()
                << ", репутация мэра: " << mayor.getReputation() << endl;
            system("pause");
            system("cls");
            break;
        }
        default:
            cout << "Неверный пункт меню." << endl;
            system("pause");
            system("cls");
            break;
        }
    }
}

int main()
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    if (!loadSettingsFromFile())
    {
        cout << "Программа завершена: не удалось загрузить настройки." << endl;
        system("pause");
        return 1;
    }
    int choice = -1;
    int citySize = citySizeValues.front();
    int budget = budgetValues.front();
    int population = populationValues.front();
    while (choice != EXIT)
    {
        Menu();
        cin >> choice;
        system("cls");
        switch (choice)
        {
        case START_GAME:
        {
            Town town(citySize, budget, population);
            Mayor mayor("Платов Александр");
            int week = 1;
            gameLoop(town, mayor, week, false);
            break;
        }
        case LOAD_GAME:
        {
            bool done = false;
            while (!done)
            {
                MenuLoad();
                int lc;
                cin >> lc;
                system("cls");
                if (lc == LOAD_CANCEL)
                {
                    done = true;
                    break;
                }
                if (lc != LOAD_MANUAL && lc != LOAD_AUTO)
                {
                    cout << "Неверный пункт меню." << endl;
                    system("pause");
                    system("cls");
                    continue;
                }
                string fname = (lc == LOAD_MANUAL) ? SAVE_FILE : AUTOSAVE_FILE;
                if (!fileExists(fname))
                {
                    cout << "Файл " << fname << " не найден." << endl;
                    system("pause");
                    system("cls");
                    continue;
                }
                Town town(citySize, budget, population);
                Mayor mayor("Мэр");
                int week = 1;
                bool isAutosave = false;
                if (loadGame(fname, town, mayor, week, isAutosave))
                {
                    cout << "Загрузка из " << fname
                        << (isAutosave ? " (автосохранение)" : " (обычное сохранение)")
                        << " выполнена успешно." << endl;
                    system("pause");
                    system("cls");
                    gameLoop(town, mayor, week, true);
                    done = true;
                }
                else
                {
                    cout << "Не удалось загрузить сохранение." << endl;
                    system("pause");
                    system("cls");
                }
            }
            break;
        }
        case SETTINGS:
        {
            int choice1 = -1;
            while (choice1 != SET_EXIT)
            {
                MenuSettings();
                cin >> choice1;
                system("cls");
                switch (choice1)
                {
                case SHOW:
                    showSettings(citySize, budget, population);
                    system("pause");
                    system("cls");
                    break;
                case CITY_SIZE:
                    cout << "Текущий размер города: " << citySize << endl;
                    nextValue(citySizeValues, citySize);
                    cout << "Размер города после изменений: " << citySize << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                case BUDGET:
                    cout << "Текущий бюджет города: " << budget << endl;
                    nextValue(budgetValues, budget);
                    cout << "Бюджет города после изменений: " << budget << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                case POPULATION:
                    cout << "Текущее количество жителей города: " << population << endl;
                    nextValue(populationValues, population);
                    cout << "Количество жителей города после изменений: " << population << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                case SET_EXIT:
                    cout << "Выход из настроек." << endl;
                    system("pause");
                    system("cls");
                    break;
                default:
                    cout << "Неверный пункт меню. Попробуйте еще раз." << endl;
                    system("pause");
                    system("cls");
                    break;
                }
            }
            break;
        }
        case ABOUT:
            aboutProgramm();
            system("pause");
            system("cls");
            break;
        case EXIT:
            cout << "Выход из программы." << endl;
            break;
        default:
            cout << "Неверный пункт меню. Попробуйте еще раз." << endl;
            system("pause");
            system("cls");
            break;
        }
    }
    return 0;
}