#include <iostream>
#include <windows.h>
using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

struct Settings {
    int citySize = 1;
    int budget = 1000;
    int citizens = 100;
};

int readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Ошибка ввода. Повторите: ";
    }
    return value;
}

void pause() {
    cout << "\nНажмите Enter...";
    cin.ignore(10000, '\n');
    cin.get();
}

void startGame() {
    cout << "\nИгра началась!\n";
    pause();
}

void loadGame() {
    cout << "\nЗагрузка пока недоступна.\n";
    pause();
}

void showSettings(Settings& s) {
    while (true) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Размер города: " << s.citySize << "\n";
        cout << "2. Начальный бюджет: " << s.budget << "\n";
        cout << "3. Количество жителей: " << s.citizens << "\n";
        cout << "0. Назад\n";
        cout << "Выбор: ";

        int choice = readInt();

        if (choice == 0) return;
        if (choice == 1) { s.citySize++; if (s.citySize > 3) s.citySize = 1; }
        else if (choice == 2) {
            if (s.budget == 1000) s.budget = 5000;
            else if (s.budget == 5000) s.budget = 10000;
            else s.budget = 1000;
        }
        else if (choice == 3) { s.citizens += 100; if (s.citizens > 500) s.citizens = 100; }
        else cout << "Нет такого пункта.\n";
    }
}

void showAbout() {
    cout << "\nВерсия программы: 1.0\n";
    cout << "Студент: Короткевич Глеб\n";
    pause();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Settings settings;

    while (true) {
        cout << "\n=== Город мечты ===\n";
        cout << MENU_START << ". Начать игру\n";
        cout << MENU_LOAD << ". Загрузить игру\n";
        cout << MENU_SETTINGS << ". Настройки\n";
        cout << MENU_ABOUT << ". О программе\n";
        cout << MENU_EXIT << ". Выход\n";
        cout << "Выбор: ";

        int choice = readInt();

        if (choice == MENU_START) startGame();
        else if (choice == MENU_LOAD) loadGame();
        else if (choice == MENU_SETTINGS) showSettings(settings);
        else if (choice == MENU_ABOUT) showAbout();
        else if (choice == MENU_EXIT) break;
        else cout << "Нет такого пункта.\n";
    }
    return 0;
}