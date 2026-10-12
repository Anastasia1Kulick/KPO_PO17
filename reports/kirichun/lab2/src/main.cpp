#include <iostream>
#include <string>
#include <fstream>
#include <set>
#include <list>
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
void settingGame(SETTING& userSet, set<string>& specialtiesSet, list<string>& namesList);
void infoGame();
void exitGame();
void saveSettings(const SETTING& userSet);
void loadSettings(SETTING& userSet);
void loadSpecialties(set<string>& specialtiesSet);
void loadNames(list<string>& namesList);

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SETTING userSetting;
    loadSettings(userSetting);

    set<string> specialtiesSet;
    list<string> namesList;
    loadSpecialties(specialtiesSet);
    loadNames(namesList);

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

        switch (choice) {
        case Start:
            startGame();
            break;
        case Load:
            loadGame();
            break;
        case Setting:
            settingGame(userSetting, specialtiesSet, namesList);
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

void settingGame(SETTING& userSet, set<string>& specialtiesSet, list<string>& namesList) {
    int choice;
    while (true) {
        system("cls");
        cout << " ---------------------------------------------" << endl;
        cout << "                  НАСТРОЙКИ                   " << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  1. Специальность: " << userSet.specialty << endl;
        cout << "  2. Имя студента: " << userSet.studentName << endl;
        cout << "  3. Количество предметов: " << userSet.subjectCount << endl;
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

        switch (choice) {
        case Specialty: {
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Доступные специальности (из Set):" << endl;
            int index = 1;
            for (const auto& sp : specialtiesSet) {
                cout << "  " << index++ << ". " << sp << endl;
            }
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите номер специальности: ";
            int pos;
            if (cin >> pos) {
                auto it = specialtiesSet.begin();
                for (int i = 1; i < pos && it != specialtiesSet.end(); ++i) {
                    ++it;
                }
                if (it != specialtiesSet.end()) {
                    userSet.specialty = *it;
                    saveSettings(userSet);
                }
            }
            cin.ignore();
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
            cin.get();
            break;
        }
        case Name: {
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Доступные имена студентов (из List):" << endl;
            int index = 1;
            for (const auto& name : namesList) {
                cout << "  " << index++ << ". " << name << endl;
            }
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите номер имени: ";
            int pos;
            if (cin >> pos) {
                auto it = namesList.begin();
                for (int i = 1; i < pos && it != namesList.end(); ++i) {
                    ++it;
                }
                if (it != namesList.end()) {
                    userSet.studentName = *it;
                    saveSettings(userSet);
                }
            }
            cin.ignore();
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
            cin.get();
            break;
        }
        case Subjects:
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите новое количество предметов: ";
            cin >> userSet.subjectCount;
            cin.ignore();
            saveSettings(userSet);
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
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
    cout << "   версия: 2.0.0               " << endl;
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

void saveSettings(const SETTING& userSet) {
    ofstream outFile("settings.txt");
    if (outFile.is_open()) {
        outFile << userSet.specialty << endl;
        outFile << userSet.studentName << endl;
        outFile << userSet.subjectCount << endl;
        outFile.close();
    }
}

void loadSettings(SETTING& userSet) {
    ifstream inFile("settings.txt");
    if (inFile.is_open()) {
        getline(inFile, userSet.specialty);
        getline(inFile, userSet.studentName);
        inFile >> userSet.subjectCount;
        inFile.close();
    }
    else {
        saveSettings(userSet);
    }
}

void loadSpecialties(set<string>& specialtiesSet) {
    ifstream inFile("specialties.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                specialtiesSet.insert(item);
            }
        }
        inFile.close();
    }
}

void loadNames(list<string>& namesList) {
    ifstream inFile("names.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                namesList.push_back(item);
            }
        }
        inFile.close();
    }
}
