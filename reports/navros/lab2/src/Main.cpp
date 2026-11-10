#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <clocale>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

//Менюшки для выборов
enum MainMenu{
    Start_Game = 1,
    Load_Game,
    Settings,
    About,
    Exit
};
enum OptionsMenu {
    Difficulty = 1,
    Size,
    Health,
    All_Settings,
    Restore_Size,
    Go_Back
};
enum DifficultyOptions {
    Easy = 1,
    Medium,
    Hard,
    VeryHard
};

//Чистит строки когда там возникают ошибки
void Cleaner () 
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

struct GameSettings
{
    int size;
    int health;
    string difficulty;
};

//Константы для игры
string difficulty = "Легко";
int fieldsize = 64;
int health = 3;
const float version = 2.0;
const string creator = "Наврось Глеб";
stack<GameSettings> history;
string settingsFile = "settings.txt";

//Границы значений, которые вводятся с клавиатуры
const int MIN_SIDE = 5;
const int MAX_SIDE = 20;
const int MIN_HEALTH = 1;
const int MAX_HEALTH = 10;

//Преобразует размер из полного значения в размеры сетки карты
string sizeConverter (int fieldsize)
{
    int conv = static_cast<int> (sqrt(fieldsize));
    return to_string(conv) + "x" + to_string(conv);
}
//Преобразование сложности в понятные слова
string diffConverter (int diff)
{
    switch (diff)
    {
    case Easy: return("Легко");
    case Medium: return("Средне");
    case Hard: return("Сложно");
    case VeryHard: return("Очень Сложно");
    default: return("Неизвестно");
    }
}
//Запись информации в файл с полной перезаписью его
void saveHistory()
{
    stack<GameSettings> copy = history;      // делаем всякое с копией чтобы недай бог не повредить оригинал
    vector<GameSettings> items;
    while (!copy.empty()) { items.push_back(copy.top()); copy.pop(); }

    ofstream out(settingsFile);
    for (int i = static_cast<int>(items.size()) - 1; i >= 0; i--)   // от старого к новому
        out << items[i].size << " " << items[i].health << " " << items[i].difficulty << "\n";
}
// Проверка что прочитанная из файла запись имеет допустимые значения
bool settingsAreValid(const GameSettings& s)
{
    if (s.size < MIN_SIDE * MIN_SIDE || s.size > MAX_SIDE * MAX_SIDE) return false;
    int side = static_cast<int>(round(sqrt(s.size)));
    if (side * side != s.size) return false;                       // размер должен быть квадратом
    if (s.health < MIN_HEALTH || s.health > MAX_HEALTH) return false;
    for (int i = Easy; i <= VeryHard; i++)
        if (diffConverter(i) == s.difficulty) return true;
    return false;                                                  // неизвестная сложность
}
// Чтение файла в стек
void loadHistory()
{
    ifstream in(settingsFile);
    string line;
    while (getline(in, line))
    {
        istringstream iss(line);
        GameSettings s;
        if (!(iss >> s.size >> s.health)) continue;     // если в начале не 2 числа значит чтото сломалось, скип
        getline(iss >> ws, s.difficulty);               // забираем нашу настройку сложности через getline тк у нас есть сложность состоящая из 2 слов
        if (!s.difficulty.empty() && s.difficulty.back() == '\r') s.difficulty.pop_back();   // на случай файла с CRLF
        if (!settingsAreValid(s)) continue;             // значения вне допустимых границ значит файл испорчен, скип
        history.push(s);
    }
}
// Кладёт текущие настройки в стек и файл, если они отличаются от верхней записи
void commitSettings()
{
    if (!history.empty())
    {
        const GameSettings& t = history.top();
        if (t.size == fieldsize && t.health == health && t.difficulty == difficulty)
            return;
    }
    history.push({fieldsize, health, difficulty});
    saveHistory();
}

void StartGame()
{
    cout << "\n***НАЧАЛО ИГРЫ***\n";
    cout << "Игра начинается...\n";
    cout << "Сложность игры: " << difficulty <<"\n";
    cout << "Размер карты подземелья: "<< sizeConverter(fieldsize) << "\n";
    cout << "Количество очков здоровья: " << health << "\n";
    cout << "Нажмите Enter для продолжения";
    Cleaner();
    cin.get();
}
void LoadGame()
{
    cout << "\n***ЗАГРУЗКА ИГРЫ***\n";
    cout << "Пока что данная функция недоступна. Ждите обновлений!\n";
    cout << "Нажмите Enter для продолжения";
    Cleaner();
    cin.get();
}
void DiffSubMenu()
{
    while (true)
    {
        cout << "\n===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
        cout << "Текущий уровень: " << difficulty << "\n";
        cout << "Введите номер уровня сложности:\n";
        cout << "1 - Легко, 2 - Средне, 3 - Сложно, 4 - Очень сложно\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < Easy || choice > VeryHard)
        {
            cout << "Недопустимое значение. Введите число от 1 до 4\n";
            continue;
        }
        difficulty = diffConverter(choice);
        cout << "Новый уровень сложности - " << difficulty << "\n";
        return;
    }
}

void SizeSubMenu()
{
    while (true)
    {
        cout << "\n***РАЗМЕР ПОДЗЕМЕЛЬЯ***\n";
        cout << "Текущий размер: " << sizeConverter(fieldsize) << "\n";
        cout << "Введите длину стороны карты (" << MIN_SIDE << "-" << MAX_SIDE << ")\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < MIN_SIDE || choice > MAX_SIDE)
        {
            cout << "Недопустимое значение. Значения могут варьироваться от " << MIN_SIDE << " до " << MAX_SIDE << "\n";
            continue;
        }
        fieldsize = choice * choice;
        cout << "Новый размер подземелья - " << sizeConverter(fieldsize) << "\n";
        return;
    }
}
void HealthSubMenu()
{
    while (true)
    {
        cout << "\n***Текущее количество очков здоровья: " << health << "\n";
        cout << "Введите необходимое количество очков здоровья (" << MIN_HEALTH << "-" << MAX_HEALTH << ")\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < MIN_HEALTH || choice > MAX_HEALTH)
        {
            cout << "Недопустимое значение. Значения могут варьироваться от " << MIN_HEALTH << " до " << MAX_HEALTH << "\n";
            continue;
        }
        health = choice;
        cout << "Количество очков здоровья изменено на " << health << "\n";
        return;
    }
    
}
//Последовательный ввод всех настроек с клавиатуры
void AllSettings()
{
    cout << "\n***ПОСЛЕДОВАТЕЛЬНАЯ НАСТРОЙКА***\n";
    cout << "Шаг 1 из 3\n";
    DiffSubMenu();
    cout << "Шаг 2 из 3\n";
    SizeSubMenu();
    cout << "Шаг 3 из 3\n";
    HealthSubMenu();
    cout << "Настройка завершена\n";
}
//Возврат к предыдущей записи из стека
void RestorePreviousSize()
{
    if (history.size() < 2)
    {
        cout << "Предыдущие значения не найдены.\n";
        return;
    }
    history.pop();                          // убираем текущую запись
    fieldsize = history.top().size;
    health = history.top().health;
    difficulty = history.top().difficulty;
    saveHistory();                          // файл снова совпадает со стеком
    cout << "Возвращены предыдущие значения. Размер карты: " << sizeConverter(fieldsize) << "\n";
}
void SettingsMenu ()
{
    while(true)
    {
        cout << "\n***НАСТРОЙКИ***\n";
        cout << "1.Изменить сложность (текущаяя сложность " << difficulty << " )\n";
        cout << "2.Изменить размер карты (текущий размер карты " << sizeConverter(fieldsize) << " )\n";
        cout << "3.Изменить количество очков здоровья (текущие очки здоровья " << health << " )\n";
        cout << "4.Задать все настройки по очереди\n";
        cout << "5.Вернуть предыдущий размер\n";
        cout << "6.Выход из настроек\n";
        cout << "Выберете нужное: ";

        int choice;
        cin >> choice;
        
        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        switch (choice)
        {
        case Difficulty: 
            DiffSubMenu();
            commitSettings();
            break;
        case Size: 
            SizeSubMenu();
            commitSettings();
            break;
        case Health: 
            HealthSubMenu();
            commitSettings();
            break;
        case All_Settings:
            AllSettings();
            commitSettings();
            break;
        case Restore_Size:
            RestorePreviousSize();
            break;
        case Go_Back: 
            return;
        default:
            cout << "Неверный пункт меню.\n";
            break;
        }
    }
}
void aboutProgramm ()
{
    cout << "\n***О ПРОГРАММЕ***\n";
    cout << "Название игры : Приключение в подземелье\n";
    cout << "Версия игры: " << version << "\n";
    cout << "Автор: " << creator << "\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    Cleaner();
    cin.get();
}
void ShowMainMenu()
{
    cout << "\n                    *****ПРИКЛЮЧЕНИЕ В ПОДЗЕМЕЛЬЕ*****\n";
    cout << "1.Начало игры\n";
    cout << "2.Загрузить игру\n";
    cout << "3.Настройки\n";
    cout << "4.Об игре\n";
    cout << "5.Выход\n";
    cout << "Выберите пункт меню: ";
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    loadHistory();
    if (history.empty())
    {
        commitSettings();                   // значения по умолчанию попадают в историю
    }
    else
    {
        fieldsize = history.top().size;
        health = history.top().health;
        difficulty = history.top().difficulty;
    }

    while (true)
    {
        ShowMainMenu();
        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }

        switch (choice)
        {
        case Start_Game:
            StartGame();
            break;
        case Load_Game:
            LoadGame();
            break;
        case Settings:
            SettingsMenu();
            break;
        case About:
            aboutProgramm();
            break;
        case Exit:
            cout << "Завершение работы.\n";
            return 0;
        default:
            cout << "Неверный пункт меню.\n";
            break;
        }
    }

    return 0;
}