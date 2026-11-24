#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
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

const string SETTINGS_FILE = "settings.txt";
const string COMPUTER_NAMES_FILE = "computer_names.txt";

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
};

DifficultyConfig getDifficultyConfig(const string& diff) {
    if (diff == "Легкий") {
        return { 1600, 0.85, 2, 1, 1 };
    }
    if (diff == "Сложный") {
        return { 900, 1.15, 4, 3, 3 };
    }
    return { 1200, 1.00, 3, 2, 2 };
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
    system("cls");
    int aiPlayers = allPlayers - humanPlayers;
    DifficultyConfig cfg = getDifficultyConfig(difficulty);

    cout << "\t\tНОВАЯ ИГРА: СИМУЛЯТОР КАПИТАНА\n\n";
    cout << "\tСложность: " << difficulty << "\n";
    cout << "\tБюджет: " << cfg.startBudget
        << " | Команд: " << allPlayers
        << " (Игроков: " << humanPlayers << ", ИИ: " << aiPlayers << ")\n\n";

    cin.ignore(10000, '\n');

    vector<Captain> captains;
    for (int i = 0; i < humanPlayers; i++) {
        string capName;
        cout << "\tИмя капитана " << i + 1 << ": ";
        getline(cin, capName);
        if (capName.empty()) {
            capName = "Капитан " + to_string(i + 1);
        }
        captains.push_back(Captain(capName, cfg.startBudget));
    }

    vector<string> aiNamePool(computerNames.begin(), computerNames.end());
    for (int i = 0; i < aiPlayers; i++) {
        string aiName = "ИИ Соперник " + to_string(i + 1);
        if (!aiNamePool.empty()) {
            int idx = rand() % static_cast<int>(aiNamePool.size());
            aiName = aiNamePool[idx];
            aiNamePool.erase(aiNamePool.begin() + idx);
        }
        captains.push_back(Captain(aiName, cfg.startBudget));
    }

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

    for (int i = 0; i < humanPlayers; i++) {
        while (true) {
            system("cls");
            cout << "\t\tМАГАЗИН ПОДГОТОВКИ\n\n";
            captains[i].showShopCard();

            cout << "\t[1] Нанять спортсмена (" << hireCost << " монет)\n"
                << "\t[2] Купить экипировку\n"
                << "\t[3] Выйти из магазина\n";

            int action = readIntInRange("\t> ", 1, 3);

            if (action == 1) {
                system("cls");
                cout << "\t\tВИТРИНА: СПОРТСМЕНЫ\n\n";
                captains[i].showShopCard();
                string tag = (captains[i].getMoney() >= hireCost) ? "[OK]" : "[--]";
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
                    captains[i].hireSportsman(marketSportsmen[sChoice - 1], hireCost);
                    Sleep(900);
                }
            }
            else if (action == 2) {
                system("cls");
                cout << "\t\tВИТРИНА: ЭКИПИРОВКА\n\n";
                captains[i].showShopCard();
                for (size_t j = 0; j < marketEquipment.size(); j++) {
                    const auto& eq = marketEquipment[j];
                    int eqCost = scalePrice(eq.getCost(), cfg.priceMultiplier);
                    string tag = (captains[i].getMoney() >= eqCost) ? "[OK]" : "[--]";
                    cout << "\t" << tag << " " << (j + 1) << ". " << eq.getName()
                        << " | " << eqCost << " монет | +" << eq.getBonus() << "\n";
                }
                cout << "\t     0. Назад\n";
                int eChoice = readIntInRange("\t> ", 0, static_cast<int>(marketEquipment.size()));
                if (eChoice > 0) {
                    const auto& chosenEq = marketEquipment[eChoice - 1];
                    int eqCost = scalePrice(chosenEq.getCost(), cfg.priceMultiplier);
                    captains[i].buyEquipment(chosenEq, eqCost);
                    Sleep(900);
                }
            }
            else {
                break;
            }
        }
    }

    for (int i = humanPlayers; i < static_cast<int>(captains.size()); i++) {
        int hired = 0;
        int bought = 0;

        while (hired < cfg.aiMaxSportsmen || bought < cfg.aiMaxEquipment) {
            bool canHire = (hired < cfg.aiMaxSportsmen) &&
                !marketSportsmen.empty() &&
                (captains[i].getMoney() >= hireCost);

            vector<int> affordableEqIdx;
            if (bought < cfg.aiMaxEquipment) {
                for (size_t j = 0; j < marketEquipment.size(); j++) {
                    int c = scalePrice(marketEquipment[j].getCost(), cfg.priceMultiplier);
                    if (captains[i].getMoney() >= c) {
                        affordableEqIdx.push_back(static_cast<int>(j));
                    }
                }
            }
            bool canBuyEq = !affordableEqIdx.empty();

            if (!canHire && !canBuyEq) {
                break;
            }

            if (canHire && (!canBuyEq || hired < max(1, cfg.aiMaxSportsmen - 1))) {
                captains[i].hireSportsman(marketSportsmen[rand() % marketSportsmen.size()], hireCost);
                hired++;
            }
            else if (canBuyEq) {
                int idx = affordableEqIdx[rand() % affordableEqIdx.size()];
                const auto& eq = marketEquipment[idx];
                int eqCost = scalePrice(eq.getCost(), cfg.priceMultiplier);
                captains[i].buyEquipment(eq, eqCost);
                bought++;
            }
            else if (canHire) {
                captains[i].hireSportsman(marketSportsmen[rand() % marketSportsmen.size()], hireCost);
                hired++;
            }
        }
    }

    system("cls");
    cout << "\t\tЭТАП 2: ТРЕНИРОВКА КОМАНД\n\n";

    for (int i = 0; i < static_cast<int>(captains.size()); i++) {
        cout << "\tКоманда: " << captains[i].getName() << "\n";
        int trainChoice = 1;

        if (i < humanPlayers) {
            cout << "\t  [1] Пробежка (выносливость и ловкость)\n"
                << "\t  [2] Силовая тренировка (сила и выносливость)\n"
                << "\t  [3] Изучение техники безопасности (сообразительность и ловкость)\n";
            trainChoice = readIntInRange("\t> ", 1, 3);
            captains[i].trainTeam(trainChoice);
        }
        else {
            for (int r = 0; r < cfg.aiTrainingRounds; r++) {
                cout << "\t  ИИ думает над выбором тренировки...\n";
                Sleep(2000);
                trainChoice = (rand() % 3) + 1;
                cout << "\t  ИИ (раунд " << (r + 1) << ") выбрал тренировку [" << trainChoice << "]\n";
                captains[i].trainTeam(trainChoice);
                Sleep(1200);
            }
        }
        cout << "\n";
    }

    Sleep(1000);

    system("cls");
    cout << "\t\tЭТАП 3: ИТОГИ СОРЕВНОВАНИЯ\n\n";

    struct ResultInfo {
        string name;
        int score;
    };

    vector<ResultInfo> results;
    for (const auto& cap : captains) {
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

    if (!results.empty()) {
        cout << "\n\tПобедитель: " << results[0].name << "!\n";
    }

    cout << "\n\tНажмите Enter для возврата в меню...";
    cin.get();
}

void Load() {
    system("cls");
    cout << "\tНа данный момент функция недоступна\n";
    Sleep(500);
    cout << "\n\t\t...Нажмите Enter чтобы вернуться в главное меню\n";
    cin.ignore(10000, '\n');
    cin.get();
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
    cout << "\t\tО ПРОГРАММЕ\n\n\tВерсия программы: 2.0\n\tРазработчик: Яна Леонович\n\n\t...Нажмите Enter чтобы вернуться в главное меню\n";
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
