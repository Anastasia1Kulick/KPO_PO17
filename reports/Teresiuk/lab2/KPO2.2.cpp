#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <fstream>      
#include <sstream>     
#include <stack>       
#include <ctime>        
#include <stdexcept>    

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_DIFFICULTY = 1;
const int SET_DUNGEON_SIZE = 2;
const int SET_LIVES = 3;
const int SET_ROLLBACK_SIZE = 4;
const int SET_BACK = 5;

const string PROGRAM_VERSION = "2.0";
const string STUDENT_NAME = "Тересюк Д. Ю.";
const string SETTINGS_FILE = "settings.txt";
const string LOG_FILE = "app.log";

struct GameSettings {
    int    dungeonSize = 10;
    int    lives = 3;
    string difficulty = "Легко";
};

GameSettings settings;
stack<int> sizeHistory;

void clearScreen();
void showMainMenu();
void showSettingsMenu();
void startGame();
void loadGame();
void showAbout();
void openSettings();
void waitForEnter();
int  readInt(const string& prompt);

bool fileExists(const string& path);
void createDefaultSettingsFile();
bool loadSettings();
bool saveSettings();

void logMessage(const string& message);

bool isValidDungeonSize(int s);
bool isValidLives(int l);
bool isValidDifficulty(const string& d);

void pushSizeHistory(int size);
bool popSizeHistory(int& out);

void logMessage(const string& message) {
    try {
        ofstream f(LOG_FILE, ios::app);
        if (!f.is_open()) return;

        time_t t = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));

        f << "[" << buf << "] " << message << "\n";
    }
    catch (...) {
    }
}

bool fileExists(const string& path) {
    ifstream f(path);
    return f.good();
}

void createDefaultSettingsFile() {
    GameSettings def;
    try {
        ofstream f(SETTINGS_FILE);
        if (!f.is_open())
            throw runtime_error("Не удалось создать файл настроек");

        f << "dungeonSize=" << def.dungeonSize << "\n";
        f << "lives=" << def.lives << "\n";
        f << "difficulty=" << def.difficulty << "\n";

        logMessage("Создан файл настроек по умолчанию");
    }
    catch (const exception& e) {
        logMessage(string("ОШИБКА создания файла: ") + e.what());
    }
}

bool loadSettings() {
    try {
        ifstream f(SETTINGS_FILE);
        if (!f.is_open())
            throw runtime_error("Файл настроек не открывается");

        string line;
        while (getline(f, line)) {
            istringstream iss(line);
            string key;
            if (!getline(iss, key, '=')) continue;

            if (key == "dungeonSize") {
                iss >> settings.dungeonSize;
            }
            else if (key == "lives") {
                iss >> settings.lives;
            }
            else if (key == "difficulty") {
                getline(iss, settings.difficulty);
                if (!settings.difficulty.empty() && settings.difficulty[0] == ' ')
                    settings.difficulty.erase(0, 1);
            }
        }

        logMessage("Настройки загружены из " + SETTINGS_FILE);
        return true;
    }
    catch (const exception& e) {
        logMessage(string("Исключение при чтении: ") + e.what());
        return false;
    }
}

bool saveSettings() {
    try {
        ofstream f(SETTINGS_FILE);
        if (!f.is_open())
            throw runtime_error("Файл настроек не открывается на запись");

        f << "dungeonSize=" << settings.dungeonSize << "\n";
        f << "lives=" << settings.lives << "\n";
        f << "difficulty=" << settings.difficulty << "\n";

        logMessage("Настройки сохранены");
        return true;
    }
    catch (const exception& e) {
        logMessage(string("Исключение при записи: ") + e.what());
        return false;
    }
}

bool isValidDungeonSize(int s) { return s >= 5 && s <= 100; }
bool isValidLives(int l) { return l >= 1 && l <= 10; }
bool isValidDifficulty(const string& d) {
    return d == "Легко" || d == "Средне" || d == "Сложно";
}

void pushSizeHistory(int size) {
    sizeHistory.push(size);
    logMessage("В историю добавлен размер: " + to_string(size));
}

bool popSizeHistory(int& out) {
    if (sizeHistory.empty()) return false;
    out = sizeHistory.top();
    sizeHistory.pop();
    logMessage("Из истории извлечён размер: " + to_string(out));
    return true;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка: необходимо ввести число!\n";
    }
}

void waitForEnter() {
    cout << "\nНажмите Enter для возврата в меню...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void showMainMenu() {
    cout << "\n ПРИКЛЮЧЕНИЕ В ПОДЗЕМЕЛЬЕ\n";
    cout << MENU_START_GAME << ". Начать игру\n";
    cout << MENU_LOAD_GAME << ". Загрузить игру\n";
    cout << MENU_SETTINGS << ". Настройки\n";
    cout << MENU_ABOUT << ". О программе\n";
    cout << MENU_EXIT << ". Выход\n";
}

void showSettingsMenu() {
    cout << "\n НАСТРОЙКИ \n";
    cout << SET_DIFFICULTY << ". Уровень сложности (текущий: "
        << settings.difficulty << ")\n";
    cout << SET_DUNGEON_SIZE << ". Величина подземелья (текущая: "
        << settings.dungeonSize << ")\n";
    cout << SET_LIVES << ". Количество жизней (текущее: "
        << settings.lives << ")\n";
    cout << SET_ROLLBACK_SIZE << ". Вернуть предыдущий размер (в истории: "
        << sizeHistory.size() << ")\n";
    cout << SET_BACK << ". Назад в главное меню\n";
}

void startGame() {
    cout << "\n>>> Игра началась! Удачи в подземелье!\n";
    cout << "    Сложность: " << settings.difficulty << "\n";
    cout << "    Размер подземелья: " << settings.dungeonSize << "\n";
    cout << "    Жизней: " << settings.lives << "\n";
    logMessage("Начата игра");
}

void loadGame() {
    cout << "\n>>> Функция загрузки игры пока недоступна.\n";
}

void showAbout() {
    cout << "\n О ПРОГРАММЕ \n";
    cout << "Название: Приключение в подземелье\n";
    cout << "Версия:   " << PROGRAM_VERSION << "\n";
    cout << "Студент:  " << STUDENT_NAME << "\n";
}

void openSettings() {
    int choice = 0;
    while (true) {
        clearScreen();
        showSettingsMenu();
        choice = readInt("Выберите параметр: ");

        switch (choice) {
        case SET_DIFFICULTY: {
            int d;
            cout << "1 - Легко, 2 - Средне, 3 - Сложно. Ваш выбор: ";
            if (cin >> d && d >= 1 && d <= 3) {
                string diff = (d == 1) ? "Легко" :
                    (d == 2) ? "Средне" : "Сложно";
                if (isValidDifficulty(diff)) {
                    settings.difficulty = diff;
                    saveSettings();
                    cout << "Сложность сохранена.\n";
                }
            }
            else {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка ввода!\n";
            }
            waitForEnter();
            break;
        }
        case SET_DUNGEON_SIZE: {
            int s;
            cout << "Введите величину подземелья (5..100): ";
            if (cin >> s && isValidDungeonSize(s)) {
                pushSizeHistory(settings.dungeonSize);
                settings.dungeonSize = s;
                saveSettings();
                cout << "Размер сохранён.\n";
            }
            else {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка ввода! Допустимо 5..100.\n";
            }
            waitForEnter();
            break;
        }
        case SET_LIVES: {
            int l;
            cout << "Введите количество жизней (1..10): ";
            if (cin >> l && isValidLives(l)) {
                settings.lives = l;
                saveSettings();
                cout << "Жизни сохранены.\n";
            }
            else {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка ввода! Допустимо 1..10.\n";
            }
            waitForEnter();
            break;
        }
        case SET_ROLLBACK_SIZE: {
            int prev;
            if (popSizeHistory(prev)) {
                settings.dungeonSize = prev;
                saveSettings();
                cout << "Размер подземелья возвращён к: " << prev << "\n";
            }
            else {
                cout << "История пуста — возвращать нечего.\n";
            }
            waitForEnter();
            break;
        }
        case SET_BACK:
            return;
        default:
            cout << "Ошибка: пункта с таким номером нет!\n";
            waitForEnter();
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    logMessage("Программа запущена");

    if (!fileExists(SETTINGS_FILE)) {
        cout << "Файл настроек не найден. Создаётся по умолчанию...\n";
        createDefaultSettingsFile();
    }

    loadSettings();

    int choice = 0;
    while (true) {
        clearScreen();
        showMainMenu();
        choice = readInt("Введите номер пункта меню: ");

        switch (choice) {
        case MENU_START_GAME: clearScreen(); startGame();    waitForEnter(); break;
        case MENU_LOAD_GAME:  clearScreen(); loadGame();     waitForEnter(); break;
        case MENU_SETTINGS:   clearScreen(); openSettings(); break;
        case MENU_ABOUT:      clearScreen(); showAbout();    waitForEnter(); break;
        case MENU_EXIT:
            clearScreen();
            logMessage("Программа завершена");
            cout << "\nВыход из программы. До свидания!\n";
            return 0;
        default:
            cout << "\nОшибка: пункта с таким номером нет!\n";
            waitForEnter();
        }
    }
}