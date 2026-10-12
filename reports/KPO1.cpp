#include <iostream>
#include <string>
#include <limits>
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
const string PROGRAM_VERSION = "v1.0";
const string STUDENT_NAME = "Жук И.В.";
struct GameSettings {
    int difficulty = 1;
    int totalPlayers = 2;
    int alivePlayers = 2;
    int getAICount() const {
        int ai = totalPlayers - alivePlayers;
        return (ai < 0) ? 0 : ai;
    }
};
void showMainMenu();
void startGame(const GameSettings& settings);
void loadGame();
void showSettings(GameSettings& settings);
void showAbout();
int  getIntInput(int min, int max);
void waitForRepeatInput(int expected);
int main() {
    setlocale(LC_ALL, "Russian");

    GameSettings settings;
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
    cout << "    Сложность:     ";
    switch (settings.difficulty) {
    case 1: cout << "Легкий\n";  break;
    case 2: cout << "Средний\n"; break;
    case 3: cout << "Сложный\n"; break;
    }
    cout << "    Всего игроков: " << settings.totalPlayers << "\n";
    cout << "    Живых игроков: " << settings.alivePlayers << "\n";
    cout << "    ИИ-игроков:    " << settings.getAICount() << "\n";
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
        cout << "  " << SET_DIFFICULTY << ". Уровень сложности: ";
        switch (settings.difficulty) {
        case 1: cout << "Легкий\n";  break;
        case 2: cout << "Средний\n"; break;
        case 3: cout << "Сложный\n"; break;
        }
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
            cout << "  1 - Легкий\n";
            cout << "  2 - Средний\n";
            cout << "  3 - Сложный\n";
            cout << "Введите уровень сложности: ";
            settings.difficulty = getIntInput(1, 3);
            cout << "\nУровень сложности сохранён!\n";
            waitForRepeatInput(SET_DIFFICULTY);
            break;
        }
        case SET_TOTAL: {
            system("cls");
            cout << "        ВСЕГО ИГРОКОВ\n";
            cout << "Введите общее количество игроков: ";
            settings.totalPlayers = getIntInput(2, 100);

            if (settings.alivePlayers > settings.totalPlayers) {
                settings.alivePlayers = settings.totalPlayers;
            }
            cout << "\nВсего игроков сохранено!\n";
            waitForRepeatInput(SET_TOTAL);
            break;
        }
        case SET_ALIVE: {
            system("cls");
            cout << "        ЖИВЫХ ИГРОКОВ\n";
            cout << "Введите количество живых игроков: ";
            settings.alivePlayers = getIntInput(1, settings.totalPlayers);
            cout << "\nКоличество живых игроков сохранено!\n";
            waitForRepeatInput(SET_ALIVE);
            break;
        }
        case SET_AI_INFO: {
            system("cls");
            cout << "          ИИ-ИГРОКИ\n";
            cout << "  Формула расчёта:\n";
            cout << "    ИИ = все игроки − живые игроки\n";
            cout << "  Всего игроков:  " << settings.totalPlayers << "\n";
            cout << "  Живых игроков:  " << settings.alivePlayers << "\n";
            cout << "  ИИ-игроков:     " << settings.getAICount() << "\n";
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
            return;   // возврат туда, откуда вызвали
        }
        else {
            cout << "Неверно. Введите " << expected << " для продолжения: ";
        }
    }
}
