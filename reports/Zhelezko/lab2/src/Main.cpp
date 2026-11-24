#include <iostream>
#include <limits>
#include <string>
#include <clocale>
#include <map>
#include <fstream>

using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SETTINGS_SHOW = 1;
const int SETTINGS_CHANGE_SIZE = 2;
const int SETTINGS_BACK = 3;

const int MIN_MAZE_SIZE = 5;
const int MAX_MAZE_SIZE = 50;

const string GAME_TITLE = "Побег из лабиринта";
const string PROGRAM_VERSION = "2.0 ";
const string STUDENT_NAME = "Железко Андрей";

const string SETTINGS_FILE = "settings.txt";
const string DIFFICULTIES_FILE = "difficulties.txt";

struct GameSettings {
    int mazeWidth = 10;
    int mazeHeight = 10;
    string difficulty = "Легкий";
};

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void createDefaultDifficultiesFile() {
    ofstream out(DIFFICULTIES_FILE);

    if (!out) {
        cerr << "Не удалось создать файл уровней сложности: " << DIFFICULTIES_FILE << "\n";
        return;
    }

    out << "Легкий=0\n";
    out << "Средний=101\n";
    out << "Сложный=401\n";
}

void ensureDifficultiesFile() {
    ifstream in(DIFFICULTIES_FILE);

    if (in.good()) {
        return;
    }

    createDefaultDifficultiesFile();
}

bool loadDifficultyLevels(map<string, int>& levels) {
    levels.clear();

    ifstream in(DIFFICULTIES_FILE);

    if (!in) {
        return false;
    }

    string line;

    while (getline(in, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t pos = line.find('=');

        if (pos == string::npos) {
            continue;
        }

        string name = trim(line.substr(0, pos));
        string valueStr = trim(line.substr(pos + 1));

        if (name.empty() || valueStr.empty()) {
            continue;
        }

        try {
            int minArea = stoi(valueStr);
            levels[name] = minArea;
        }
        catch (...) {
            continue;
        }
    }

    if (levels.empty()) {
        levels["Легкий"] = 0;
        levels["Средний"] = 101;
        levels["Сложный"] = 401;
        createDefaultDifficultiesFile();
        return false;
    }

    return true;
}

string calculateDifficulty(int width, int height, const map<string, int>& levels) {
    int area = width * height;
    string result;
    int bestMinArea = -1;

    for (const auto& pair : levels) {
        const string& name = pair.first;
        int minArea = pair.second;

        if (area >= minArea && minArea > bestMinArea) {
            bestMinArea = minArea;
            result = name;
        }
    }

    if (result.empty()) {
        if (!levels.empty()) {
            int minValue = numeric_limits<int>::max();

            for (const auto& pair : levels) {
                if (pair.second < minValue) {
                    minValue = pair.second;
                    result = pair.first;
                }
            }
        }
        else {
            result = "Неизвестно";
        }
    }

    return result;
}

void updateDifficulty(GameSettings& settings, const map<string, int>& levels) {
    settings.difficulty = calculateDifficulty(settings.mazeWidth, settings.mazeHeight, levels);
}

bool saveSettings(const GameSettings& settings) {
    ofstream out(SETTINGS_FILE);

    if (!out) {
        cerr << "Не удалось сохранить настройки в файл: " << SETTINGS_FILE << "\n";
        return false;
    }

    out << "mazeWidth=" << settings.mazeWidth << "\n";
    out << "mazeHeight=" << settings.mazeHeight << "\n";
    out << "difficulty=" << settings.difficulty << "\n";

    return true;
}

void loadSettings(GameSettings& settings, const map<string, int>& levels) {
    settings = GameSettings{};

    ifstream in(SETTINGS_FILE);

    if (!in) {
        updateDifficulty(settings, levels);
        saveSettings(settings);
        return;
    }

    string line;

    while (getline(in, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t pos = line.find('=');

        if (pos == string::npos) {
            continue;
        }

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));

        if (key == "mazeWidth") {
            try {
                settings.mazeWidth = stoi(value);
            }
            catch (...) {
                settings.mazeWidth = 10;
            }
        }
        else if (key == "mazeHeight") {
            try {
                settings.mazeHeight = stoi(value);
            }
            catch (...) {
                settings.mazeHeight = 10;
            }
        }
        else if (key == "difficulty") {
            settings.difficulty = value;
        }
    }

    if (settings.mazeWidth < MIN_MAZE_SIZE || settings.mazeWidth > MAX_MAZE_SIZE) {
        settings.mazeWidth = 10;
    }

    if (settings.mazeHeight < MIN_MAZE_SIZE || settings.mazeHeight > MAX_MAZE_SIZE) {
        settings.mazeHeight = 10;
    }

    updateDifficulty(settings, levels);
    saveSettings(settings);
}

int readInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Ошибка: введите целое число.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void waitEnter() {
    cout << "\nНажмите Enter, чтобы продолжить...";
    cin.get();
}

void changeMazeSize(GameSettings& settings, const map<string, int>& levels) {
    cout << "\n    Изменение размера лабиринта    \n";

    int width = readInt(
        "Введите ширину x (" +
        to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): "
    );

    if (width < MIN_MAZE_SIZE || width > MAX_MAZE_SIZE) {
        cout << "Недопустимая ширина.\n";
        waitEnter();
        return;
    }

    int height = readInt(
        "Введите высоту y (" +
        to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): "
    );

    if (height < MIN_MAZE_SIZE || height > MAX_MAZE_SIZE) {
        cout << "Недопустимая высота.\n";
        waitEnter();
        return;
    }

    settings.mazeWidth = width;
    settings.mazeHeight = height;
    updateDifficulty(settings, levels);
    saveSettings(settings);

    cout << "\nРазмер лабиринта сохранён: "
        << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
    cout << "Уровень сложности автоматически изменён на: "
        << settings.difficulty << "\n";

    waitEnter();
}

void settingsMenu(GameSettings& settings, const map<string, int>& levels) {
    while (true) {
        updateDifficulty(settings, levels);

        cout << "\n    Настройки    \n";
        cout << "Текущий уровень сложности: " << settings.difficulty << "\n";
        cout << "Текущий размер лабиринта: "
            << settings.mazeWidth << " x " << settings.mazeHeight << "\n";

        cout << SETTINGS_SHOW << ". Показать параметры\n";
        cout << SETTINGS_CHANGE_SIZE << ". Изменить размер лабиринта\n";
        cout << SETTINGS_BACK << ". Назад в главное меню\n";

        int choice = readInt("Выберите пункт: ");

        switch (choice) {
        case SETTINGS_SHOW:
            cout << "\nТекущие настройки:\n";
            cout << "Сложность: " << settings.difficulty << "\n";
            cout << "Размер лабиринта: "
                << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
            waitEnter();
            break;

        case SETTINGS_CHANGE_SIZE:
            changeMazeSize(settings, levels);
            break;

        case SETTINGS_BACK:
            return;

        default:
            cout << "Неверный пункт меню.\n";
            waitEnter();
        }
    }
}

void startGame() {
    cout << "\n    Начать игру    \n";
    cout << "Игра «Побег из лабиринта» началась!\n";
    waitEnter();
}

void loadGame() {
    cout << "\n    Загрузить игру    \n";
    cout << "Данная функция на данный момент недоступна и появится в ближайших обновлениях.\n";
    waitEnter();
}

void about() {
    cout << "\n    О программе    \n";
    cout << "Название: " << GAME_TITLE << "\n";
    cout << "Версия: " << PROGRAM_VERSION << "\n";
    cout << "Выполнил: " << STUDENT_NAME << "\n";
    waitEnter();
}

int main() {
    setlocale(LC_ALL, "Russian");

    ensureDifficultiesFile();

    map<string, int> difficultyLevels;
    loadDifficultyLevels(difficultyLevels);

    GameSettings settings;
    loadSettings(settings, difficultyLevels);

    int choice = 0;

    do {
        cout << "\n" << GAME_TITLE << "\n";
        cout << MENU_START << ". Начать игру\n";
        cout << MENU_LOAD << ". Загрузить игру\n";
        cout << MENU_SETTINGS << ". Настройки\n";
        cout << MENU_ABOUT << ". О программе\n";
        cout << MENU_EXIT << ". Выход\n";

        choice = readInt("Введите номер пункта меню: ");

        switch (choice) {
        case MENU_START:
            startGame();
            break;

        case MENU_LOAD:
            loadGame();
            break;

        case MENU_SETTINGS:
            settingsMenu(settings, difficultyLevels);
            break;

        case MENU_ABOUT:
            about();
            break;

        case MENU_EXIT:
            cout << "\nВыход из программы.\n";
            break;

        default:
            cout << "Неверный пункт меню. Попробуйте снова.\n";
            waitEnter();
        }

    } while (choice != MENU_EXIT);

    return 0;
}