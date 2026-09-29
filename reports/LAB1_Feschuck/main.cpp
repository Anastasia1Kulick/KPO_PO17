#include <iostream>
#include <string>

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_CITY_SIZE = 1;
const int SET_BUDGET = 2;
const int SET_POPULATION = 3;
const int SET_BACK = 4;

struct GameSettings {
    int citySize;
    int budget;
    int population; 

    GameSettings() : citySize(10), budget(1000), population(100) {}
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.ignore(10000, '\n');
    cin.get();
}

void showMainMenu() {
    cout << "=====================================\n";
    cout << "          ГОРОД МЕЧТЫ\n";
    cout << "=====================================\n";
    cout << MENU_START_GAME << ". Начать игру\n";
    cout << MENU_LOAD_GAME << ". Загрузить игру\n";
    cout << MENU_SETTINGS << ". Настройки\n";
    cout << MENU_ABOUT << ". О программе\n";
    cout << MENU_EXIT << ". Выход\n";
    cout << "=====================================\n";
    cout << "Выберите пункт меню: ";
}

void startGame() {
    clearScreen();
    cout << "=== НАЧАЛО ИГРЫ ===\n";
    cout << "Вы начали новую игру в «Город мечты»!\n";
    waitForEnter();
}

void loadGame() {
    clearScreen();
    cout << "=== ЗАГРУЗКА ИГРЫ ===\n";
    cout << "Данная функция на данный момент недоступна\n";
    cout << "и появится в ближайших обновлениях.\n";
    waitForEnter();
}

void switchCitySize(int& size) {
    if (size == 10)      size = 20;
    else if (size == 20) size = 30;
    else                 size = 10;
}

void switchBudget(int& budget) {
    if (budget == 1000)      budget = 5000;
    else if (budget == 5000) budget = 10000;
    else                     budget = 1000;
}

void switchPopulation(int& population) {
    if (population == 100)      population = 500;
    else if (population == 500) population = 1000;
    else                        population = 100;
}

void showSettingsMenu(const GameSettings& settings) {
    cout << "=====================================\n";
    cout << "             НАСТРОЙКИ\n";
    cout << "=====================================\n";
    cout << SET_CITY_SIZE << ". Размер города:      " << settings.citySize << "\n";
    cout << SET_BUDGET << ". Начальный бюджет:   " << settings.budget << "\n";
    cout << SET_POPULATION << ". Количество жителей: " << settings.population << "\n";
    cout << SET_BACK << ". Назад\n";
    cout << "=====================================\n";
    cout << "Выберите пункт: ";
}

void settingsMenu(GameSettings& settings) {
    int choice = 0;
    while (choice != SET_BACK) {
        clearScreen();
        showSettingsMenu(settings);
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case SET_CITY_SIZE:
            switchCitySize(settings.citySize);
            break;
        case SET_BUDGET:
            switchBudget(settings.budget);
            break;
        case SET_POPULATION:
            switchPopulation(settings.population);
            break;
        case SET_BACK:
            break;
        default:
            cout << "Неверный пункт меню!\n";
            waitForEnter();
            break;
        }
    }
}

void showAbout() {
    clearScreen();
    cout << "=== О ПРОГРАММЕ ===\n";
    cout << "Игра: «Город мечты»\n";
    cout << "Версия программы: 1.0\n";
    cout << "Студент: Фещук Артём\n";
    cout << "\n--- Рекомендация музыкального плейлиста ---\n";
    cout << "Для атмосферы строительства города советуем:\n";
    cout << "  • «City Builder OST» — спокойный эмбиент\n";
    cout << "  • «SimCity Soundtrack» — классика градостроения\n";
    cout << "  • «Lo-Fi City Beats» — ненавязчивый фон для игры\n";
    cout << "  • «Max Korzh Playlist»\n";
    cout << "Приятной игры под хорошую музыку!\n";
    waitForEnter();
}

int main() {
    setlocale(LC_ALL, "Russian");
    GameSettings settings;
    int choice = 0;

    while (choice != MENU_EXIT) {
        clearScreen();
        showMainMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case MENU_START_GAME:
            startGame();
            break;
        case MENU_LOAD_GAME:
            loadGame();
            break;
        case MENU_SETTINGS:
            settingsMenu(settings);
            break;
        case MENU_ABOUT:
            showAbout();
            break;
        case MENU_EXIT:
            cout << "Выход из программы...\n";
            break;
        default:
            cout << "Неверный пункт меню!\n";
            waitForEnter();
            break;
        }
    }

    return 0;
}
