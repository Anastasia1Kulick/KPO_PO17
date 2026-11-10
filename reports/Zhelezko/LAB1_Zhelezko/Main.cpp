#include <iostream>
#include <limits>
#include <string>
#include <clocale>

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
const string PROGRAM_VERSION = "1.0";
const string STUDENT_NAME = "Железко Андрей";

struct GameSettings {
    int mazeWidth = 10;
    int mazeHeight = 10;
    string difficulty = "Легкий";
};

string calculateDifficulty(int width, int height) {
    int area = width * height;

    if (area <= 100) {
        return "Легкий";
    }
    else if (area <= 400) {
        return "Средний";
    }
    else {
        return "Сложный";
    }
}

void updateDifficulty(GameSettings& settings) {
    settings.difficulty = calculateDifficulty(settings.mazeWidth, settings.mazeHeight);
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

void changeMazeSize(GameSettings& settings) {
    cout << "\n    Изменение размера лабиринта    \n";

    int width = readInt("Введите ширину x (" + to_string(MIN_MAZE_SIZE) + "-" + to_string(MAX_MAZE_SIZE) + "): ");

    if (width < MIN_MAZE_SIZE or width > MAX_MAZE_SIZE) {
        cout << "Недопустимая ширина.\n";
        waitEnter();
        return;
    }

    int height = readInt("Введите высоту y (" + to_string(MIN_MAZE_SIZE) + "-" + to_string(MAX_MAZE_SIZE) + "): ");

    if (height < MIN_MAZE_SIZE or height > MAX_MAZE_SIZE) {
        cout << "Недопустимая высота.\n";
        waitEnter();
        return;
    }

    settings.mazeWidth = width;
    settings.mazeHeight = height;
    updateDifficulty(settings);

    cout << "\nРазмер лабиринта сохранён: " << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
    cout << "Уровень сложности автоматически изменён на: " << settings.difficulty << "\n";

    waitEnter();
}

void settingsMenu(GameSettings& settings) {
    while (true) {
        updateDifficulty(settings);

        cout << "\n    Настройки    \n";
        cout << "Текущий уровень сложности: " << settings.difficulty << " \n";
        cout << "Текущий размер лабиринта: " << settings.mazeWidth << " x " << settings.mazeHeight << "\n";

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
            changeMazeSize(settings);
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

    GameSettings settings;
    updateDifficulty(settings);

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
            settingsMenu(settings);
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
