#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

const int Start = 1;
const int Load = 2;
const int Setting = 3;
const int Info = 4;
const int Exit = 5;

const int Specialty = 1;
const int Name = 2;
const int Subjects = 3;
const int Back = 4;

struct SETTING {
    string specialty = "Программная инженерия";
    string studentName = "Аня";
    int subjectCount = 11;
};

void startGame();
void loadGame();
void settingGame(SETTING& set);
void infoGame();
void exitGame();

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SETTING userSetting;
    int choice;

    while (true) {
        system("cls");
        cout << " ------------------------------" << endl;
        cout << "    МОЯ ЖИЗНЬ В УНИВЕРСИТЕТЕ   " << endl;
        cout << " ------------------------------" << endl;
        cout << "   " << Start << ". Начать игру              " << endl;
        cout << "   " << Load << ". Загрузить игру           " << endl;
        cout << "   " << Setting << ". Настройки                " << endl;
        cout << "   " << Info << ". О программе              " << endl;
        cout << "   " << Exit << ". Выход                    " << endl;
        cout << " ------------------------------" << endl;
        cout << "   Ваш выбор (1-5): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            continue;
        }

        switch(choice) {
        case Start:
            startGame();
            break;
        case Load:
            loadGame();
            break;
        case Setting:
            settingGame(userSetting);
            break;
        case Info:
            infoGame();
            break;
        case Exit:
            exitGame();
            return 0;
            break;
        }
    }
    return 0;
}

void startGame() {
    system("cls");
    cout << "\n\n\n ------------------------------" << endl;
    cout << "         Игра началась!        " << endl;
    cout << " ------------------------------\n\n\n" << endl;
    cout << " Нажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void loadGame() {
    system("cls");
    cout << "\n ------------------------------" << endl;
    cout << "         Данная функция        " << endl;
    cout << "        на данный момент       " << endl;
    cout << "          не доступна,         " << endl;
    cout << "     и появится в ближайших    " << endl;
    cout << "           обновлениях         " << endl;
    cout << " ------------------------------\n" << endl;
    cout << " Нажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void settingGame(SETTING& set) {
    int choice;
    while (true) {
        system("cls");
        cout << " ---------------------------------------------" << endl;
        cout << "                  НАСТРОЙКИ                   " << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  1. Специальность: " << set.specialty << endl;
        cout << "  2. Имя студента: " << set.studentName << endl;
        cout << "  3. Количество предметов: " << set.subjectCount << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  4. Назад в главное меню       " << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  Выберите пункт для изменения: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            continue;
        }
        cin.ignore();

        switch(choice) {
        case Specialty:
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите новую специальность: ";
            getline(cin, set.specialty);
            cout << " ---------------------------------------------" << endl;
            cout << "  Для сохранения нажмите enter ...";
            cin.get();
            break;
        case Name:
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите новое имя студента: ";
            getline(cin, set.studentName);
            cout << " ---------------------------------------------" << endl;
            cout << "  Для сохранения нажмите enter ...";
            cin.get();
            break;
        case Subjects:
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите новое количество предметов: ";
            cin >> set.subjectCount;
            cin.ignore();
            cout << " ---------------------------------------------" << endl;
            cout << "  Для сохранения нажмите enter ...";
            cin.get();
            break;
        case Back:
            return;
        }
    } 
}

void infoGame() {
    system("cls");
    cout << "\n ------------------------------" << endl;
    cout << "          О ПРОГРАММЕ          " << endl;
    cout << " ------------------------------" << endl;
    cout << "   версия: 1.0.0               " << endl;
    cout << "   выполнила: Киричун Анна     " << endl;
    cout << " ------------------------------\n\n\n" << endl;
    cout << " Нажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void exitGame() {
    cout << " ------------------------------" << endl;
    cout << "   Выход из программы ...      " << endl;
}
