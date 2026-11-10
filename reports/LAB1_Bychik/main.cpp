#define NOMINMAX                 
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_SPECIALITY = 1;
const int SET_NAME = 2;
const int SET_SUBJECTS = 3;
const int SET_BACK = 4;

const string VERSION = "1.0";

struct Settings {
    string speciality = "Программная инженерия";
    string studentName = "Кристина";
    int    subjectsCount = 5;
};

void clearInput() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void waitEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.get();
}

void startGame() {
    cout << "\n=== Начало игры \"Моя жизнь в университете\" ===\n";
    cout << "Вы поступаете в университет. Удачи!\n";
    waitEnter();
}

void loadGame() {
    cout << "\nФункция загрузки пока недоступна.\n";
    cout << "Она появится в ближайших обновлениях.\n";
    waitEnter();
}

void settingsMenu(Settings& s) {
    int choice = 0;
    while (choice != SET_BACK) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Специальность (текущая: " << s.speciality << ")\n";
        cout << "2. Имя студента (текущее: " << s.studentName << ")\n";
        cout << "3. Количество предметов (текущее: " << s.subjectsCount << ")\n";
        cout << "4. Назад\n";
        cout << "Выбор: ";
        cin >> choice;

        if (cin.fail()) {
            clearInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        switch (choice) {
        case SET_SPECIALITY:
            cout << "Введите специальность: ";
            clearInput();
            getline(cin, s.speciality);
            break;
        case SET_NAME:
            cout << "Введите имя студента: ";
            clearInput();
            getline(cin, s.studentName);
            break;
        case SET_SUBJECTS: {
            cout << "Введите количество предметов: ";
            int n;
            cin >> n;
            if (cin.fail() || n <= 0) {
                clearInput();
                cout << "Некорректное значение!\n";
            }
            else {
                s.subjectsCount = n;
            }
            break;
        }
        case SET_BACK:
            break;
        default:
            cout << "Такого пункта нет.\n";
        }
    }
}

void aboutProgram() {
    cout << "\n=== О программе ===\n";
    cout << "Версия: " << VERSION << "\n";
    cout << "Студент: Фамилия Имя\n";
    waitEnter();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    Settings settings;
    int choice = 0;

    while (choice != MENU_EXIT) {
        cout << "\n*** Моя жизнь в университете ***\n";
        cout << "1. Начать игру\n";
        cout << "2. Загрузить игру\n";
        cout << "3. Настройки\n";
        cout << "4. О программе\n";
        cout << "5. Выход\n";
        cout << "Выбор: ";
        cin >> choice;

        if (cin.fail()) {
            clearInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        switch (choice) {
        case MENU_START:    startGame();            break;
        case MENU_LOAD:     loadGame();             break;
        case MENU_SETTINGS: settingsMenu(settings); break;
        case MENU_ABOUT:    aboutProgram();         break;
        case MENU_EXIT:
            cout << "Выход из программы...\n";
            break;
        default:
            cout << "Такого пункта нет.\n";
        }
    }

    return 0;
}
