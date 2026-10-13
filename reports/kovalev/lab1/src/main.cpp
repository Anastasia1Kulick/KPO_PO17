#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

using namespace std;

// Данные студента
const string STUDENT_NAME = "Александр Ковалев";
const string VERSION = "1.0";              // менять после каждой лабы

// Константы главного меню
const int START_GAME = 1;
const int LOAD_GAME = 2;
const int SETTINGS = 3;
const int ABOUT = 4;
const int EXIT = 5;

// Константы меню настроек
const int DIFFICULTY = 1;
const int COWS = 2;
const int CHICKENS = 3;
const int BACK = 4;

// Настройки игры 
int difficulty = 2;
int cows = 3;
int chickens = 5;


int readInt()
{
    string line;
    if (!getline(cin, line))
        exit(0);

    stringstream ss(line);
    int value;
    char extra;
    if (!(ss >> value) || (ss >> extra))
        return -1;
    return value;
}


void waitForInput()
{
    cout << "\nНажмите Enter, чтобы вернуться в главное меню...";
    string line;
    if (!getline(cin, line))
        exit(0);
}


void mainMenu()
{
    cout << "\n";
    cout << "=============================\n";
    cout << "        ВЕСЕЛАЯ ФЕРМА\n";
    cout << "=============================\n";
    cout << "1. Начать игру\n";
    cout << "2. Загрузить игру\n";
    cout << "3. Настройки\n";
    cout << "4. О программе\n";
    cout << "5. Выход\n";
    cout << "=============================\n";
}


void startGame()
{
    cout << "\n";
    cout << "Игра началась!\n";
    cout << "Уровень сложности: ";
    if (difficulty == 1)
        cout << "Легко\n";
    else if (difficulty == 2)
        cout << "Нормально\n";
    else
        cout << "Сложно\n";
    cout << "Количество коров: " << cows << "\n";
    cout << "Количество кур: " << chickens << "\n";
    waitForInput();
}

void loadGame()
{
    cout << "\n";
    cout << "Данная функция пока недоступна.\n";
    cout << "Она появится в ближайших обновлениях.\n";
    waitForInput();
}

void changeDifficulty()
{
    cout << "\n";
    cout << "===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
    cout << "1. Легко\n";
    cout << "2. Нормально\n";
    cout << "3. Сложно\n";
    cout << "Выберите уровень: ";
    int choice = readInt();

    if (choice >= 1 && choice <= 3)
    {
        difficulty = choice;
        cout << "Уровень сложности изменён.\n";
    }
    else
    {
        cout << "Ошибка: такого варианта нет.\n";
    }
}

void changeCows()
{
    cout << "\n";
    cout << "===== КОЛИЧЕСТВО КОРОВ =====\n";
    cout << "1. 1 корова\n";
    cout << "2. 3 коровы\n";
    cout << "3. 5 коров\n";
    cout << "4. 10 коров\n";
    cout << "Выберите количество: ";
    int choice = readInt();

    switch (choice)
    {
        case 1: cows = 1; break;
        case 2: cows = 3; break;
        case 3: cows = 5; break;
        case 4: cows = 10; break;
        default:
            cout << "Ошибка: такого варианта нет.\n";
            return;
    }
    cout << "Количество коров изменено.\n";
}

void changeChickens()
{
    cout << "\n";
    cout << "===== КОЛИЧЕСТВО КУР =====\n";
    cout << "1. 2 курицы\n";
    cout << "2. 5 кур\n";
    cout << "3. 10 кур\n";
    cout << "4. 20 кур\n";
    cout << "Выберите количество: ";
    int choice = readInt();

    switch (choice)
    {
        case 1: chickens = 2; break;
        case 2: chickens = 5; break;
        case 3: chickens = 10; break;
        case 4: chickens = 20; break;
        default:
            cout << "Ошибка: такого варианта нет.\n";
            return;
    }
    cout << "Количество кур изменено.\n";
}

void settingsMenu()
{
    int choice = 0;
    while (choice != BACK)
    {
        cout << "\n";
        cout << "=============================\n";
        cout << "          НАСТРОЙКИ\n";
        cout << "=============================\n";
        cout << "1. Уровень сложности\n";
        cout << "2. Количество коров\n";
        cout << "3. Количество кур\n";
        cout << "4. Назад\n";
        cout << "=============================\n";
        cout << "Выберите пункт: ";
        choice = readInt();

        switch (choice)
        {
            case DIFFICULTY: changeDifficulty(); break;
            case COWS: changeCows(); break;
            case CHICKENS: changeChickens(); break;
            case BACK: cout << "Возврат в главное меню.\n"; break;
            default: cout << "Ошибка: такого пункта нет.\n";
        }
    }
}

void about()
{
    cout << "\n";
    cout << "=============================\n";
    cout << "        О ПРОГРАММЕ\n";
    cout << "=============================\n";
    cout << "Название: Весёлая ферма\n";
    cout << "Версия: " << VERSION << "\n";
    cout << "Лабораторная работа №1\n";
    cout << "Автор: " << STUDENT_NAME << "\n";
    waitForInput();
}
int main()
{
    int choice = 0;
    while (choice != EXIT)
    {
        mainMenu();
        cout << "Выберите пункт: ";
        choice = readInt();

        switch (choice)
        {
            case START_GAME: startGame(); break;
            case LOAD_GAME: loadGame(); break;
            case SETTINGS: settingsMenu(); break;
            case ABOUT: about(); break;
            case EXIT: cout << "\nПрограмма завершена.\n"; break;
            default: cout << "\nОшибка: такого пункта нет.\n";
        }
    }
    return 0;
}
