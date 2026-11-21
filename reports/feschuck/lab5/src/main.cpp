#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <limits>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const int MAX_BUILDINGS = 30; // Максимальное количество зданий в массиве
const int CATALOG_SIZE = 8;   // Количество доступных для постройки зданий

// ==================== НАСТРОЙКА КОНСОЛИ ====================
void setupConsole() {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
}

// ==================== КОНФИГУРАЦИЯ ====================
struct GameConfig {
    int citySize = 10, budget = 1000, population = 100;
};

class ConfigManager {
    GameConfig cfg;
    queue<int> sizeOptions, budgetOptions, popOptions;

    void loadData(const string& filename, queue<int>& q, const int defaults[], int defSize) {
        ifstream in(filename);
        int v;
        if (in.good()) { while (in >> v) q.push(v); return; }
        ofstream out(filename);
        for (int i = 0; i < defSize; ++i) { out << defaults[i] << "\n"; q.push(defaults[i]); }
    }

    int getNextValue(queue<int>& q, int current) {
        if (q.empty()) return current;
        int firstVal = q.front(), nextVal = firstVal;
        int size = q.size();
        for (int i = 0; i < size; ++i) {
            int v = q.front(); q.pop(); q.push(v);
            if (v == current) { q.pop(); nextVal = q.empty() ? firstVal : q.front(); q.push(nextVal); break; }
        }
        return nextVal;
    }

public:
    ConfigManager() {
        int defSizes[] = { 10, 20, 30 };
        int defBudgets[] = { 1000, 5000, 10000 };
        int defPops[] = { 100, 500, 1000 };
        
        loadData("city_size_values.txt", sizeOptions, defSizes, 3);
        loadData("budget_values.txt", budgetOptions, defBudgets, 3);
        loadData("population_values.txt", popOptions, defPops, 3);
        
        ifstream in("settings.txt");
        if (!(in >> cfg.citySize >> cfg.budget >> cfg.population)) saveConfig();
    }
    GameConfig& getSettings() { return cfg; }
    void saveConfig() {
        ofstream out("settings.txt");
        out << cfg.citySize << "\n" << cfg.budget << "\n" << cfg.population << "\n";
    }
    void changeSize() { cfg.citySize = getNextValue(sizeOptions, cfg.citySize); saveConfig(); }
    void changeBudget() { cfg.budget = getNextValue(budgetOptions, cfg.budget); saveConfig(); }
    void changePop() { cfg.population = getNextValue(popOptions, cfg.population); saveConfig(); }
};

// ==================== ИГРОВЫЕ СУЩНОСТИ ====================
enum class ProtectionType { NONE, FIRE, FLOOD };

class CityObject {
public:
    string title;
    int price, weeklyIncome, citizenBonus, moodBonus;
    ProtectionType protection;

    CityObject() : title(""), price(0), weeklyIncome(0), citizenBonus(0), moodBonus(0), protection(ProtectionType::NONE) {}
    
    CityObject(string t, int p, int inc, int pop, int mood, ProtectionType prot = ProtectionType::NONE)
        : title(t), price(p), weeklyIncome(inc), citizenBonus(pop), moodBonus(mood), protection(prot) {}

    void display(int index) const {
        cout << " [" << index << "] " << title
            << " | Стоимость: " << price
            << " | Прибыль: " << weeklyIncome
            << " | +Жители: " << citizenBonus
            << " | Настроение: " << (moodBonus >= 0 ? "+" : "") << moodBonus;
        if (protection == ProtectionType::FIRE) cout << " [Защита: Пожар]";
        if (protection == ProtectionType::FLOOD) cout << " [Защита: Наводнение]";
        cout << "\n";
    }
};

class CitySimulation {
public:
    int maxBuildings, currentBudget, totalPopulation, happinessLevel;
    CityObject builtObjects[MAX_BUILDINGS];
    int builtCount;
    string lastEventText;
    
    CitySimulation(int limit, int cash, int people) 
        : maxBuildings(limit), currentBudget(cash), totalPopulation(people), happinessLevel(50), builtCount(0), lastEventText("Спокойная неделя.") {}

    bool construct(const CityObject& obj) {
        if (currentBudget < obj.price) {
            cout << "Ошибка: не хватает денег на " << obj.title << "!\n";
            return false;
        }
        if (builtCount >= maxBuildings) {
            cout << "Ошибка: в городе больше нет места (лимит: " << maxBuildings << ").\n";
            return false;
        }
        currentBudget -= obj.price;
        totalPopulation += obj.citizenBonus;
        happinessLevel = min(100, max(0, happinessLevel + obj.moodBonus));
        builtObjects[builtCount++] = obj;
        cout << "Успешно возведено: " << obj.title << "\n";
        return true;
    }

    bool hasProtection(ProtectionType type) const {
        for (int i = 0; i < builtCount; ++i) {
            if (builtObjects[i].protection == type) return true;
        }
        return false;
    }

    void printStatus() const {
        cout << "--- ОТЧЕТ О ГОРОДЕ ---\n";
        cout << "Земля: " << maxBuildings << " мест (занято: " << builtCount << ")\n";
        cout << "Казна: " << currentBudget << "\n";
        cout << "Граждане: " << totalPopulation << "\n";
        cout << "Уровень счастья: " << happinessLevel << "/100\n";
        if (builtCount > 0) {
            cout << "Инфраструктура:\n";
            for (int i = 0; i < builtCount; ++i)
                cout << "  - " << builtObjects[i].title << "\n";
        }
    }

    void triggerCataclysm(ProtectionType type) {
        if (type == ProtectionType::FIRE) {
            if (hasProtection(ProtectionType::FIRE)) {
                lastEventText = "ПОЖАР! Но пожарная станция спасла город. (-100 бюджета)";
                currentBudget -= 100;
            } else {
                lastEventText = "КАТАСТРОФА: Сильный пожар! (-500 бюджета, -20 счастья)";
                currentBudget -= 500;
                happinessLevel = max(0, happinessLevel - 20);
            }
        } else if (type == ProtectionType::FLOOD) {
            if (hasProtection(ProtectionType::FLOOD)) {
                lastEventText = "НАВОДНЕНИЕ! Дамба удержала воду. (-100 бюджета)";
                currentBudget -= 100;
            } else {
                lastEventText = "КАТАСТРОФА: Наводнение! (-400 бюджета, -15 счастья, -50 жителей)";
                currentBudget -= 400;
                happinessLevel = max(0, happinessLevel - 15);
                totalPopulation = max(0, totalPopulation - 50);
            }
        }
    }

    int simulateWeek() {
        int profit = 0;
        for (int i = 0; i < builtCount; ++i) profit += builtObjects[i].weeklyIncome;
        int taxes = totalPopulation / 10;
        double multiplier = 0.5 + happinessLevel / 100.0;
        int finalIncome = (int)((profit + taxes) * multiplier);
        currentBudget += finalIncome;
        
        if (happinessLevel >= 60) totalPopulation += totalPopulation / 20;
        else if (happinessLevel <= 30) totalPopulation -= totalPopulation / 25;

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(1, 100);
        int roll = dist(gen);

        if (roll <= 10) {
            currentBudget -= 300;
            lastEventText = "СОБЫТИЕ: Экономический кризис! (-300 бюджета)";
        } else if (roll <= 20) {
            currentBudget += 600;
            lastEventText = "СОБЫТИЕ: Внезапная прибыль от туристов! (+600 бюджета)";
        } else if (roll <= 30) {
            triggerCataclysm(ProtectionType::FIRE);
        } else if (roll <= 40) {
            triggerCataclysm(ProtectionType::FLOOD);
        } else {
            lastEventText = "Спокойная неделя без происшествий.";
        }

        return finalIncome;
    }
};

class PlayerActions {
public:
    int movesLeft, currentWeek;
    int builtCountAction, festivalsCount; 

    PlayerActions() : movesLeft(5), currentWeek(0), builtCountAction(0), festivalsCount(0) {}

    void resetMoves() { movesLeft = 5; }
    bool canMove() const { return movesLeft > 0; }
    void spendMove() { --movesLeft; }
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ КАТАЛОГА ====================
void initCatalog(CityObject catalog[]) {
    catalog[0] = CityObject("Жилой комплекс", 200, 20, 30, 5);
    catalog[1] = CityObject("Торговый центр", 300, 60, 5, 3);
    catalog[2] = CityObject("Завод", 500, 120, 10, -5);
    catalog[3] = CityObject("Сквер", 150, 5, 0, 15);
    catalog[4] = CityObject("Гимназия", 400, 30, 5, 10);
    catalog[5] = CityObject("Оперный театр", 350, 40, 0, 18);
    catalog[6] = CityObject("Пожарная станция", 450, 0, 0, 5, ProtectionType::FIRE);
    catalog[7] = CityObject("Дамба", 600, 0, 0, 2, ProtectionType::FLOOD);
}

// ==================== СИСТЕМА СОХРАНЕНИЙ ====================
class SaveSlotManager {
    const string SAVE_FILE = "saves.txt";
    const char DELIM = '#';

public:
    string getCurrentTimeStr() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
        return string(buffer);
    }

    void saveGame(int slot, const CitySimulation& game, const PlayerActions& player) {
        string lines[3] = {"", "", ""};
        ifstream fin(SAVE_FILE);
        if (fin.is_open()) {
            for (int i = 0; i < 3; ++i) {
                if (!getline(fin, lines[i])) break;
            }
        }
        fin.close();

        stringstream ss;
        ss << (slot - 1) << DELIM 
           << getCurrentTimeStr() << DELIM
           << player.currentWeek << DELIM
           << game.currentBudget << DELIM
           << game.totalPopulation << DELIM
           << game.happinessLevel << DELIM
           << player.builtCountAction << DELIM
           << player.festivalsCount << DELIM
           << player.movesLeft << DELIM;
           
        for (int i = 0; i < game.builtCount; ++i) {
            ss << game.builtObjects[i].title;
            if (i + 1 < game.builtCount) ss << ",";
        }
        
        lines[slot - 1] = ss.str();

        ofstream fout(SAVE_FILE);
        for (int i = 0; i < 3; ++i) {
            fout << lines[i] << "\n";
        }
        cout << "Игра успешно сохранена в слот " << slot << "!\n";
    }

    bool loadGame(int slot, CitySimulation& game, PlayerActions& player, CityObject catalog[], int catalogSize) {
        ifstream fin(SAVE_FILE);
        if (!fin.is_open()) return false;

        string line;
        while (getline(fin, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string token;
            string tokens[10];
            int tokenCount = 0;
            
            while (getline(ss, token, DELIM) && tokenCount < 10) {
                tokens[tokenCount++] = token;
            }
            
            if (tokenCount >= 9 && stoi(tokens[0]) == (slot - 1)) {
                try {
                    player.currentWeek = stoi(tokens[2]);
                    game.currentBudget = stoi(tokens[3]);
                    game.totalPopulation = stoi(tokens[4]);
                    game.happinessLevel = stoi(tokens[5]);
                    player.builtCountAction = stoi(tokens[6]);
                    player.festivalsCount = stoi(tokens[7]);
                    player.movesLeft = stoi(tokens[8]);

                    game.builtCount = 0; 
                    if (tokenCount == 10 && !tokens[9].empty()) {
                        stringstream obj_ss(tokens[9]);
                        string obj_name;
                        while (getline(obj_ss, obj_name, ',')) {
                            for (int i = 0; i < catalogSize; ++i) {
                                if (catalog[i].title == obj_name) {
                                    game.builtObjects[game.builtCount++] = catalog[i];
                                    break;
                                }
                            }
                        }
                    }
                    return true;
                } catch (...) {
                    return false;
                }
            }
        }
        return false;
    }

    void printSavesInfo() {
        ifstream fin(SAVE_FILE);
        if (!fin.is_open()) {
            cout << "\nСохранений пока нет.\n";
            return;
        }
        string line;
        bool hasSaves = false;
        cout << "\n--- ДОСТУПНЫЕ СОХРАНЕНИЯ ---\n";
        for (int i = 0; i < 3; ++i) {
            if (getline(fin, line) && !line.empty()) {
                stringstream ss(line);
                string token;
                string tokens[3];
                int tokenCount = 0;
                while (getline(ss, token, DELIM) && tokenCount < 3) {
                    tokens[tokenCount++] = token;
                }
                if (tokenCount >= 2) {
                    cout << "Слот " << (i + 1) << " | Время: " << tokens[1] << "\n";
                    hasSaves = true;
                }
            }
        }
        if (!hasSaves) cout << "Сохранений пока нет.\n";
        cout << "----------------------------\n";
    }
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ UI ====================
void cls() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\n[Нажмите Enter для продолжения...]";
    cin.get();
}

int getUserInput(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) { cin.ignore(10000, '\n'); return val; }
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Неверный формат. Попробуйте снова.\n";
    }
}

string getUserString(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ==================== МЕНЮ НАСТРОЕК (ВОЗВРАЩЕНО) ====================
void configureGame(ConfigManager& cfm) {
    int choice = 0;
    while (choice != 4) {
        cls();
        cout << "=====================================\n";
        cout << " НАСТРОЙКИ СИМУЛЯЦИИ\n";
        cout << "=====================================\n";
        cout << "1. Площадь города: " << cfm.getSettings().citySize << "\n";
        cout << "2. Бюджет: " << cfm.getSettings().budget << "\n";
        cout << "3. Население: " << cfm.getSettings().population << "\n";
        cout << "4. Вернуться в главное меню\n";
        cout << "=====================================\n";
        choice = getUserInput("Выберите параметр: ");
        if (choice == 1) cfm.changeSize();
        else if (choice == 2) cfm.changeBudget();
        else if (choice == 3) cfm.changePop();
    }
}

// ==================== ОСНОВНОЙ ЦИКЛ ИГРЫ ====================
void playGame(CitySimulation& game, PlayerActions& player, SaveSlotManager& saveMgr) {
    CityObject catalog[CATALOG_SIZE];
    initCatalog(catalog);
    const int TOTAL_WEEKS = 6;

    while (true) {
        if (player.movesLeft == 5) {
            player.currentWeek++;
            if (player.currentWeek > TOTAL_WEEKS) break;
            cout << "\n---------------------------------------------\n";
            cout << " ТЕКУЩАЯ НЕДЕЛЯ: " << player.currentWeek << " из " << TOTAL_WEEKS << "\n";
            cout << "---------------------------------------------\n";
        }

        while (player.canMove()) {
            cls();
            game.printStatus();
            cout << "\nХодов осталось: " << player.movesLeft << "\n";
            cout << "1. Строительство\n";
            cout << "2. Организовать праздник (+10 счастья, -100 монет)\n";
            cout << "3. Завершить неделю\n";
            cout << "4. Сохранить игру\n";
            int action = getUserInput("Ваш выбор: ");

            if (action == 1) {
                cls();
                cout << "=== ДОСТУПНЫЕ ПРОЕКТЫ ===\n";
                for (int i = 0; i < CATALOG_SIZE; ++i) catalog[i].display(i + 1);
                cout << " 0. Вернуться\n";
                int choice = getUserInput("Что будем строить? ");
                if (choice >= 1 && choice <= CATALOG_SIZE) {
                    if (game.construct(catalog[choice - 1])) {
                        player.spendMove();
                        player.builtCountAction++;
                    }
                    pause();
                }
            }
            else if (action == 2) {
                if (game.currentBudget >= 100) {
                    string festName = getUserString("Введите название праздника: ");
                    if (festName.empty()) festName = "Веселый фестиваль";
                    
                    game.currentBudget -= 100;
                    game.happinessLevel = min(100, game.happinessLevel + 10);
                    cout << "Праздник \"" << festName << "\" удался! Счастье выросло.\n";
                    player.spendMove();
                    player.festivalsCount++;
                }
                else cout << "В казне пусто, праздник отменяется.\n";
                pause();
            }
            else if (action == 3) {
                player.movesLeft = 0;
            }
            else if (action == 4) {
                int slot = getUserInput("Выберите слот для сохранения (1-3): ");
                if (slot >= 1 && slot <= 3) {
                    saveMgr.saveGame(slot, game, player);
                } else {
                    cout << "Неверный номер слота!\n";
                }
                pause();
            }
            else {
                cout << "Такого пункта нет!\n";
                pause();
            }
        }

        if (game.currentBudget < -1000) {
            cls();
            cout << "=== ИГРА ОКОНЧЕНА: ПОРАЖЕНИЕ ===\n";
            cout << "Ваш бюджет ушел в глубокий минус (-1000). Город объявлен банкротом!\n";
            pause();
            return;
        }
        if (game.totalPopulation <= 0) {
            cls();
            cout << "=== ИГРА ОКОНЧЕНА: ПОРАЖЕНИЕ ===\n";
            cout << "Население города достигло нуля. Люди разбежались!\n";
            pause();
            return;
        }

        if (player.currentWeek <= TOTAL_WEEKS) {
            int weekProfit = game.simulateWeek();
            cout << "\n=== ИТОГИ НЕДЕЛИ " << player.currentWeek << " ===\n";
            cout << "Прибыль: " << weekProfit << "\n";
            cout << "СОБЫТИЕ НЕДЕЛИ: " << game.lastEventText << "\n";
            cout << "Остаток в казне: " << game.currentBudget << "\n";
            cout << "Численность: " << game.totalPopulation << "\n";
            cout << "Счастье: " << game.happinessLevel << "/100\n";
            pause();
            player.movesLeft = 5; 
        }
    }

    cls();
    cout << "=== СИМУЛЯЦИЯ ЗАВЕРШЕНА ===\n";
    game.printStatus();
    
    int finalScore = game.currentBudget + game.totalPopulation * 2 + game.happinessLevel * 5;
    
    cout << "\n=============================================\n";
    cout << " ВЕРДИКТ ГОРОДСКОГО СОВЕТА\n";
    cout << "=============================================\n";
    
    if (game.currentBudget > 5000 && game.totalPopulation > 300 && game.happinessLevel >= 70) {
        cout << "[+] ВЫ - ВЕЛИКИЙ МЭР (ПОБЕДА)!\n";
        cout << "Город процветает! Экономика на подъеме, люди счастливы.\n";
    } else if (game.happinessLevel <= 20) {
        cout << "[!] ГОРОД В УПАДКЕ (ПОРАЖЕНИЕ)!\n";
        cout << "Вы забыли о счастье горожан. Начались массовые протесты.\n";
    } else if (player.builtCountAction <= 2) {
        cout << "[!] ВЫ ЕДВА СПРАВИЛИСЬ (НИЧЬЯ)\n";
        cout << "Город развивался слишком медленно.\n";
    } else {
        cout << "[+] СТАБИЛЬНЫЙ РУКОВОДИТЕЛЬ\n";
        cout << "Вы неплохо справились, но до идеала далеко.\n";
    }
    cout << "=============================================\n";
    cout << "\nВаш итоговый рейтинг: " << finalScore << "\n";
    pause();
}

void runSimulation(const GameConfig& settings, SaveSlotManager& saveMgr) {
    cls();
    cout << ">>> СТАРТ СИМУЛЯЦИИ: «ГОРОД МЕЧТЫ» <<<\n\n";
    cout << "Добро пожаловать на пост мэра!\n";
    cout << "Ваши стартовые условия:\n";
    cout << "  Площадь: " << settings.citySize << "\n";
    cout << "  Стартовый капитал: " << settings.budget << "\n";
    cout << "  Жители: " << settings.population << "\n";

    CitySimulation game(settings.citySize, settings.budget, settings.population);
    PlayerActions player;
    playGame(game, player, saveMgr);
}

void loadGameMenu(SaveSlotManager& saveMgr) {
    cls();
    cout << "=== ЗАГРУЗКА ИГРЫ ===\n";
    saveMgr.printSavesInfo();
    int slot = getUserInput("Выберите слот для загрузки (1-3) или 0 для отмены: ");
    if (slot >= 1 && slot <= 3) {
        GameConfig defaultCfg; 
        CitySimulation game(defaultCfg.citySize, defaultCfg.budget, defaultCfg.population);
        PlayerActions player;
        
        CityObject catalog[CATALOG_SIZE];
        initCatalog(catalog);
        
        if (saveMgr.loadGame(slot, game, player, catalog, CATALOG_SIZE)) {
            cout << "Игра успешно загружена из слота " << slot << "!\n";
            pause();
            playGame(game, player, saveMgr); 
        } else {
            cout << "Слот пуст или поврежден.\n";
            pause();
        }
    }
}

#ifndef UNIT_TEST
int main() {
    setupConsole();
    setlocale(LC_ALL, "Russian");
    ConfigManager config;
    SaveSlotManager saveMgr;
    int menuChoice = 0;

    while (menuChoice != 5) {
        cls();
        cout << "=====================================\n";
        cout << " ДОБРО ПОЖАЛОВАТЬ В ГОРОД МЕЧТЫ\n";
        cout << "=====================================\n";
        cout << "1. Новая игра\n";
        cout << "2. Загрузить игру\n";
        cout << "3. Параметры\n";
        cout << "4. Информация\n";
        cout << "5. Выход\n";
        cout << "=====================================\n";
        menuChoice = getUserInput("Выберите действие: ");

        if (menuChoice == 1) runSimulation(config.getSettings(), saveMgr);
        else if (menuChoice == 2) loadGameMenu(saveMgr);
        else if (menuChoice == 3) {
             configureGame(config); // <--- ТЕПЕРЬ ЗДЕСЬ ВЫЗОВ НАСТРОЕК
        }
        else if (menuChoice == 4) {
            cls();
            cout << "=== О ПРОЕКТЕ ===\n";
            cout << "Игра: «Город мечты»\n";
            cout << "Версия: 4.0 (ЛР5)\n";
            cout << "Автор: Фещук Артём\n";
            pause();
        }
        else if (menuChoice == 5) cout << "Завершение работы...\n";
        else { cout << "Ошибка ввода!\n"; pause(); }
    }
    return 0;
}
#endif
