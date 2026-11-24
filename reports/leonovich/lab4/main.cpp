#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <clocale>
#include <cmath>

using namespace std;

const int START = 1;
const int LOAD = 2;
const int SETTINGS = 3;
const int INFO = 4;
const int EXIT = 5;

const string DEFAULT_DIFFICULTY = "Легкий";
const int DEFAULT_ALL_PLAYERS = 4;
const int DEFAULT_HUMAN_PLAYERS = 2;
const int TOTAL_DAYS = 3;

const string SETTINGS_FILE = "settings.txt";
const string COMPUTER_NAMES_FILE = "computer_names.txt";
const string MANUAL_SAVE_FILE = "savegame.txt";
const string AUTO_SAVE_FILE = "autosave.txt";
const char SAVE_DELIMITER = '|';

string difficulty = DEFAULT_DIFFICULTY;
int allPlayers = DEFAULT_ALL_PLAYERS;
int humanPlayers = DEFAULT_HUMAN_PLAYERS;

set<string> computerNames;

struct DifficultyConfig {
    int startBudget;
    double priceMultiplier;
    int aiMaxSportsmen;
    int aiMaxEquipment;
    int aiTrainingRounds;
    int dailyBonus;
};

DifficultyConfig getDifficultyConfig(const string& diff) {
    if (diff == "Легкий") {
        return { 1600, 0.85, 2, 1, 1, 250 };
    }
    if (diff == "Сложный") {
        return { 1000, 1.10, 3, 2, 2, 180 };
    }
    return { 1200, 1.00, 3, 2, 2, 200 };
}

int scalePrice(int basePrice, double multiplier) {
    return max(50, static_cast<int>(round(basePrice * multiplier)));
}

void printLine(int width = 54, char ch = '-') {
    cout << "\t+" << string(width, ch) << "+\n";
}

int readIntInRange(const string& prompt, int minVal, int maxVal) {
    int value = 0;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minVal && value <= maxVal) {
            cin.ignore(10000, '\n');
            return value;
        }
        cout << "\t[!] Введите число от " << minVal << " до " << maxVal << ".\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
}

class Sportsman {
private:
    string name;
    int strength;
    int stamina;
    int agility;
    int intelligence;

public:
    Sportsman(string n = "Атлет", int str = 10, int stam = 10, int agi = 10, int intel = 10)
        : name(n), strength(str), stamina(stam), agility(agi), intelligence(intel) {
    }

    string getName() const { return name; }
    int getStrength() const { return strength; }
    int getStamina() const { return stamina; }
    int getAgility() const { return agility; }
    int getIntelligence() const { return intelligence; }

    void trainRunning() {
        stamina += 5;
        agility += 5;
    }

    void trainStrengthAndPower() {
        strength += 6;
        stamina += 3;
    }

    void trainSafetyAndMind() {
        intelligence += 6;
        agility += 2;
    }

    int getTotalScore() const {
        return strength + stamina + agility + intelligence;
    }

    void printInfo() const {
        cout << "\t|  * " << name
            << " [С:" << strength
            << " В:" << stamina
            << " Л:" << agility
            << " И:" << intelligence << "]\n";
    }
};

class Equipment {
private:
    string name;
    int cost;
    int statBonus;

public:
    Equipment(string n = "Стандартная экипировка", int c = 100, int bonus = 5)
        : name(n), cost(c), statBonus(bonus) {
    }

    string getName() const { return name; }
    int getCost() const { return cost; }
    int getBonus() const { return statBonus; }
};

class Captain {
private:
    string name;
    int money;
    vector<Sportsman> team;
    vector<Equipment> inventory;

public:
    Captain(string n = "Капитан", int startMoney = 1000)
        : name(n), money(startMoney) {
    }

    string getName() const { return name; }
    int getMoney() const { return money; }
    const vector<Sportsman>& getTeam() const { return team; }
    const vector<Equipment>& getInventory() const { return inventory; }

    void addMoney(int amount) {
        money += max(0, amount);
    }

    void hireSportsman(const Sportsman& s, int cost) {
        if (money >= cost) {
            team.push_back(s);
            money -= cost;
            cout << "\t[+] Нанят спортсмен: " << s.getName() << " (-" << cost << " монет)\n";
        }
        else {
            cout << "\t[!] Недостаточно средств для найма.\n";
        }
    }

    void buyEquipment(const Equipment& eq, int actualCost) {
        if (money >= actualCost) {
            inventory.push_back(eq);
            money -= actualCost;
            cout << "\t[+] Куплена экипировка: " << eq.getName() << " (-" << actualCost << " монет)\n";
        }
        else {
            cout << "\t[!] Недостаточно средств для покупки экипировки.\n";
        }
    }

    void trainTeam(int choice) {
        if (team.empty()) {
            cout << "\t[!] У капитана " << name << " нет спортсменов для тренировки.\n";
            return;
        }
        for (auto& s : team) {
            if (choice == 1) s.trainRunning();
            else if (choice == 2) s.trainStrengthAndPower();
            else if (choice == 3) s.trainSafetyAndMind();
        }
        cout << "\t[+] Команда капитана " << name << " прошла тренировку.\n";
    }

    int calculateTotalScore() const {
        int score = 0;
        for (const auto& s : team) {
            score += s.getTotalScore();
        }
        for (const auto& eq : inventory) {
            score += eq.getBonus() * 10;
        }
        return score;
    }

    void showShopCard() const {
        printLine(54, '-');
        cout << "\t| КАПИТАН: " << name << "\n";
        cout << "\t| Бюджет: " << money << " монет  |  Бойцов: " << team.size()
            << "  |  Снаряжения: " << inventory.size() << "\n";
        for (const auto& s : team) {
            s.printInfo();
        }
        printLine(54, '-');
    }
};

struct GameState {
    string diff;
    int totalPlayers;
    int humPlayers;
    int day;
    int stage;
    int playerIndex;
    bool aiPreparedForDay;
    vector<Captain> captains;
};

bool SaveGameToFile(const string& fileName, const GameState& state, bool silent = false) {
    ofstream out(fileName);
    if (!out.is_open()) {
        if (!silent) {
            cout << "\t[!] Не удалось сохранить игру.\n";
        }
        return false;
    }

    out << "META" << SAVE_DELIMITER
        << state.diff << SAVE_DELIMITER
        << state.totalPlayers << SAVE_DELIMITER
        << state.humPlayers << SAVE_DELIMITER
        << state.day << SAVE_DELIMITER
        << state.stage << SAVE_DELIMITER
        << state.playerIndex << SAVE_DELIMITER
        << (state.aiPreparedForDay ? 1 : 0) << SAVE_DELIMITER
        << state.captains.size() << "\n";

    for (const auto& cap : state.captains) {
        const auto& team = cap.getTeam();
        const auto& inv = cap.getInventory();

        out << "CAP" << SAVE_DELIMITER
            << cap.getName() << SAVE_DELIMITER
            << cap.getMoney() << SAVE_DELIMITER
            << team.size() << SAVE_DELIMITER
            << inv.size() << "\n";

        for (const auto& s : team) {
            out << "SPO" << SAVE_DELIMITER
                << s.getName() << SAVE_DELIMITER
                << s.getStrength() << SAVE_DELIMITER
                << s.getStamina() << SAVE_DELIMITER
                << s.getAgility() << SAVE_DELIMITER
                << s.getIntelligence() << "\n";
        }

        for (const auto& eq : inv) {
            out << "EQP" << SAVE_DELIMITER
                << eq.getName() << SAVE_DELIMITER
                << eq.getCost() << SAVE_DELIMITER
                << eq.getBonus() << "\n";
        }
    }

    if (!silent) {
        cout << "\t[+] Игра сохранена.\n";
    }
    return true;
}

bool LoadGameFromFile(const string& fileName, GameState& outState) {
    ifstream in(fileName);
    if (!in.is_open()) {
        return false;
    }

    string line;
    if (!getline(in, line)) {
        return false;
    }

    stringstream metaSs(line);
    string tag;
    string totalPlayersStr, humPlayersStr, dayStr, stageStr, playerIdxStr, aiPrepStr, capCountStr;

    if (!getline(metaSs, tag, SAVE_DELIMITER) || tag != "META") return false;
    if (!getline(metaSs, outState.diff, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, totalPlayersStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, humPlayersStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, dayStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, stageStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, playerIdxStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, aiPrepStr, SAVE_DELIMITER)) return false;
    if (!getline(metaSs, capCountStr, SAVE_DELIMITER)) return false;

    int capCount = 0;
    try {
        outState.totalPlayers = stoi(totalPlayersStr);
        outState.humPlayers = stoi(humPlayersStr);
        outState.day = stoi(dayStr);
        outState.stage = stoi(stageStr);
        outState.playerIndex = stoi(playerIdxStr);
        outState.aiPreparedForDay = (stoi(aiPrepStr) != 0);
        capCount = stoi(capCountStr);
    }
    catch (...) {
        return false;
    }

    outState.captains.clear();
    outState.captains.reserve(capCount);

    for (int i = 0; i < capCount; i++) {
        if (!getline(in, line)) return false;
        stringstream capSs(line);
        string capTag, capName, moneyStr, teamCountStr, invCountStr;

        if (!getline(capSs, capTag, SAVE_DELIMITER) || capTag != "CAP") return false;
        if (!getline(capSs, capName, SAVE_DELIMITER)) return false;
        if (!getline(capSs, moneyStr, SAVE_DELIMITER)) return false;
        if (!getline(capSs, teamCountStr, SAVE_DELIMITER)) return false;
        if (!getline(capSs, invCountStr, SAVE_DELIMITER)) return false;

        int money = 0, teamCount = 0, invCount = 0;
        try {
            money = stoi(moneyStr);
            teamCount = stoi(teamCountStr);
            invCount = stoi(invCountStr);
        }
        catch (...) {
            return false;
        }

        Captain cap(capName, money);

        for (int sIdx = 0; sIdx < teamCount; sIdx++) {
            if (!getline(in, line)) return false;
            stringstream spoSs(line);
            string spoTag, sName, strS, stamS, agiS, intS;

            if (!getline(spoSs, spoTag, SAVE_DELIMITER) || spoTag != "SPO") return false;
            if (!getline(spoSs, sName, SAVE_DELIMITER)) return false;
            if (!getline(spoSs, strS, SAVE_DELIMITER)) return false;
            if (!getline(spoSs, stamS, SAVE_DELIMITER)) return false;
            if (!getline(spoSs, agiS, SAVE_DELIMITER)) return false;
            if (!getline(spoSs, intS, SAVE_DELIMITER)) return false;

            try {
                Sportsman sp(sName, stoi(strS), stoi(stamS), stoi(agiS), stoi(intS));
                cap.hireSportsman(sp, 0);
            }
            catch (...) {
                return false;
            }
        }

        for (int eIdx = 0; eIdx < invCount; eIdx++) {
            if (!getline(in, line)) return false;
            stringstream eqpSs(line);
            string eqpTag, eName, costS, bonusS;

            if (!getline(eqpSs, eqpTag, SAVE_DELIMITER) || eqpTag != "EQP") return false;
            if (!getline(eqpSs, eName, SAVE_DELIMITER)) return false;
            if (!getline(eqpSs, costS, SAVE_DELIMITER)) return false;
            if (!getline(eqpSs, bonusS, SAVE_DELIMITER)) return false;

            try {
                Equipment eq(eName, stoi(costS), stoi(bonusS));
                cap.buyEquipment(eq, 0);
            }
            catch (...) {
                return false;
            }
        }

        outState.captains.push_back(cap);
    }

    return true;
}

void AutoSaveStage(const GameState& state) {
    SaveGameToFile(AUTO_SAVE_FILE, state, true);
}

void RunGameplay(GameState state, bool fromSave) {
    difficulty = state.diff;
    allPlayers = state.totalPlayers;
    humanPlayers = state.humPlayers;

    int aiPlayers = allPlayers - humanPlayers;
    DifficultyConfig cfg = getDifficultyConfig(difficulty);

    const int baseHireCost = 300;
    const int hireCost = scalePrice(baseHireCost, cfg.priceMultiplier);

    vector<Sportsman> marketSportsmen = {
        Sportsman("Бегун Алексей", 10, 15, 18, 10),
        Sportsman("Силач Роман", 20, 12, 8, 10),
        Sportsman("Многоборец Артём", 12, 14, 12, 12),
        Sportsman("Тактик Павел", 10, 10, 10, 18)
    };

    vector<Equipment> marketEquipment = {
        Equipment("Профессиональные кроссовки", 200, 8),
        Equipment("Защитный шлем и экипировка", 250, 10),
        Equipment("Усиленный тренажер", 300, 15)
    };

    if (!fromSave) {
        system("cls");
        cout << "\t\tНОВАЯ ИГРА: СИМУЛЯТОР КАПИТАНА\n\n";
        cout << "\tСложность: " << difficulty << "\n";
        cout << "\tБюджет: " << cfg.startBudget
            << " | Дней подготовки: " << TOTAL_DAYS
            << " | Команд: " << allPlayers
            << " (Игроков: " << humanPlayers << ", ИИ: " << aiPlayers << ")\n\n";

        cin.ignore(10000, '\n');

        for (int i = 0; i < humanPlayers; i++) {
            string capName;
            cout << "\tИмя капитана " << i + 1 << ": ";
            getline(cin, capName);
            if (capName.empty()) {
                capName = "Капитан " + to_string(i + 1);
            }
            state.captains.push_back(Captain(capName, cfg.startBudget));
        }

        vector<string> aiNamePool(computerNames.begin(), computerNames.end());
        for (int i = 0; i < aiPlayers; i++) {
            string aiName = "ИИ Соперник " + to_string(i + 1);
            if (!aiNamePool.empty()) {
                int idx = rand() % static_cast<int>(aiNamePool.size());
                aiName = aiNamePool[idx];
                aiNamePool.erase(aiNamePool.begin() + idx);
            }
            state.captains.push_back(Captain(aiName, cfg.startBudget));
        }

        state.day = 1;
        state.stage = 1;
        state.playerIndex = 0;
        state.aiPreparedForDay = false;
        AutoSaveStage(state);
    }

    while (state.day <= TOTAL_DAYS) {
        if (state.stage == 1) {
            for (int i = state.playerIndex; i < humanPlayers; i++) {
                state.playerIndex = i;
                while (true) {
                    system("cls");
                    cout << "\t\tДЕНЬ " << state.day << "/" << TOTAL_DAYS << " — МАГАЗИН ПОДГОТОВКИ\n\n";
                    state.captains[i].showShopCard();

                    cout << "\t[1] Нанять спортсмена (" << hireCost << " монет)\n"
                        << "\t[2] Купить экипировку\n"
                        << "\t[3] Сохранить игру\n"
                        << "\t[4] Завершить магазин\n"
                        << "\t[5] Выйти в главное меню\n";

                    int action = readIntInRange("\t> ", 1, 5);

                    if (action == 1) {
                        system("cls");
                        cout << "\t\tДЕНЬ " << state.day << " — ВИТРИНА: СПОРТСМЕНЫ\n\n";
                        state.captains[i].showShopCard();
                        string tag = (state.captains[i].getMoney() >= hireCost) ? "[OK]" : "[--]";
                        for (size_t j = 0; j < marketSportsmen.size(); j++) {
                            const auto& s = marketSportsmen[j];
                            cout << "\t" << tag << " " << (j + 1) << ". " << s.getName()
                                << " [С:" << s.getStrength()
                                << " В:" << s.getStamina()
                                << " Л:" << s.getAgility()
                                << " И:" << s.getIntelligence() << "]\n";
                        }
                        cout << "\t     0. Назад\n";
                        int sChoice = readIntInRange("\t> ", 0, static_cast<int>(marketSportsmen.size()));
                        if (sChoice > 0) {
                            state.captains[i].hireSportsman(marketSportsmen[sChoice - 1], hireCost);
                            Sleep(900);
                        }
                    }
                    else if (action == 2) {
                        system("cls");
                        cout << "\t\tДЕНЬ " << state.day << " — ВИТРИНА: ЭКИПИРОВКА\n\n";
                        state.captains[i].showShopCard();
                        for (size_t j = 0; j < marketEquipment.size(); j++) {
                            const auto& eq = marketEquipment[j];
                            int eqCost = scalePrice(eq.getCost(), cfg.priceMultiplier);
                            string tag = (state.captains[i].getMoney() >= eqCost) ? "[OK]" : "[--]";
                            cout << "\t" << tag << " " << (j + 1) << ". " << eq.getName()
                                << " | " << eqCost << " монет | +" << eq.getBonus() << "\n";
                        }
                        cout << "\t     0. Назад\n";
                        int eChoice = readIntInRange("\t> ", 0, static_cast<int>(marketEquipment.size()));
                        if (eChoice > 0) {
                            const auto& chosenEq = marketEquipment[eChoice - 1];
                            int eqCost = scalePrice(chosenEq.getCost(), cfg.priceMultiplier);
                            state.captains[i].buyEquipment(chosenEq, eqCost);
                            Sleep(900);
                        }
                    }
                    else if (action == 3) {
                        SaveGameToFile(MANUAL_SAVE_FILE, state, false);
                        Sleep(900);
                    }
                    else if (action == 4) {
                        break;
                    }
                    else if (action == 5) {
                        return;
                    }
                }
            }

            if (!state.aiPreparedForDay) {
                for (int i = humanPlayers; i < static_cast<int>(state.captains.size()); i++) {
                    int hired = 0;
                    int bought = 0;

                    while (hired < cfg.aiMaxSportsmen || bought < cfg.aiMaxEquipment) {
                        bool canHire = (hired < cfg.aiMaxSportsmen) &&
                            !marketSportsmen.empty() &&
                            (state.captains[i].getMoney() >= hireCost);

                        vector<int> affordableEqIdx;
                        if (bought < cfg.aiMaxEquipment) {
                            for (size_t j = 0; j < marketEquipment.size(); j++) {
                                int c = scalePrice(marketEquipment[j].getCost(), cfg.priceMultiplier);
                                if (state.captains[i].getMoney() >= c) {
                                    affordableEqIdx.push_back(static_cast<int>(j));
                                }
                            }
                        }
                        bool canBuyEq = !affordableEqIdx.empty();

                        if (!canHire && !canBuyEq) {
                            break;
                        }

                        if (canHire && (!canBuyEq || hired < max(1, cfg.aiMaxSportsmen - 1))) {
                            state.captains[i].hireSportsman(marketSportsmen[rand() % marketSportsmen.size()], hireCost);
                            hired++;
                        }
                        else if (canBuyEq) {
                            int idx = affordableEqIdx[rand() % affordableEqIdx.size()];
                            const auto& eq = marketEquipment[idx];
                            int eqCost = scalePrice(eq.getCost(), cfg.priceMultiplier);
                            state.captains[i].buyEquipment(eq, eqCost);
                            bought++;
                        }
                        else if (canHire) {
                            state.captains[i].hireSportsman(marketSportsmen[rand() % marketSportsmen.size()], hireCost);
                            hired++;
                        }
                    }
                }
                state.aiPreparedForDay = true;
            }

            state.stage = 2;
            state.playerIndex = 0;
            AutoSaveStage(state);
        }

        if (state.stage == 2) {
            system("cls");
            cout << "\t\tДЕНЬ " << state.day << "/" << TOTAL_DAYS << " — ТРЕНИРОВКА КОМАНД\n\n";

            for (int i = state.playerIndex; i < static_cast<int>(state.captains.size()); i++) {
                state.playerIndex = i;
                cout << "\tКоманда: " << state.captains[i].getName() << "\n";
                int trainChoice = 1;

                if (i < humanPlayers) {
                    cout << "\t  [1] Пробежка (выносливость и ловкость)\n"
                        << "\t  [2] Силовая тренировка (сила и выносливость)\n"
                        << "\t  [3] Изучение техники безопасности (сообразительность и ловкость)\n"
                        << "\t  [4] Сохранить игру\n"
                        << "\t  [5] Выйти в главное меню\n";
                    trainChoice = readIntInRange("\t> ", 1, 5);
                    if (trainChoice == 4) {
                        SaveGameToFile(MANUAL_SAVE_FILE, state, false);
                        Sleep(900);
                        i--;
                        continue;
                    }
                    if (trainChoice == 5) {
                        return;
                    }
                    state.captains[i].trainTeam(trainChoice);
                }
                else {
                    for (int r = 0; r < cfg.aiTrainingRounds; r++) {
                        cout << "\t  ИИ думает над выбором тренировки...\n";
                        Sleep(2000);
                        trainChoice = (rand() % 3) + 1;
                        cout << "\t  ИИ (раунд " << (r + 1) << ") выбрал тренировку [" << trainChoice << "]\n";
                        state.captains[i].trainTeam(trainChoice);
                        Sleep(1200);
                    }
                }
                cout << "\n";
            }

            state.stage = 3;
            state.playerIndex = 0;
            AutoSaveStage(state);
        }

        if (state.stage == 3) {
            system("cls");
            cout << "\t\tИТОГИ ДНЯ " << state.day << "/" << TOTAL_DAYS << "\n\n";

            for (auto& cap : state.captains) {
                cap.addMoney(cfg.dailyBonus);
            }
            cout << "\t[+] Каждый капитан получил дневной бонус: +" << cfg.dailyBonus << " монет.\n\n";

            struct ResultInfo {
                string name;
                int score;
            };

            vector<ResultInfo> results;
            for (const auto& cap : state.captains) {
                results.push_back({ cap.getName(), cap.calculateTotalScore() });
            }

            sort(results.begin(), results.end(), [](const ResultInfo& a, const ResultInfo& b) {
                return a.score > b.score;
                });

            printLine(54, '-');
            for (size_t i = 0; i < results.size(); i++) {
                string medal = (i == 0) ? "[#1]" : ("[" + to_string(i + 1) + " ]");
                cout << "\t| " << medal << " " << results[i].name << " — " << results[i].score << " очков\n";
            }
            printLine(54, '-');

            if (state.day < TOTAL_DAYS) {
                cout << "\n\t[1] Следующий день\n"
                    << "\t[2] Сохранить игру\n"
                    << "\t[3] Выйти в главное меню\n";
                int nextAction = readIntInRange("\t> ", 1, 3);

                if (nextAction == 2) {
                    SaveGameToFile(MANUAL_SAVE_FILE, state, false);
                    Sleep(900);
                    continue;
                }
                if (nextAction == 3) {
                    return;
                }

                state.day++;
                state.stage = 1;
                state.playerIndex = 0;
                state.aiPreparedForDay = false;
                AutoSaveStage(state);
            }
            else {
                if (!results.empty()) {
                    cout << "\n\tПобедитель соревнования: " << results[0].name << "!\n";
                }
                AutoSaveStage(state);
                cout << "\n\tНажмите Enter для возврата в меню...";
                cin.ignore(10000, '\n');
                cin.get();
                return;
            }
        }
    }
}

void Start();
void Load();
void Settings();
void Info();
void Exit();

void CreateDefaultSettingsFile();
void LoadSettings();
void SaveSettings();
void CreateComputerNamesFile();
void LoadComputerNames();

void CreateDefaultSettingsFile() {
    ifstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        ofstream newFile(SETTINGS_FILE);
        if (newFile.is_open()) {
            newFile << DEFAULT_DIFFICULTY << '\n';
            newFile << DEFAULT_ALL_PLAYERS << '\n';
            newFile << DEFAULT_HUMAN_PLAYERS << '\n';
        }
    }
}

void LoadSettings() {
    ifstream file(SETTINGS_FILE);
    if (!file.is_open()) return;

    string loadedDifficulty;
    int loadedAllPlayers;
    int loadedHumanPlayers;

    if (getline(file, loadedDifficulty) && file >> loadedAllPlayers && file >> loadedHumanPlayers) {
        if (loadedAllPlayers >= 2 && loadedAllPlayers <= 5 &&
            loadedHumanPlayers >= 1 && loadedHumanPlayers <= loadedAllPlayers &&
            (loadedDifficulty == "Легкий" || loadedDifficulty == "Средний" || loadedDifficulty == "Сложный")) {
            difficulty = loadedDifficulty;
            allPlayers = loadedAllPlayers;
            humanPlayers = loadedHumanPlayers;
        }
    }
}

void SaveSettings() {
    ofstream file(SETTINGS_FILE);
    if (file.is_open()) {
        file << difficulty << '\n';
        file << allPlayers << '\n';
        file << humanPlayers << '\n';
    }
}

void CreateComputerNamesFile() {
    ifstream file(COMPUTER_NAMES_FILE);
    if (!file.is_open()) {
        ofstream newFile(COMPUTER_NAMES_FILE);
        if (newFile.is_open()) {
            newFile << "Берсерк\nФеникс\nСкорпион\nДанте\nТерминатор\nЭлой\nГладиатор\nТайфун\nАтлант\nШторм\nЛеон\nТорнадо\nТитан\nМорбиус\nАльтер\n";
        }
    }
}

void LoadComputerNames() {
    computerNames.clear();
    ifstream file(COMPUTER_NAMES_FILE);
    string name;
    if (file.is_open()) {
        while (getline(file, name)) {
            if (!name.empty()) {
                computerNames.insert(name);
            }
        }
    }
}

void Start() {
    GameState newState;
    newState.diff = difficulty;
    newState.totalPlayers = allPlayers;
    newState.humPlayers = humanPlayers;
    newState.day = 1;
    newState.stage = 1;
    newState.playerIndex = 0;
    newState.aiPreparedForDay = false;
    RunGameplay(newState, false);
}

void Load() {
    system("cls");
    cout << "\t\tЗАГРУЗКА ИГРЫ\n\n";

    ifstream manualCheck(MANUAL_SAVE_FILE);
    bool hasManual = manualCheck.good();

    ifstream autoCheck(AUTO_SAVE_FILE);
    bool hasAuto = autoCheck.good();

    if (!hasManual && !hasAuto) {
        cout << "\tСохранений пока нет.\n";
        Sleep(800);
        cout << "\n\t\t...Нажмите Enter чтобы вернуться в главное меню\n";
        cin.ignore(10000, '\n');
        cin.get();
        return;
    }

    if (hasManual) cout << "\t[1] Сохранение\n";
    if (hasAuto) cout << "\t[2] Автосохранение\n";
    cout << "\t[0] Назад\n\n";

    int choice = readIntInRange("\tВыберите слот: ", 0, 2);
    if (choice == 0) {
        return;
    }

    string chosenFile;
    if (choice == 1 && hasManual) chosenFile = MANUAL_SAVE_FILE;
    else if (choice == 2 && hasAuto) chosenFile = AUTO_SAVE_FILE;
    else {
        cout << "\n\tВыбранный слот пуст.\n";
        Sleep(900);
        return;
    }

    if (choice == 1 && hasAuto) {
        cout << "\n\tПри прохождении этапов автосохранение будет обновлено под загруженную игру.\n";
        cout << "\t[1] Продолжить загрузку\n"
            << "\t[2] Отмена\n";
        int confirm = readIntInRange("\t> ", 1, 2);
        if (confirm == 2) {
            return;
        }
    }

    GameState loadedState;
    if (LoadGameFromFile(chosenFile, loadedState)) {
        RunGameplay(loadedState, true);
    }
    else {
        cout << "\n\t[!] Не удалось загрузить сохранение.\n";
        Sleep(1100);
    }
}

void Settings() {
    int setChoice = 0;

    do {
        system("cls");

        int aiPlayers = allPlayers - humanPlayers;
        cout << "\t\tНАСТРОЙКИ\n"
            << "\n\t1. Уровень сложности: " << difficulty
            << "\n\t2. Всего игроков: " << allPlayers
            << "\n\t3. Живых игроков: " << humanPlayers
            << "\n\t4. Игроков ИИ: " << aiPlayers
            << "\n\t5. Сбросить настройки по умолчанию"
            << "\n\t6. Вернуться в главное меню"
            << "\n\n\tВыберите пункт: ";

        cin >> setChoice;

        switch (setChoice) {
        case 1: {
            int diffChoice = 0;
            cout << "\n\tВыберите сложность:\n"
                << "\t1. Легкий\n"
                << "\t2. Средний\n"
                << "\t3. Сложный\n"
                << "\tВаш выбор: ";
            cin >> diffChoice;
            if (diffChoice == 1) difficulty = "Легкий";
            else if (diffChoice == 2) difficulty = "Средний";
            else if (diffChoice == 3) difficulty = "Сложный";
            else cout << "\tНеверный выбор!\n";

            SaveSettings();
            break;
        }
        case 2: {
            int tempAll = 0;
            cout << "\n\tВведите общее количество игроков (от 2 до 5): ";
            cin >> tempAll;
            if (tempAll >= 2 && tempAll <= 5) {
                allPlayers = tempAll;
                if (humanPlayers > allPlayers) {
                    humanPlayers = allPlayers;
                }
                SaveSettings();
            }
            else {
                cout << "\tОшибка: Число игроков должно быть от 2 до 5!\n";
                Sleep(1500);
            }
            break;
        }
        case 3: {
            int tempHuman = 0;
            cout << "\n\tВведите количество живых игроков (от 1 до " << allPlayers << "): ";
            cin >> tempHuman;
            if (tempHuman >= 1 && tempHuman <= allPlayers) {
                humanPlayers = tempHuman;
                SaveSettings();
            }
            else {
                cout << "\tОшибка: Живых игроков не может быть больше, чем всего (" << allPlayers << ")!\n";
                Sleep(1500);
            }
            break;
        }
        case 4:
            cout << "\n\tЭтот параметр считается автоматически: (Всего - Живые)\n";
            Sleep(1500);
            break;

        case 5:
            difficulty = DEFAULT_DIFFICULTY;
            allPlayers = DEFAULT_ALL_PLAYERS;
            humanPlayers = DEFAULT_HUMAN_PLAYERS;
            SaveSettings();
            cout << "\n\tНастройки сброшены к значениям по умолчанию!\n";
            Sleep(1200);
            break;

        case 6:
            break;

        default:
            cout << "\n\tНеверный пункт!\n";
            Sleep(1000);
        }
    } while (setChoice != 6);
}

void Info() {
    system("cls");
    cout << "\t\tО ПРОГРАММЕ\n\n\tВерсия программы: 3.0\n\tРазработчик: Яна Леонович\n\n\t...Нажмите Enter чтобы вернуться в главное меню\n";
    cin.ignore(10000, '\n');
    cin.get();
}

void Exit() {
    system("cls");
    cout << "\n\tЗавершение работы программы...\n";
    Sleep(1000);
}

int main() {
    setlocale(LC_ALL, "Rus");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand(static_cast<unsigned>(time(0)));

    CreateDefaultSettingsFile();
    LoadSettings();
    CreateComputerNamesFile();
    LoadComputerNames();

    int choice = 0;

    do {
        system("cls");
        cout << "\t\tСОРЕВНОВАНИЯ АРМИЙ\n\n\t"
            << START << ". Начать игру\n\t"
            << LOAD << ". Загрузить игру\n\t"
            << SETTINGS << ". Настройки\n\t"
            << INFO << ". О программе\n\t"
            << EXIT << ". Выход\n\n\tВыберите пункт: ";

        cin >> choice;

        switch (choice) {
        case START: Start(); break;
        case LOAD: Load(); break;
        case SETTINGS: Settings(); break;
        case INFO: Info(); break;
        case EXIT: Exit(); break;
        default:
            cout << "\n\tНеверный пункт\n";
            Sleep(1000);
        }
    } while (choice != EXIT);

    return 0;
}
