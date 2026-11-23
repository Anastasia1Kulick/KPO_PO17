#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <set>
#include <vector>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace std;
const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;
const int SET_DIFFICULTY = 1;
const int SET_TOTAL = 2;
const int SET_ALIVE = 3;
const int SET_AI_INFO = 4;
const int SET_BACK = 5;
const string PROGRAM_VERSION = "v5.0";
const string STUDENT_NAME = "Жук И.В.";
const string SETTINGS_FILE = "settings.txt";
const int SAVE_COOLDOWN_SECONDS = 1800;
const string DIFF_EASY = "Легкий";
const string DIFF_MEDIUM = "Средний";
const string DIFF_HARD = "Сложный";
struct GameSettings {
    string difficulty = DIFF_EASY;
    int totalPlayers = 2;
    int alivePlayers = 2;
    int getAICount() const {
        int ai = totalPlayers - alivePlayers;
        return (ai < 0) ? 0 : ai;
    }
};
enum class AI_NAMES {
    IVAN, KIRILL, DANIIL, ANATOLY,
    ANDREY, ALEXANDER, VIOLETTA, GLEB
};
class Equipment {
public:
    string name;
    int price;
    int bonusStrength;
    int bonusEndurance;
    int bonusAgility;
    int bonusIntellect;
    Equipment(string n, int p, int s, int e, int a, int i);
    void printInfo() const;
};
class Sportsman {
public:
    string name;
    int strength;
    int endurance;
    int agility;
    int intellect;
    vector<Equipment> gear;
    Sportsman(string n, int baseStat);
    int getTotalStrength() const;
    int getTotalEndurance() const;
    int getTotalAgility() const;
    int getTotalIntellect() const;
    void printStats() const;
};
class Captain {
public:
    string name;
    int money;
    vector<Sportsman> team;
    vector<Equipment> shop;
    Captain(string n, int startMoney);
    void buyEquipment();
    void hireSportsman(string name, int baseStat);
    void trainTeam();
};
GameSettings settings;
vector<Captain> players;
bool isGameLoaded = false;
string savegameFile = "savegame.txt";
time_t lastSaveTime = 0;
void showMainMenu();
void startGame();
void loadGame();
void showSettings();
void showAbout();
int  getIntInput(int min, int max);
void waitForRepeatInput(int expected);
void saveSettings();
bool loadSettings();
void ensureSettingsFile();
string getDifficultyInput();
string aiNameToString(AI_NAMES n);
set<string> buildAINamesSet();
void prepareAndPlay();
bool saveGame();
bool loadSavedGame();
Equipment::Equipment(string n, int p, int s, int e, int a, int i)
    : name(n), price(p), bonusStrength(s), bonusEndurance(e), bonusAgility(a), bonusIntellect(i) {
}
void Equipment::printInfo() const {
    cout << name << " (Цена: " << price << ") | +СИЛ:" << bonusStrength
        << " +ВЫН:" << bonusEndurance << " +ЛОВ:" << bonusAgility
        << " +ИНТ:" << bonusIntellect << "\n";
}
Sportsman::Sportsman(string n, int baseStat)
    : name(n), strength(baseStat), endurance(baseStat), agility(baseStat), intellect(baseStat) {
}
int Sportsman::getTotalStrength() const {
    int total = strength;
    for (const auto& e : gear) total += e.bonusStrength;
    return total;
}
int Sportsman::getTotalEndurance() const {
    int total = endurance;
    for (const auto& e : gear) total += e.bonusEndurance;
    return total;
}
int Sportsman::getTotalAgility() const {
    int total = agility;
    for (const auto& e : gear) total += e.bonusAgility;
    return total;
}
int Sportsman::getTotalIntellect() const {
    int total = intellect;
    for (const auto& e : gear) total += e.bonusIntellect;
    return total;
}
void Sportsman::printStats() const {
    cout << name << ": СИЛ " << getTotalStrength() << " | ВЫН " << getTotalEndurance()
        << " | ЛОВ " << getTotalAgility() << " | ИНТ " << getTotalIntellect() << "\n";
}
Captain::Captain(string n, int startMoney) : name(n), money(startMoney) {
    shop.push_back(Equipment("Кроссовки", 130, 1, 2, 3, 0));
    shop.push_back(Equipment("Штанга", 115, 4, 1, 0, 0));
    shop.push_back(Equipment("Учебник тактики", 110, 0, 0, 1, 4));
    shop.push_back(Equipment("Скакалка", 85, 0, 2, 2, 0));
}
void Captain::buyEquipment() {
    if (team.empty()) {
        cout << "Сначала наймите спортсменов!\n";
        return;
    }
    cout << "Выберите спортсмена (0-" << team.size() - 1 << "): ";
    int sIdx = getIntInput(0, team.size() - 1);
    while (true) {
        cout << "\n--- МАГАЗИН ---\n";
        for (size_t i = 0; i < shop.size(); ++i) {
            cout << "[" << i + 1 << "] ";
            shop[i].printInfo();
        }
        cout << "[" << shop.size() + 1 << "] Создать свою экипировку (33$ за 1 стат)\n";
        cout << "[0] Назад\n";
        cout << "Что купить? ";
        int choice;
        cin >> choice;
        if (choice == 0) break;
        if (choice == (int)shop.size() + 1) {
            string name; int str, end, agi, intel;
            cout << "\n--- СОЗДАНИЕ ЭКИПИРОВКИ ---\n";
            cout << "Название: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); getline(cin, name);
            cout << "Бонус к Силе: "; cin >> str;
            cout << "Бонус к Выносливости: "; cin >> end;
            cout << "Бонус к Ловкости: "; cin >> agi;
            cout << "Бонус к Интеллекту: "; cin >> intel;
            int p = (str + end + agi + intel) * 33;
            cout << "Цена экипировки рассчитана автоматически: " << p << "$\n";
            shop.push_back(Equipment(name, p, str, end, agi, intel));
            cout << "Экипировка '" << name << "' добавлена в магазин!\n";
            continue;
        }
        int idx = choice - 1;
        if (idx >= 0 && idx < (int)shop.size()) {
            Equipment& item = shop[idx];
            if (money < item.price) {
                cout << "Недостаточно денег! Нужно " << item.price << ", у вас " << money << "\n";
            }
            else {
                money -= item.price;
                team[sIdx].gear.push_back(item);
                cout << "Куплено: " << item.name << " для " << team[sIdx].name << "\n";
            }
        }
    }
}
void Captain::hireSportsman(string name, int baseStat) {
    team.push_back(Sportsman(name, baseStat));
    cout << "Нанят спортсмен: " << name << "\n";
}
void Captain::trainTeam() {
    if (team.empty()) {
        cout << "Сначала наймите спортсменов!\n";
        return;
    }
    cout << "\nКого тренируем?\n";
    for (size_t i = 0; i < team.size(); ++i) {
        cout << "[" << i << "] ";
        team[i].printStats();
    }
    cout << "Выберите спортсмена (0-" << team.size() - 1 << "): ";
    int sIdx = getIntInput(0, team.size() - 1);
    Sportsman& target = team[sIdx];
    while (true) {
        cout << "\n--- ТРЕНИРОВКИ ДЛЯ " << target.name << " ---\n";
        cout << "1. Пробежка (+2 Ловкость, +2 Выносливость) - 100$\n";
        cout << "2. Силовая (+3 Сила) - 75$\n";
        cout << "3. Изучение техники (+2 Интеллект) - 50$\n";
        cout << "4. Создать свою тренировку (33$ за 1 стат)\n";
        cout << "0. Назад\n";
        cout << "Выберите тренировку: ";
        int choice;
        cin >> choice;
        if (choice == 0) break;
        if (choice >= 1 && choice <= 3) {
            int cost = 0;
            if (choice == 1) cost = 100;
            else if (choice == 2) cost = 75;
            else if (choice == 3) cost = 50;
            if (money >= cost) {
                money -= cost;
                if (choice == 1) { target.agility += 2; target.endurance += 2; }
                else if (choice == 2) { target.strength += 3; }
                else if (choice == 3) { target.intellect += 2; }
                cout << target.name << " потренировался! Характеристики выросли.\n";
                target.printStats();
            }
            else {
                cout << "Недостаточно денег! Нужно " << cost << "$, у вас " << money << "$\n";
            }
        }
        else if (choice == 4) {
            cout << "\n--- СОЗДАНИЕ ТРЕНИРОВКИ ---\n";
            cout << "Название тренировки: "; cin.ignore(numeric_limits<streamsize>::max(), '\n'); string tName; getline(cin, tName);
            cout << "Сколько добавить к Силе? "; int str; cin >> str;
            cout << "Сколько добавить к Выносливости? "; int end; cin >> end;
            cout << "Сколько добавить к Ловкости? "; int agi; cin >> agi;
            cout << "Сколько добавить к Интеллекту? "; int intel; cin >> intel;
            int totalStats = str + end + agi + intel;
            int cost = totalStats * 33;
            cout << "Итого статов: " << totalStats << ". Стоимость тренировки: " << cost << "$\n";
            if (money >= cost) {
                money -= cost;
                target.strength += str;
                target.endurance += end;
                target.agility += agi;
                target.intellect += intel;
                cout << "Тренировка '" << tName << "' проведена!\n";
                target.printStats();
            }
            else {
                cout << "Недостаточно денег! Нужно " << cost << "$, у вас " << money << "$\n";
            }
        }
    }
}
bool saveGame() {
    time_t now = time(nullptr);
    if (lastSaveTime != 0 && difftime(now, lastSaveTime) < SAVE_COOLDOWN_SECONDS) {
        int remaining = SAVE_COOLDOWN_SECONDS - (int)difftime(now, lastSaveTime);
        cout << "\nСохранение недоступно! Подождите ещё "
            << remaining / 60 << " мин. " << remaining % 60 << " сек.\n";
        return false;
    }
    ofstream f(savegameFile);
    if (!f.is_open()) return false;
    f << settings.difficulty << ";" << settings.totalPlayers << ";" << settings.alivePlayers << ";" << now << "\n";
    f << players.size() << "\n";
    for (const auto& p : players) {
        f << p.name << ";" << p.money << ";" << p.team.size() << ";" << p.shop.size() << "\n";
        for (const auto& s : p.team) {
            f << s.name << ";" << s.strength << ";" << s.endurance << ";" << s.agility << ";" << s.intellect << ";"
                << s.gear.size() << ";";
            for (const auto& e : s.gear) {
                f << e.name << ";" << e.price << ";" << e.bonusStrength << ";" << e.bonusEndurance << ";"
                    << e.bonusAgility << ";" << e.bonusIntellect << ";";
            }
            f << "\n";
        }
        for (const auto& e : p.shop) {
            f << e.name << ";" << e.price << ";" << e.bonusStrength << ";" << e.bonusEndurance << ";"
                << e.bonusAgility << ";" << e.bonusIntellect << ";\n";
        }
    }
    lastSaveTime = now;
    cout << "\nИгра успешно сохранена в файл!\n";
    return true;
}
bool loadSavedGame() {
    ifstream f(savegameFile);
    if (!f.is_open()) return false;
    string line, item;
    getline(f, line);
    stringstream ss(line);
    getline(ss, settings.difficulty, ';');
    getline(ss, item, ';'); settings.totalPlayers = stoi(item);
    getline(ss, item, ';'); settings.alivePlayers = stoi(item);
    getline(ss, item, ';'); lastSaveTime = stoi(item);
    getline(f, line);
    int numPlayers = stoi(line);
    players.clear();
    for (int i = 0; i < numPlayers; ++i) {
        getline(f, line);
        stringstream pss(line);
        string pName; int pMoney, teamSize, shopSize;
        getline(pss, pName, ';');
        getline(pss, item, ';'); pMoney = stoi(item);
        getline(pss, item, ';'); teamSize = stoi(item);
        getline(pss, item, ';'); shopSize = stoi(item);
        Captain p(pName, pMoney);
        p.team.clear(); p.shop.clear();
        for (int j = 0; j < teamSize; ++j) {
            getline(f, line);
            stringstream tss(line);
            string sName; int str, end, agi, intel, gearCount;
            getline(tss, sName, ';');
            getline(tss, item, ';'); str = stoi(item);
            getline(tss, item, ';'); end = stoi(item);
            getline(tss, item, ';'); agi = stoi(item);
            getline(tss, item, ';'); intel = stoi(item);
            getline(tss, item, ';'); gearCount = stoi(item);
            Sportsman s(sName, str);
            s.endurance = end; s.agility = agi; s.intellect = intel;
            for (int k = 0; k < gearCount; ++k) {
                string eName; int price, bs, be, ba, bi;
                getline(tss, eName, ';');
                getline(tss, item, ';'); price = stoi(item);
                getline(tss, item, ';'); bs = stoi(item);
                getline(tss, item, ';'); be = stoi(item);
                getline(tss, item, ';'); ba = stoi(item);
                getline(tss, item, ';'); bi = stoi(item);
                s.gear.push_back(Equipment(eName, price, bs, be, ba, bi));
            }
            p.team.push_back(s);
        }
        for (int j = 0; j < shopSize; ++j) {
            getline(f, line);
            stringstream sss(line);
            string eName; int price, bs, be, ba, bi;
            getline(sss, eName, ';');
            getline(sss, item, ';'); price = stoi(item);
            getline(sss, item, ';'); bs = stoi(item);
            getline(sss, item, ';'); be = stoi(item);
            getline(sss, item, ';'); ba = stoi(item);
            getline(sss, item, ';'); bi = stoi(item);
            p.shop.push_back(Equipment(eName, price, bs, be, ba, bi));
        }
        players.push_back(p);
    }
    return true;
}
void prepareAndPlay() {
    for (int i = 0; i < settings.alivePlayers; ++i) {
        Captain& c = players[i];
        int choice = 0;
        while (true) {
            cout << "\n=== ПОДГОТОВКА ИГРОКА: " << c.name << " ===\n";
            cout << "1. Нанять спортсмена (100$)\n";
            cout << "2. Купить экипировку\n";
            cout << "3. Провести тренировку\n";
            if (i == settings.alivePlayers - 1) {
                cout << "4. НАЧАТЬ СОРЕВНОВАНИЕ (Все готовы)\n";
            }
            else {
                cout << "4. ЗАКОНЧИТЬ ПОДГОТОВКУ (Передать ход)\n";
            }
            cout << "5. Сохранить игру\n";
            cout << "6. Выйти в главное меню\n";
            cout << "Ваш бюджет: " << c.money << "\n";
            cout << "Команда (" << c.team.size() << " чел.):\n";
            for (size_t k = 0; k < c.team.size(); ++k) {
                cout << "  [" << k << "] ";
                c.team[k].printStats();
            }
            cout << "Выбор: ";
            cin >> choice;
            if (choice == 1) {
                if (c.money >= 100) {
                    c.money -= 100;
                    c.hireSportsman("Спортсмен", 1);
                }
                else cout << "Не хватает денег на найм!\n";
            }
            else if (choice == 2) c.buyEquipment();
            else if (choice == 3) c.trainTeam();
            else if (choice == 5) saveGame();
            else if (choice == 6) return;
            else if (choice == 4) break;
        }
    }
    cout << "\n--- СОРЕВНОВАНИЕ ---\n";
    struct Competitor {
        string name;
        bool isAI;
        vector<Sportsman> team;
        vector<bool> used;
        int totalScore = 0;
    };
    vector<Competitor> comps;
    for (const auto& p : players) {
        comps.push_back({ p.name, false, p.team, vector<bool>(p.team.size(), false), 0 });
    }
    int aiMin = 2, aiMax = 6;
    if (settings.difficulty == DIFF_EASY) { aiMin = 1; aiMax = 4; }
    else if (settings.difficulty == DIFF_HARD) { aiMin = 5; aiMax = 8; }
    set<string> aiNames = buildAINamesSet();
    auto it = aiNames.begin();
    for (int i = 0; i < settings.getAICount() && it != aiNames.end(); ++i, ++it) {
        vector<Sportsman> aiTeam;
        for (int j = 0; j < 3; ++j) aiTeam.push_back(Sportsman("Спортсмен", aiMin + rand() % (aiMax - aiMin + 1)));
        comps.push_back({ *it, true, aiTeam, vector<bool>(3, false), 0 });
    }
    for (int r = 1; r <= 3; ++r) {
        cout << "\n--- РАУНД " << r << " ---\n";
        int statType = rand() % 4;
        string statName = (statType == 0) ? "СИЛА" : (statType == 1) ? "ВЫНОСЛИВОСТЬ" : (statType == 2) ? "ЛОВКОСТЬ" : "ИНТЕЛЛЕКТ";
        cout << "Конкурс на: " << statName << "!\n";
        for (auto& p : comps) {
            if (p.team.empty()) continue;
            bool anyAvailable = false;
            for (bool u : p.used) if (!u) anyAvailable = true;
            if (!anyAvailable) {
                cout << p.name << " (нет доступных спортсменов)\n";
                continue;
            }
            int chosenIdx = -1;
            if (!p.isAI) {
                cout << "\nХод игрока: " << p.name << "\n";
                for (size_t k = 0; k < p.team.size(); ++k) {
                    cout << "[" << k << "] " << p.team[k].name << " ";
                    if (statType == 0) cout << "СИЛ:" << p.team[k].getTotalStrength();
                    else if (statType == 1) cout << "ВЫН:" << p.team[k].getTotalEndurance();
                    else if (statType == 2) cout << "ЛОВ:" << p.team[k].getTotalAgility();
                    else cout << "ИНТ:" << p.team[k].getTotalIntellect();
                    if (p.used[k]) cout << " (УЖЕ ВЫСТУПАЛ)";
                    cout << "\n";
                }
                while (true) {
                    cout << "Выберите спортсмена: ";
                    if (!(cin >> chosenIdx)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Ошибка ввода. Введите номер: ";
                        continue;
                    }
                    if (chosenIdx >= 0 && chosenIdx < (int)p.team.size()) {
                        if (p.used[chosenIdx]) {
                            cout << "Этот спортсмен уже выступал! Выберите другого.\n";
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        cout << "Неверный номер! Выберите от 0 до " << p.team.size() - 1 << ": ";
                    }
                }
            }
            else {
                do {
                    chosenIdx = rand() % p.team.size();
                } while (p.used[chosenIdx]);
                cout << p.name << " (ИИ) выбирает спортсмена...\n";
            }
            p.used[chosenIdx] = true;
            int statValue = 0;
            if (statType == 0) statValue = p.team[chosenIdx].getTotalStrength();
            else if (statType == 1) statValue = p.team[chosenIdx].getTotalEndurance();
            else if (statType == 2) statValue = p.team[chosenIdx].getTotalAgility();
            else statValue = p.team[chosenIdx].getTotalIntellect();
            int roll = rand() % 10;
            int power = statValue + roll;
            p.totalScore += power;
            cout << p.name << " выставляет " << p.team[chosenIdx].name
                << " (Стат: " << statValue << " + Рандом: " << roll << ") и получает " << power << " очков!\n";
        }
    }
    cout << "\n--- ИТОГИ СОРЕВНОВАНИЯ ---\n";
    sort(comps.begin(), comps.end(), [](const Competitor& a, const Competitor& b) {
        return a.totalScore > b.totalScore;
        });
    for (const auto& p : comps) cout << p.name << ": " << p.totalScore << " очков\n";
    cout << "\nПобедитель: " << comps[0].name << "!\n";
}
string aiNameToString(AI_NAMES n) {
    switch (n) {
    case AI_NAMES::IVAN:      return "Иван";
    case AI_NAMES::KIRILL:    return "Кирилл";
    case AI_NAMES::DANIIL:    return "Даниил";
    case AI_NAMES::ANATOLY:   return "Анатолий";
    case AI_NAMES::ANDREY:    return "Андрей";
    case AI_NAMES::ALEXANDER: return "Александр";
    case AI_NAMES::VIOLETTA:  return "Виолетта";
    case AI_NAMES::GLEB:      return "Глеб";
    }
    return "Неизвестный";
}
set<string> buildAINamesSet() {
    set<string> names;
    AI_NAMES all[] = { AI_NAMES::IVAN, AI_NAMES::KIRILL, AI_NAMES::DANIIL, AI_NAMES::ANATOLY, AI_NAMES::ANDREY, AI_NAMES::ALEXANDER, AI_NAMES::VIOLETTA, AI_NAMES::GLEB };
    for (auto n : all) names.insert(aiNameToString(n));
    return names;
}
void saveSettings() {
    ofstream f(SETTINGS_FILE);
    if (!f.is_open()) return;
    f << "difficulty=" << settings.difficulty << "\n";
    f << "totalPlayers=" << settings.totalPlayers << "\n";
    f << "alivePlayers=" << settings.alivePlayers << "\n";
}
bool loadSettings() {
    ifstream f(SETTINGS_FILE);
    if (!f.is_open()) return false;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto pos = line.find('=');
        if (pos == string::npos) continue;
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        if (key == "difficulty") settings.difficulty = value;
        else if (key == "totalPlayers") { try { settings.totalPlayers = stoi(value); } catch (...) {} }
        else if (key == "alivePlayers") { try { settings.alivePlayers = stoi(value); } catch (...) {} }
    }
    if (settings.alivePlayers > settings.totalPlayers) settings.alivePlayers = settings.totalPlayers;
    return true;
}
void ensureSettingsFile() {
    ifstream check(SETTINGS_FILE);
    if (!check.is_open()) saveSettings();
    else { check.close(); loadSettings(); }
}
void startGame() {
    system("cls");
    cout << "           НАЧАЛО ИГРЫ\n";
    if (!isGameLoaded) {
        players.clear();
        for (int i = 0; i < settings.alivePlayers; ++i) {
            string pName;
            cout << "Введите имя Игрока " << i + 1 << ": ";
            getline(cin, pName);
            if (pName.empty()) pName = "Игрок " + to_string(i + 1);
            int startMoney = 500;
            Captain c(pName, startMoney);
            for (int j = 0; j < 3; ++j) {
                c.team.push_back(Sportsman("Новичок " + to_string(j + 1), 1));
            }
            players.push_back(c);
        }
        isGameLoaded = true;
    }
    prepareAndPlay();
    waitForRepeatInput(MENU_START_GAME);
}
void loadGame() {
    system("cls");
    if (loadSavedGame()) {
        isGameLoaded = true;
        cout << "Игра успешно загружена!\n";
        prepareAndPlay();
    }
    else {
        cout << "Сохранение не найдено.\n";
    }
    waitForRepeatInput(MENU_LOAD_GAME);
}
#ifndef TESTS
int main() {
    setlocale(LC_ALL, "Russian");
#ifdef _WIN32
    system("chcp 1251 > nul");
#endif
    srand(time(NULL));
    ensureSettingsFile();
    int choice = 0;
    do {
        showMainMenu();
        choice = getIntInput(MENU_START_GAME, MENU_EXIT);
        switch (choice) {
        case MENU_START_GAME: startGame(); break;
        case MENU_LOAD_GAME:  loadGame();  break;
        case MENU_SETTINGS:   showSettings(); break;
        case MENU_ABOUT:      showAbout(); break;
        case MENU_EXIT:
            cout << "\nВыход из программы. До свидания!\n";
            break;
        }
    } while (choice != MENU_EXIT);
    return 0;
}
#endif
void showMainMenu() {
    system("cls");
    cout << "        СОРЕВНОВАНИЕ АРМИЙ\n";
    cout << "  " << MENU_START_GAME << ". Начать игру\n";
    cout << "  " << MENU_LOAD_GAME << ". Загрузить игру\n";
    cout << "  " << MENU_SETTINGS << ". Настройки\n";
    cout << "  " << MENU_ABOUT << ". О программе\n";
    cout << "  " << MENU_EXIT << ". Выход\n";
    cout << "Выберите пункт меню: ";
}
void showSettings() {
    int choice = 0;
    do {
        system("cls");
        cout << "              НАСТРОЙКИ\n";
        cout << "  " << SET_DIFFICULTY << ". Уровень сложности: " << settings.difficulty << "\n";
        cout << "  " << SET_TOTAL << ". Всего игроков:     " << settings.totalPlayers << "\n";
        cout << "  " << SET_ALIVE << ". Живых игроков:     " << settings.alivePlayers << "\n";
        cout << "  " << SET_AI_INFO << ". ИИ-игроков:        " << settings.getAICount() << " (автоматически)\n";
        cout << "  " << SET_BACK << ". Назад в главное меню\n";
        cout << "Выберите пункт: ";
        choice = getIntInput(1, 5);
        switch (choice) {
        case SET_DIFFICULTY: {
            system("cls");
            cout << "       УРОВЕНЬ СЛОЖНОСТИ\n";
            cout << "  1 - " << DIFF_EASY << "\n";
            cout << "  2 - " << DIFF_MEDIUM << "\n";
            cout << "  3 - " << DIFF_HARD << "\n";
            cout << "Введите уровень сложности: ";
            settings.difficulty = getDifficultyInput();
            saveSettings();
            cout << "\nУровень сложности сохранён в файл!\n";
            waitForRepeatInput(SET_DIFFICULTY);
            break;
        }
        case SET_TOTAL: {
            system("cls");
            cout << "        ВСЕГО ИГРОКОВ\n";
            cout << "Введите общее количество игроков: ";
            settings.totalPlayers = getIntInput(2, 100);
            if (settings.alivePlayers > settings.totalPlayers)
                settings.alivePlayers = settings.totalPlayers;
            saveSettings();
            cout << "\nВсего игроков сохранено в файл!\n";
            waitForRepeatInput(SET_TOTAL);
            break;
        }
        case SET_ALIVE: {
            system("cls");
            cout << "        ЖИВЫХ ИГРОКОВ\n";
            cout << "Введите количество живых игроков: ";
            settings.alivePlayers = getIntInput(1, settings.totalPlayers);
            saveSettings();
            cout << "\nКоличество живых игроков сохранено в файл!\n";
            waitForRepeatInput(SET_ALIVE);
            break;
        }
        case SET_AI_INFO: {
            system("cls");
            cout << "          ИИ-ИГРОКИ\n";
            cout << "  Формула расчёта:\n";
            cout << "    ИИ = все игроки - живые игроки\n";
            cout << "  Всего игроков:  " << settings.totalPlayers << "\n";
            cout << "  Живых игроков:  " << settings.alivePlayers << "\n";
            cout << "  ИИ-игроков:     " << settings.getAICount() << "\n";
            set<string> names = buildAINamesSet();
            cout << "  Возможные имена ИИ (всего " << names.size() << "):\n";
            for (const auto& n : names)
                cout << "    - " << n << "\n";
            waitForRepeatInput(SET_AI_INFO);
            break;
        }
        case SET_BACK:
            break;
        }
    } while (choice != SET_BACK);
}
void showAbout() {
    system("cls");
    cout << "            О ПРОГРАММЕ\n";
    cout << "  Версия:  " << PROGRAM_VERSION << "\n";
    cout << "  Студент: " << STUDENT_NAME << "\n";
    waitForRepeatInput(MENU_ABOUT);
}
int getIntInput(int min, int max) {
    int value;
    while (true) {
        cin >> value;
        if (cin.fail() || value < min || value > max) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод. Попробуйте снова: ";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}
string getDifficultyInput() {
    int v;
    while (true) {
        cin >> v;
        if (cin.fail() || v < 1 || v > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод. Попробуйте снова: ";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (v == 1) return DIFF_EASY;
            if (v == 2) return DIFF_MEDIUM;
            return DIFF_HARD;
        }
    }
}
void waitForRepeatInput(int expected) {
    cout << "\nДля продолжения\n";
    cout << "повторно введите " << expected << ": ";
    int value;
    while (true) {
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Некорректный ввод. Повторите: ";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (value == expected) {
            return;
        }
        else {
            cout << "Неверно. Введите " << expected << " для продолжения: ";
        }
    }
}