#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <set>
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

const string PROGRAM_VERSION = "v2.0";
const string STUDENT_NAME = "Жук И.В.";
const string SETTINGS_FILE = "settings.txt";

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

void showMainMenu();
void startGame(const GameSettings& settings);
void loadGame();
void showSettings(GameSettings& settings);
void showAbout();
int  getIntInput(int min, int max);
void waitForRepeatInput(int expected);
void saveSettings(const GameSettings& s);
bool loadSettings(GameSettings& s);
void ensureSettingsFile(GameSettings& s);
string getDifficultyInput();
string aiNameToString(AI_NAMES n);
set<string> buildAINamesSet();

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
    AI_NAMES all[] = {
        AI_NAMES::IVAN, AI_NAMES::KIRILL, AI_NAMES::DANIIL,
        AI_NAMES::ANATOLY, AI_NAMES::ANDREY, AI_NAMES::ALEXANDER,
        AI_NAMES::VIOLETTA, AI_NAMES::GLEB
    };
    for (auto n : all) names.insert(aiNameToString(n));
    return names;
}

void saveSettings(const GameSettings& s) {
    ofstream f(SETTINGS_FILE);
    if (!f.is_open()) return;
    f << "difficulty=" << s.difficulty << "\n";
    f << "totalPlayers=" << s.totalPlayers << "\n";
    f << "alivePlayers=" << s.alivePlayers << "\n";
}

bool loadSettings(GameSettings& s) {
    ifstream f(SETTINGS_FILE);
    if (!f.is_open()) return false;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        auto pos = line.find('=');
        if (pos == string::npos) continue;

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        if (key == "difficulty") {
            s.difficulty = value;
        }
        else if (key == "totalPlayers") {
            try { s.totalPlayers = stoi(value); }
            catch (...) {}
        }
        else if (key == "alivePlayers") {
            try { s.alivePlayers = stoi(value); }
            catch (...) {}
        }
    }

    if (s.alivePlayers > s.totalPlayers)
        s.alivePlayers = s.totalPlayers;

    return true;
}

void ensureSettingsFile(GameSettings& s) {
    ifstream check(SETTINGS_FILE);
    if (!check.is_open()) {
        saveSettings(s);
    }
    else {
        check.close();
        loadSettings(s);
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    GameSettings settings;
    ensureSettingsFile(settings);

    int choice = 0;
    do {
        showMainMenu();
        choice = getIntInput(MENU_START_GAME, MENU_EXIT);

        switch (choice) {
        case MENU_START_GAME: startGame(settings);    break;
        case MENU_LOAD_GAME:  loadGame();             break;
        case MENU_SETTINGS:   showSettings(settings); break;
        case MENU_ABOUT:      showAbout();            break;
        case MENU_EXIT:
            cout << "\nВыход из программы. До свидания!\n";
            break;
        }
    } while (choice != MENU_EXIT);

    return 0;
}

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

void startGame(const GameSettings& settings) {
    system("cls");
    cout << "           НАЧАЛО ИГРЫ\n";
    cout << "  Игра началась!\n\n";
    cout << "  Параметры игры:\n";
    cout << "    Сложность:     " << settings.difficulty << "\n";
    cout << "    Всего игроков: " << settings.totalPlayers << "\n";
    cout << "    Живых игроков: " << settings.alivePlayers << "\n";
    cout << "    ИИ-игроков:    " << settings.getAICount() << "\n";

    set<string> aiNames = buildAINamesSet();
    cout << "  Доступные ИИ-имена:\n";
    int shown = 0;
    for (const auto& name : aiNames) {
        if (shown >= settings.getAICount()) break;
        cout << "    - " << name << "\n";
        ++shown;
    }

    waitForRepeatInput(MENU_START_GAME);
}

void loadGame() {
    system("cls");
    cout << "          ЗАГРУЗКА ИГРЫ\n";
    cout << "  Данная функция на данный момент\n";
    cout << "  недоступна и появится в ближайших\n";
    cout << "  обновлениях.\n";
    waitForRepeatInput(MENU_LOAD_GAME);
}

void showSettings(GameSettings& settings) {
    int choice = 0;
    do {
        system("cls");
        cout << "              НАСТРОЙКИ\n";
        cout << "  " << SET_DIFFICULTY << ". Уровень сложности: "
            << settings.difficulty << "\n";
        cout << "  " << SET_TOTAL << ". Всего игроков:     "
            << settings.totalPlayers << "\n";
        cout << "  " << SET_ALIVE << ". Живых игроков:     "
            << settings.alivePlayers << "\n";
        cout << "  " << SET_AI_INFO << ". ИИ-игроков:        "
            << settings.getAICount() << " (автоматически)\n";
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
            saveSettings(settings);
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
            saveSettings(settings);
            cout << "\nВсего игроков сохранено в файл!\n";
            waitForRepeatInput(SET_TOTAL);
            break;
        }
        case SET_ALIVE: {
            system("cls");
            cout << "        ЖИВЫХ ИГРОКОВ\n";
            cout << "Введите количество живых игроков: ";
            settings.alivePlayers = getIntInput(1, settings.totalPlayers);
            saveSettings(settings);
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