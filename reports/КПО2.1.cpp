#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_DIFFICULTY = 1;
const int SET_DUNGEON_SIZE = 2;
const int SET_LIVES = 3;
const int SET_BACK = 4;

const string PROGRAM_VERSION = "1.0";
const string STUDENT_NAME = "Тересюк Д. Ю.";

struct GameSettings {
    int difficulty = 1;
    int dungeonSize = 10;
    int lives = 3;
};

GameSettings settings;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
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
    cout << SET_DIFFICULTY << ". Уровень сложности (текущий: ";
    switch (settings.difficulty) {
    case 1: cout << "Легко";  break;
    case 2: cout << "Средне"; break;
    case 3: cout << "Сложно"; break;
    }
    cout << ")\n";
    cout << SET_DUNGEON_SIZE << ". Величина подземелья (текущая: "
        << settings.dungeonSize << ")\n";
    cout << SET_LIVES << ". Количество жизней (текущее: "
        << settings.lives << ")\n";
    cout << SET_BACK << ". Назад в главное меню\n";
}

void startGame() {
    cout << "\n>>> Игра началась! Удачи в подземелье!\n";
    cout << "    Сложность: ";
    switch (settings.difficulty) {
    case 1: cout << "Легко";  break;
    case 2: cout << "Средне"; break;
    case 3: cout << "Сложно"; break;
    }
    cout << ", размер подземелья: " << settings.dungeonSize
        << ", жизней: " << settings.lives << "\n";
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

void waitForEnter() {
    cout << "\nНажмите Enter для возврата в меню...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void openSettings() {
    int choice = 0;
    while (true) {
        clearScreen();
        showSettingsMenu();
        cout << "Выберите параметр: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nОшибка: необходимо ввести число!\n";
            waitForEnter();
            continue;
        }
        switch (choice) {
        case SET_DIFFICULTY: {
            int d;
            cout << "1 - Легко, 2 - Средне, 3 - Сложно. Ваш выбор: ";
            if (cin >> d && d >= 1 && d <= 3) settings.difficulty = d;
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
            if (cin >> s && s >= 5 && s <= 100) settings.dungeonSize = s;
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
            if (cin >> l && l >= 1 && l <= 10) settings.lives = l;
            else {
                cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Ошибка ввода! Допустимо 1..10.\n";
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

    int choice = 0;
    while (true) {
        clearScreen();
        showMainMenu();

        cout << "Введите номер пункта меню: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nОшибка: необходимо ввести число!\n";
            waitForEnter();
            continue;
        }

        switch (choice) {
        case MENU_START_GAME: clearScreen(); startGame();    waitForEnter(); break;
        case MENU_LOAD_GAME:  clearScreen(); loadGame();     waitForEnter(); break;
        case MENU_SETTINGS:   clearScreen(); openSettings(); break;
        case MENU_ABOUT:      clearScreen(); showAbout();    waitForEnter(); break;
        case MENU_EXIT:
            clearScreen();
            cout << "\nВыход из программы. До свидания!\n";
            return 0;
        default:
            cout << "\nОшибка: пункта с таким номером нет!\n";
            waitForEnter();
        }
    }
}