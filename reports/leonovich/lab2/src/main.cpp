#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <set>
#include <cstdlib>
#include <ctime>
#include <clocale>

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
            newFile << "Берсерк\n";
            newFile << "Феникс\n";
            newFile << "Скорпион\n";
            newFile << "Данте\n";
            newFile << "Терминатор\n";
            newFile << "Элой\n";
            newFile << "Гладиатор\n";
            newFile << "Тайфун\n";
            newFile << "Атлант\n";
            newFile << "Шторм\n";
            newFile << "Леон\n";
            newFile << "Торнадо\n";
            newFile << "Титан\n";
            newFile << "Морбиус\n";
            newFile << "Альтер\n";
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
    cout << "\tНачало игры\n";
    cout << "\n\tУровень сложности: " << difficulty;
    cout << "\n\tВсего игроков: " << allPlayers;
    cout << "\n\tЖивых игроков: " << humanPlayers;
    int aiPlayers = allPlayers - humanPlayers;
    cout << "\n\tИгроков ИИ: " << aiPlayers << "\n\n";

    cin.ignore(10000, '\n');

    string* humanNamesArr = new string[humanPlayers];
    cout << "\tВведите имена игроков:\n";
    for (int i = 0; i < humanPlayers; i++) {
        cout << "\tИгрок " << i + 1 << ": ";
        getline(cin, humanNamesArr[i]);
        if (humanNamesArr[i].empty()) {
            humanNamesArr[i] = "Игрок " + to_string(i + 1);
        }
    }

    int totalNames = computerNames.size();
    if (totalNames > 0) {
        string* namesArray = new string[totalNames];
        int idx = 0;
        for (const string& name : computerNames) {
            namesArray[idx++] = name;
        }

        bool* used = new bool[totalNames] { false };

        cout << "\n\tКомпьютерные игроки (соперники):\n";
        for (int i = 0; i < aiPlayers && i < totalNames; i++) {
            int randomIndex;
            do {
                randomIndex = rand() % totalNames;
            } while (used[randomIndex]);

            used[randomIndex] = true;
            cout << "\t" << i + 1 << ". " << namesArray[randomIndex] << '\n';
        }

        delete[] namesArray;
        delete[] used;
    }
    else {
        cout << "\tФайл имён ИИ пуст!\n";
    }

    delete[] humanNamesArr;

    Sleep(500);
    cout << "\n\t\t...Нажмите Enter чтобы вернуться в главное меню\n";
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
        cout << "\t\t\nНАСТРОЙКИ\n"
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
    cout << "\t\tО ПРОГРАММЕ\n\n\tВерсия программы: 1.1\n\tРазработчик: Яна Леонович\n\n\t...Нажмите Enter чтобы вернуться в главное меню\n";
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
    srand(static_cast<unsigned>(time(0)));

    CreateDefaultSettingsFile();
    LoadSettings();
    CreateComputerNamesFile();
    LoadComputerNames();

    int choice = 0;

    do {
        system("cls");
        cout << "\tСОРЕВНОВАНИЯ АРМИЙ\n\n\t"
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
