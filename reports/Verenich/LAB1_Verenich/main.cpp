#include <iostream>
#include <string>
#include <limits>

using namespace std;

// Константы главного меню
const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

// Константы подменю настроек
const int SET_DIFFICULTY = 1;   // Уровень сложности
const int SET_COWS = 2;   // Коровы
const int SET_CHICKENS = 3;   // Курицы
const int SET_BACK = 4;   // Назад

// Константы уровня сложности
const int DIFF_EASY = 1;
const int DIFF_NORMAL = 2;
const int DIFF_HARD = 3;

// Версия программы, ФИО
const string PROGRAM_VERSION = "1.0";
const string STUDENT_NAME = "Веренич Даниил";

// Глобальные настройки
int difficulty = DIFF_NORMAL;  // текущая сложность
int cowsCount = 5;            // начальное количество коров
int chickensCount = 10;           // начальное количество куриц

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Преобразование сложности в строку для отображения
string difficultyToString(int d)
{
    switch (d)
    {
    case DIFF_EASY:   return "Легкий";
    case DIFF_NORMAL: return "Средний";
    case DIFF_HARD:   return "Сложный";
    default:          return "Неизвестно";
    }
}

// Подменю: Уровень сложности
void difficultySubmenu()
{
    while (true)
    {
        cout << "\n===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
        cout << "1. Легкий\n";
        cout << "2. Средний\n";
        cout << "3. Сложный\n";
        cout << "4. Назад\n";
        cout << "Текущий уровень: " << difficultyToString(difficulty) << "\n";
        cout << "Выберите пункт: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        switch (choice)
        {
        case DIFF_EASY:
            difficulty = DIFF_EASY;
            cout << "Уровень сложности установлен: Легкий\n";
            break;
        case DIFF_NORMAL:
            difficulty = DIFF_NORMAL;
            cout << "Уровень сложности установлен: Средний\n";
            break;
        case DIFF_HARD:
            difficulty = DIFF_HARD;
            cout << "Уровень сложности установлен: Сложный\n";
            break;
        case SET_BACK:
            return;
        default:
            cout << "Неверный пункт меню!\n";
            break;
        }
    }
}

// Подменю: начальное начальное количество животных
void animalCountSubmenu(const string& animalName, int& value)
{
    while (true)
    {
        cout << "\n===== НАЧАЛЬНОЕ КОЛИЧЕСТВО: " << animalName << " =====\n";
        cout << "Текущее значение: " << value << "\n";
        cout << "Введите новое количество (0..1000) или -1 для возврата: ";

        int newValue;
        cin >> newValue;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        if (newValue == -1)
            return;

        if (newValue < 0 || newValue > 1000)
        {
            cout << "Недопустимое значение! Диапазон: 0..1000.\n";
            continue;
        }

        value = newValue;
        cout << "Количество (" << animalName << ") установлено: " << value << "\n";
        return;
    }
}

// Подменю настроек
void settingsMenu()
{
    while (true)
    {
        cout << "\n===== НАСТРОЙКИ =====\n";
        cout << SET_DIFFICULTY << ". Уровень сложности (текущий: "
            << difficultyToString(difficulty) << ")\n";
        cout << SET_COWS << ". Начальное количество коров (текущее: "
            << cowsCount << ")\n";
        cout << SET_CHICKENS << ". Начальное количество куриц (текущее: "
            << chickensCount << ")\n";
        cout << SET_BACK << ". Назад\n";
        cout << "Выберите пункт: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        switch (choice)
        {
        case SET_DIFFICULTY:
            difficultySubmenu();
            break;
        case SET_COWS:
            animalCountSubmenu("Коровы", cowsCount);
            break;
        case SET_CHICKENS:
            animalCountSubmenu("Курицы", chickensCount);
            break;
        case SET_BACK:
            return;
        default:
            cout << "Неверный пункт меню!\n";
            break;
        }
    }
}

// Пункты главного меню
void startGame()
{
    cout << "\n=== НАЧАЛО ИГРЫ ===\n";
    cout << "Игра \"Веселая ферма\" запускается!\n";
    cout << "Уровень сложности: " << difficultyToString(difficulty) << "\n";
    cout << "Коров: " << cowsCount << ", Куриц: " << chickensCount << "\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    clearInput();
    cin.get();
}

void loadGame()
{
    cout << "\n=== ЗАГРУЗКА ИГРЫ ===\n";
    cout << "Данная функция на данный момент недоступна\n";
    cout << "и появится в ближайших обновлениях.\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    clearInput();
    cin.get();
}

void aboutProgram()
{
    cout << "\n=== О ПРОГРАММЕ ===\n";
    cout << "Название: Веселая ферма\n";
    cout << "Версия программы: " << PROGRAM_VERSION << "\n";
    cout << "Студент: " << STUDENT_NAME << "\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    clearInput();
    cin.get();
}

// Главное меню
void showMainMenu()
{
    cout << "\n******************************\n";
    cout << "*        ВЕСЕЛАЯ ФЕРМА       *\n";
    cout << "******************************\n";
    cout << MENU_START_GAME << ". Начать игру\n";
    cout << MENU_LOAD_GAME << ". Загрузить игру\n";
    cout << MENU_SETTINGS << ". Настройки\n";
    cout << MENU_ABOUT << ". О программе\n";
    cout << MENU_EXIT << ". Выход\n";
    cout << "Выберите пункт меню: ";
}

int main()
{
    setlocale(LC_ALL, "Russian");

    while (true)
    {
        showMainMenu();

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Введите число.\n";
            continue;
        }

        switch (choice)
        {
        case MENU_START_GAME:
            startGame();
            break;
        case MENU_LOAD_GAME:
            loadGame();
            break;
        case MENU_SETTINGS:
            settingsMenu();
            break;
        case MENU_ABOUT:
            aboutProgram();
            break;
        case MENU_EXIT:
            cout << "Выход из программы. До свидания!\n";
            return 0;
        default:
            cout << "Неверный пункт меню! Попробуйте снова.\n";
            break;
        }
    }

    return 0;
}