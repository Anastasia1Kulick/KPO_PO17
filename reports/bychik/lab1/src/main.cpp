#define NOMINMAX                 
#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

// Глобальные константы программы
namespace Config {
    const string VERSION = "1.0";
}

// Идентификаторы для главного меню
enum MainMenuOption {
    OPT_START = 1,
    OPT_LOAD,
    OPT_SETTINGS,
    OPT_ABOUT,
    OPT_EXIT
};

// Идентификаторы для меню настроек
enum SettingsMenuOption {
    OPT_CHANGE_SPECIALITY = 1,
    OPT_CHANGE_NAME,
    OPT_CHANGE_SUBJECTS,
    OPT_BACK
};

// Класс для инкапсуляции пользовательских настроек
class UserProfile {
private:
    string specialityField;
    string nameField;
    int subjectsCountField;

public:
    UserProfile() : specialityField("Программная инженерия"), 
                    nameField("Кристина"), 
                    subjectsCountField(5) {}

    string getSpeciality() const { return specialityField; }
    void setSpeciality(const string& s) { specialityField = s; }

    string getName() const { return nameField; }
    void setName(const string& n) { nameField = n; }

    int getSubjectsCount() const { return subjectsCountField; }
    void setSubjectsCount(int count) { subjectsCountField = count; }
};

// Вспомогательные функции
void flushInput() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void pauseAndReturn() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.get();
}

// Логические блоки программы
void executeStart() {
    cout << "\n=== Начало игры \"Моя жизнь в университете\" ===\n";
    cout << "Вы поступаете в университет. Удачи!\n";
    pauseAndReturn();
}

void executeLoad() {
    cout << "\nФункция загрузки пока недоступна.\n";
    cout << "Она появится в ближайших обновлениях.\n";
    pauseAndReturn();
}

void executeAbout() {
    cout << "\n=== О программе ===\n";
    cout << "Версия: " << Config::VERSION << "\n";
    cout << "Студент: Фамилия Имя\n";
    pauseAndReturn();
}

void executeSettings(UserProfile& profile) {
    int menuItem = 0;
    
    while (menuItem != OPT_BACK) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Специальность (текущая: " << profile.getSpeciality() << ")\n";
        cout << "2. Имя студента (текущее: " << profile.getName() << ")\n";
        cout << "3. Количество предметов (текущее: " << profile.getSubjectsCount() << ")\n";
        cout << "4. Назад\n";
        cout << "Выбор: ";
        
        cin >> menuItem;

        if (cin.fail()) {
            flushInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        switch (menuItem) {
            case OPT_CHANGE_SPECIALITY: {
                string inputVal;
                cout << "Введите специальность: ";
                flushInput();
                getline(cin, inputVal);
                profile.setSpeciality(inputVal);
                break;
            }
            case OPT_CHANGE_NAME: {
                string inputVal;
                cout << "Введите имя студента: ";
                flushInput();
                getline(cin, inputVal);
                profile.setName(inputVal);
                break;
            }
            case OPT_CHANGE_SUBJECTS: {
                int count = 0;
                cout << "Введите количество предметов: ";
                cin >> count;
                
                if (cin.fail() || count <= 0) {
                    flushInput();
                    cout << "Некорректное значение!\n";
                } else {
                    profile.setSubjectsCount(count);
                }
                break;
            }
            case OPT_BACK:
                break;
            default:
                cout << "Такого пункта нет.\n";
        }
    }
}

int main() {
    // Установка русской кодировки для Windows
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    UserProfile currentUser;
    int userChoice = 0;

    while (userChoice != OPT_EXIT) {
        cout << "\n*** Моя жизнь в университете ***\n";
        cout << "1. Начать игру\n";
        cout << "2. Загрузить игру\n";
        cout << "3. Настройки\n";
        cout << "4. О программе\n";
        cout << "5. Выход\n";
        cout << "Выбор: ";
        
        cin >> userChoice;

        if (cin.fail()) {
            flushInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        if (userChoice == OPT_START) {
            executeStart();
        } else if (userChoice == OPT_LOAD) {
            executeLoad();
        } else if (userChoice == OPT_SETTINGS) {
            executeSettings(currentUser);
        } else if (userChoice == OPT_ABOUT) {
            executeAbout();
        } else if (userChoice == OPT_EXIT) {
            cout << "Выход из программы...\n";
        } else {
            cout << "Такого пункта нет.\n";
        }
    }

    return 0;
}
