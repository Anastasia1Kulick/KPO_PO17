#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <limits>
#include <algorithm>
#include <clocale>

#ifdef _WIN32
#define NOMINMAX         
#include <windows.h>
#endif

using namespace std;

// ==================== КОНСТАНТЫ ГЛАВНОГО МЕНЮ ====================
const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

// ==================== КОНСТАНТЫ ПОДМЕНЮ НАСТРОЕК ====================
const int SET_DIFFICULTY = 1;
const int SET_ANIMALS = 2;
const int SET_ADD_ANIMAL = 3;
const int SET_BACK = 4;

// ==================== КОНСТАНТЫ ПОДМЕНЮ ЖИВОТНЫХ ====================
const int ANIMAL_EDIT = 1;
const int ANIMAL_DELETE = 2;
const int ANIMAL_BACK = 3;

// ==================== КОНСТАНТЫ УРОВНЯ СЛОЖНОСТИ ====================
const int DIFF_COUNT = 3;

// ==================== ВЕРСИЯ ПРОГРАММЫ ====================
const string PROGRAM_VERSION = "2.0";
const string STUDENT_NAME = "Иван Иванов";

// ==================== ИМЯ ФАЙЛА НАСТРОЕК ====================
const string SETTINGS_FILE = "settings.txt";

// ==================== ГЛОБАЛЬНЫЕ НАСТРОЙКИ ====================
string difficulty = "Medium";
map<string, int> animals;

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================
void setupLocale()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
}

void clearInput()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool isValidDifficulty(const string& d)
{
    return d == "Easy" || d == "Medium" || d == "Hard";
}

// ==================== РАБОТА С ФАЙЛОМ НАСТРОЕК ====================

void setDefaults()
{
    difficulty = "Medium";
    animals.clear();
    animals["Cows"] = 5;
    animals["Chickens"] = 10;
}

void saveSettings()
{
    ofstream fout(SETTINGS_FILE);
    if (!fout.is_open())
    {
        cout << "Ошибка: не удалось открыть файл " << SETTINGS_FILE << " для записи!\n";
        return;
    }

    fout << "difficulty=" << difficulty << "\n";

    for (const auto& pair : animals)
    {
        fout << "animals=" << pair.first << ":" << pair.second << "\n";
    }

    fout.close();
}

bool loadSettings()
{
    ifstream fin(SETTINGS_FILE);
    if (!fin.is_open())
        return false;

    string line;
    string loadedDifficulty;
    map<string, int> loadedAnimals;

    while (getline(fin, line))
    {
        if (line.empty()) continue;

        size_t eqPos = line.find('=');
        if (eqPos == string::npos) continue;

        string key = line.substr(0, eqPos);
        string value = line.substr(eqPos + 1);

        if (key == "difficulty")
        {
            if (isValidDifficulty(value))
                loadedDifficulty = value;
        }
        else if (key == "animals")
        {
            size_t colonPos = value.find(':');
            if (colonPos == string::npos) continue;

            string animalName = value.substr(0, colonPos);
            string countStr = value.substr(colonPos + 1);

            try
            {
                int count = stoi(countStr);
                if (count >= 0 && count <= 1000)
                    loadedAnimals[animalName] = count;
            }
            catch (...)
            {
                // Пропускаем некорректные строки
            }
        }
    }

    fin.close();

    if (!loadedDifficulty.empty())
        difficulty = loadedDifficulty;

    if (!loadedAnimals.empty())
        animals = loadedAnimals;

    return true;
}

void initSettings()
{
    if (!loadSettings())
    {
        cout << "Файл настроек " << SETTINGS_FILE
            << " не найден. Созданы настройки по умолчанию.\n";
        setDefaults();
        saveSettings();
    }
}

// ==================== ПОДМЕНЮ: УРОВЕНЬ СЛОЖНОСТИ ====================
void difficultySubmenu()
{
    const string options[DIFF_COUNT] = { "Easy", "Medium", "Hard" };

    while (true)
    {
        cout << "\n===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
        for (int i = 0; i < DIFF_COUNT; i++)
            cout << (i + 1) << ". " << options[i] << "\n";
        cout << (DIFF_COUNT + 1) << ". Назад\n";
        cout << "Текущий уровень: " << difficulty << "\n";
        cout << "Выберите пункт: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        if (choice >= 1 && choice <= DIFF_COUNT)
        {
            difficulty = options[choice - 1];
            cout << "Уровень сложности установлен: " << difficulty << "\n";
            saveSettings();
        }
        else if (choice == DIFF_COUNT + 1)
        {
            return;
        }
        else
        {
            cout << "Неверный пункт меню!\n";
        }
    }
}

// ==================== ПОДМЕНЮ: УПРАВЛЕНИЕ ЖИВОТНЫМИ ====================
void animalsSubmenu()
{
    while (true)
    {
        cout << "\n===== ЖИВОТНЫЕ НА ФЕРМЕ =====\n";
        if (animals.empty())
        {
            cout << "(список пуст)\n";
        }
        else
        {
            int idx = 1;
            for (const auto& pair : animals)
            {
                cout << idx++ << ". " << pair.first
                    << " — " << pair.second << "\n";
            }
        }
        cout << "0. Назад\n";
        cout << "Выберите животное для редактирования (или 0 для выхода): ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            clearInput();
            cout << "Ошибка ввода! Попробуйте снова.\n";
            continue;
        }

        if (choice == 0)
            return;

        if (choice < 1 || choice >(int)animals.size())
        {
            cout << "Неверный пункт меню!\n";
            continue;
        }

        auto it = animals.begin();
        advance(it, choice - 1);
        string animalName = it->first;

        while (true)
        {
            cout << "\n--- " << animalName
                << " (текущее количество: " << animals[animalName] << ") ---\n";
            cout << ANIMAL_EDIT << ". Изменить количество\n";
            cout << ANIMAL_DELETE << ". Удалить животное\n";
            cout << ANIMAL_BACK << ". Назад\n";
            cout << "Выберите пункт: ";

            int action;
            cin >> action;

            if (cin.fail())
            {
                clearInput();
                cout << "Ошибка ввода! Попробуйте снова.\n";
                continue;
            }

            if (action == ANIMAL_EDIT)
            {
                cout << "Введите новое количество (0..1000): ";
                int newCount;
                cin >> newCount;

                if (cin.fail())
                {
                    clearInput();
                    cout << "Ошибка ввода!\n";
                    continue;
                }

                if (newCount < 0 || newCount > 1000)
                {
                    cout << "Недопустимое значение! Диапазон: 0..1000.\n";
                    continue;
                }

                animals[animalName] = newCount;
                cout << "Количество (" << animalName << ") установлено: "
                    << newCount << "\n";
                saveSettings();
            }
            else if (action == ANIMAL_DELETE)
            {
                animals.erase(animalName);
                cout << "Животное \"" << animalName << "\" удалено.\n";
                saveSettings();
                break;
            }
            else if (action == ANIMAL_BACK)
            {
                break;
            }
            else
            {
                cout << "Неверный пункт меню!\n";
            }
        }
    }
}

// ==================== ПОДМЕНЮ: ДОБАВЛЕНИЕ ЖИВОТНОГО ====================
void addAnimalSubmenu()
{
    cout << "\n===== ДОБАВЛЕНИЕ НОВОГО ЖИВОТНОГО =====\n";

    cout << "Введите название животного (английскими буквами): ";
    string name;
    cin >> name;

    if (name.empty())
    {
        cout << "Название не может быть пустым!\n";
        return;
    }

    if (animals.count(name) > 0)
    {
        cout << "Животное \"" << name << "\" уже есть в списке!\n";
        return;
    }

    cout << "Введите начальное количество (0..1000): ";
    int count;
    cin >> count;

    if (cin.fail())
    {
        clearInput();
        cout << "Ошибка ввода!\n";
        return;
    }

    if (count < 0 || count > 1000)
    {
        cout << "Недопустимое значение! Диапазон: 0..1000.\n";
        return;
    }

    animals[name] = count;
    saveSettings();
    cout << "Животное \"" << name << "\" добавлено с количеством " << count << ".\n";
}

// ==================== ПОДМЕНЮ НАСТРОЕК ====================
void settingsMenu()
{
    while (true)
    {
        cout << "\n===== НАСТРОЙКИ =====\n";
        cout << SET_DIFFICULTY << ". Уровень сложности (текущий: "
            << difficulty << ")\n";
        cout << SET_ANIMALS << ". Управление животными ("
            << animals.size() << " видов)\n";
        cout << SET_ADD_ANIMAL << ". Добавить новый тип животного\n";
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
        case SET_ANIMALS:
            animalsSubmenu();
            break;
        case SET_ADD_ANIMAL:
            addAnimalSubmenu();
            break;
        case SET_BACK:
            return;
        default:
            cout << "Неверный пункт меню!\n";
            break;
        }
    }
}

// ==================== ПУНКТЫ ГЛАВНОГО МЕНЮ ====================
void startGame()
{
    cout << "\n=== НАЧАЛО ИГРЫ ===\n";
    cout << "Игра \"Веселая ферма\" запускается!\n";
    cout << "Уровень сложности: " << difficulty << "\n";
    cout << "Животные на ферме:\n";

    if (animals.empty())
    {
        cout << "  (нет животных)\n";
    }
    else
    {
        for (const auto& pair : animals)
            cout << "  " << pair.first << ": " << pair.second << "\n";
    }

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
    cout << "Файл настроек: " << SETTINGS_FILE << "\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    clearInput();
    cin.get();
}

// ==================== ГЛАВНОЕ МЕНЮ ====================
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
    setupLocale();

    initSettings();

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