#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
using namespace std;

struct GameConfig {
    int citySize = 10, budget = 1000, population = 100;
};

class ConfigManager {
    GameConfig cfg;
    queue<int> sizeOptions, budgetOptions, popOptions;

    void loadData(const string& filename, queue<int>& q, const vector<int>& defaults) {
        ifstream in(filename);
        int v;
        if (in.good()) { while (in >> v) q.push(v); return; }
        ofstream out(filename);
        for (int x : defaults) { out << x << "\n"; q.push(x); }
    }

    int getNextValue(queue<int>& q, int current) {
        if (q.empty()) return current;
        int firstVal = q.front(), nextVal = firstVal;
        for (size_t i = 0; i < q.size(); ++i) {
            int v = q.front(); q.pop(); q.push(v);
            if (v == current) { q.pop(); nextVal = q.empty() ? firstVal : q.front(); q.push(nextVal); break; }
        }
        return nextVal;
    }

public:
    ConfigManager() {
        loadData("city_size_values.txt", sizeOptions, { 10, 20, 30 });
        loadData("budget_values.txt", budgetOptions, { 1000, 5000, 10000 });
        loadData("population_values.txt", popOptions, { 100, 500, 1000 });
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
class CityObject {
public:
    string title;
    int price, weeklyIncome, citizenBonus, moodBonus;
    CityObject(string t, int p, int inc, int pop, int mood)
        : title(t), price(p), weeklyIncome(inc), citizenBonus(pop), moodBonus(mood) {}

    void display(int index) const {
        cout << " [" << index << "] " << title
            << " | Стоимость: " << price
            << " | Прибыль: " << weeklyIncome
            << " | +Жители: " << citizenBonus
            << " | Настроение: " << (moodBonus >= 0 ? "+" : "") << moodBonus << "\n";
    }
};

class CitySimulation {
public:
    int maxBuildings, currentBudget, totalPopulation, happinessLevel = 50;
    vector<CityObject> constructedObjects;
    
    CitySimulation(int limit, int cash, int people) 
        : maxBuildings(limit), currentBudget(cash), totalPopulation(people) {}

    bool construct(const CityObject& obj) {
        if (currentBudget < obj.price) {
            cout << "Ошибка: не хватает денег на " << obj.title << "!\n";
            return false;
        }
        if ((int)constructedObjects.size() >= maxBuildings) {
            cout << "Ошибка: в городе больше нет места (лимит: " << maxBuildings << ").\n";
            return false;
        }
        currentBudget -= obj.price;
        totalPopulation += obj.citizenBonus;
        happinessLevel = min(100, max(0, happinessLevel + obj.moodBonus));
        constructedObjects.push_back(obj);
        cout << "Успешно возведено: " << obj.title << "\n";
        return true;
    }

    void printStatus() const {
        cout << "--- ОТЧЕТ О ГОРОДЕ ---\n";
        cout << "Земля: " << maxBuildings << " мест (занято: " << constructedObjects.size() << ")\n";
        cout << "Казна: " << currentBudget << "\n";
        cout << "Граждане: " << totalPopulation << "\n";
        cout << "Уровень счастья: " << happinessLevel << "/100\n";
        if (!constructedObjects.empty()) {
            cout << "Инфраструктура:\n";
            for (size_t i = 0; i < constructedObjects.size(); ++i)
                cout << "  - " << constructedObjects[i].title << "\n";
        }
    }

    int simulateWeek() {
        int profit = 0;
        for (auto& b : constructedObjects) profit += b.weeklyIncome;
        int taxes = totalPopulation / 10;
        double multiplier = 0.5 + happinessLevel / 100.0;
        int finalIncome = int((profit + taxes) * multiplier);
        currentBudget += finalIncome;
        if (happinessLevel >= 60) totalPopulation += totalPopulation / 20;
        else if (happinessLevel <= 30) totalPopulation -= totalPopulation / 25;
        return finalIncome;
    }
};

class PlayerActions {
public:
    int movesLeft = 5, currentWeek = 0;
    // Добавили счетчики для концовок
    int builtCount = 0; 
    int festivalsCount = 0; 

    void resetMoves() { movesLeft = 5; }
    bool canMove() const { return movesLeft > 0; }
    void spendMove() { --movesLeft; }
};

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
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

// ==================== ОСНОВНОЙ ЦИКЛ ====================
void runSimulation(const GameConfig& settings) {
    cls();
    cout << ">>> СТАРТ СИМУЛЯЦИИ: «ГОРОД МЕЧТЫ» <<<\n\n";
    cout << "Добро пожаловать на пост мэра!\n";
    cout << "Ваши стартовые условия:\n";
    cout << "  Площадь: " << settings.citySize << "\n";
    cout << "  Стартовый капитал: " << settings.budget << "\n";
    cout << "  Жители: " << settings.population << "\n";

    CitySimulation game(settings.citySize, settings.budget, settings.population);
    PlayerActions player;
    vector<CityObject> shop = {
        {"Жилой комплекс", 200, 20, 30, 5},
        {"Торговый центр", 300, 60, 5, 3},
        {"Завод", 500, 120, 10, -5},
        {"Сквер", 150, 5, 0, 15},
        {"Гимназия", 400, 30, 5, 10},
        {"Оперный театр", 350, 40, 0, 18}
    };

    const int TOTAL_WEEKS = 6;
    while (player.currentWeek < TOTAL_WEEKS) {
        player.resetMoves();
        player.currentWeek++;
        cout << "\n---------------------------------------------\n";
        cout << " ТЕКУЩАЯ НЕДЕЛЯ: " << player.currentWeek << " из " << TOTAL_WEEKS << "\n";
        cout << "---------------------------------------------\n";

        while (player.canMove()) {
            cls();
            game.printStatus();
            cout << "\nХодов осталось: " << player.movesLeft << "\n";
            cout << "1. Строительство\n";
            cout << "2. Организовать праздник (+10 счастья, -100 монет)\n";
            cout << "3. Завершить неделю\n";
            int action = getUserInput("Ваш выбор: ");

            if (action == 1) {
                cls();
                cout << "=== ДОСТУПНЫЕ ПРОЕКТЫ ===\n";
                for (size_t i = 0; i < shop.size(); ++i) shop[i].display(i + 1);
                cout << " 0. Вернуться\n";
                int choice = getUserInput("Что будем строить? ");
                if (choice >= 1 && choice <= (int)shop.size()) {
                    if (game.construct(shop[choice - 1])) {
                        player.spendMove();
                        player.builtCount++; // Считаем постройки
                    }
                    pause();
                }
            }
            else if (action == 2) {
                if (game.currentBudget >= 100) {
                    game.currentBudget -= 100;
                    game.happinessLevel = min(100, game.happinessLevel + 10);
                    cout << "Праздник удался! Счастье выросло.\n";
                    player.spendMove();
                    player.festivalsCount++; // Считаем праздники
                }
                else cout << "В казне пусто, праздник отменяется.\n";
                pause();
            }
            else if (action == 3) {
                player.movesLeft = 0;
            }
            else {
                cout << "Такого пункта нет!\n";
                pause();
            }
        }

        int weekProfit = game.simulateWeek();
        cout << "\n=== ИТОГИ НЕДЕЛИ " << player.currentWeek << " ===\n";
        cout << "Прибыль: " << weekProfit << "\n";
        cout << "Остаток в казне: " << game.currentBudget << "\n";
        cout << "Численность: " << game.totalPopulation << "\n";
        cout << "Счастье: " << game.happinessLevel << "/100\n";
        pause();
    }

    cls();
    cout << "=== СИМУЛЯЦИЯ ЗАВЕРШЕНА ===\n";
    game.printStatus();
    
    int finalScore = game.currentBudget + game.totalPopulation * 2 + game.happinessLevel * 5;
    
    // ==================== ВЫСТАВЛЕНИЕ ВЕРДИКТА ====================
    cout << "\n=============================================\n";
    cout << " ВЕРДИКТ ГОРОДСКОГО СОВЕТА\n";
    cout << "=============================================\n";
    
    if (player.builtCount == 0 && player.festivalsCount == 0) {
        cout << "❌ СНЯТ С ДОЛЖНОСТИ МЭРА!\n";
        cout << "Вы абсолютно ничего не сделали для города. За 6 недель не было\n";
        cout << "построено ни одного здания и не проведено ни одного праздника.\n";
        cout << "Горожане требуют импичмента!\n";
    } else if (player.builtCount <= 3) {
        cout << "⚠️ ВЫ ЕДВА СПРАВИЛИСЬ С ОБЯЗАННОСТЯМИ\n";
        cout << "Город развивался слишком медленно (построено зданий: " << player.builtCount << ").\n";
        cout << "Совет выражает вам недоверие, но дает второй шанс.\n";
    } else if (game.happinessLevel <= 20) {
        cout << "😡 ГОРОД В УПАДКЕ!\n";
        cout << "Вы настроили много зданий (" << player.builtCount << "), но забыли о людях.\n";
        cout << "Уровень счастья упал до " << game.happinessLevel << ". Начались забастовки!\n";
    } else if (player.builtCount >= 10 && game.happinessLevel >= 70) {
        cout << "🏆 ВЫ - ВЕЛИКИЙ МЭР!\n";
        cout << "Вы превратили город в процветающий мегаполис! Построено зданий: " << player.builtCount << ".\n";
        cout << "Уровень счастья зашкаливает (" << game.happinessLevel << "/100).\n";
        cout << "Горожане готовы поставить вам памятник при жизни!\n";
    } else {
        cout << "👍 СТАБИЛЬНЫЙ РУКОВОДИТЕЛЬ\n";
        cout << "Вы неплохо справились. Построено зданий: " << player.builtCount << ".\n";
        cout << "Город живет стабильно, но ему еще есть куда расти.\n";
    }
    cout << "=============================================\n";

    cout << "\nВаш итоговый рейтинг: " << finalScore << "\n";
    cout << "(Формула: казна + люди*2 + счастье*5)\n";
    pause();
}

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

#ifndef UNIT_TEST
int main() {
    setlocale(LC_ALL, "Russian");
    ConfigManager config;
    int menuChoice = 0;

    while (menuChoice != 5) {
        cls();
        cout << "=====================================\n";
        cout << " ДОБРО ПОЖАЛОВАТЬ В ГОРОД МЕЧТЫ\n";
        cout << "=====================================\n";
        cout << "1. Новая игра\n";
        cout << "2. Загрузить сохранение\n";
        cout << "3. Параметры\n";
        cout << "4. Информация\n";
        cout << "5. Выход\n";
        cout << "=====================================\n";
        menuChoice = getUserInput("Выберите действие: ");

        if (menuChoice == 1) runSimulation(config.getSettings());
        else if (menuChoice == 2) {
            cls();
            cout << "=== ЗАГРУЗКА ===\n";
            cout << "Сохранения пока не поддерживаются.\n";
            pause();
        }
        else if (menuChoice == 3) configureGame(config);
        else if (menuChoice == 4) {
            cls();
            cout << "=== О ПРОЕКТЕ ===\n";
            cout << "Игра: «Город мечты»\n";
            cout << "Версия: 2.0 (ЛР3)\n";
            cout << "Автор: Фещук Артём\n";
            pause();
        }
        else if (menuChoice == 5) cout << "Завершение работы...\n";
        else { cout << "Ошибка ввода!\n"; pause(); }
    }
    return 0;
}
#endif
