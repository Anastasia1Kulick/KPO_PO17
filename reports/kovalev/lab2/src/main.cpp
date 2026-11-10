#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <cstdlib>

using namespace std;

const string STUDENT_NAME = "Александр Ковалев";
const string VERSION = "2.0";

const int START_GAME = 1;
const int LOAD_GAME = 2;
const int SETTINGS = 3;
const int ABOUT = 4;
const int EXIT = 5;

const int DIFFICULTY = 1;
const int ANIMALS = 2;
const int ADD_ANIMAL = 3;
const int BACK = 4;

const string DEFAULT_DIFFICULTY = "Нормально";
const int MIN_COUNT = 1;
const int MAX_COUNT = 100;
const string ANIMAL_PREFIX = "animal.";

string settingsFile = "settings.txt";
string difficulty = DEFAULT_DIFFICULTY;
map<string, int> animals;

void setDefaults()
{
    difficulty = DEFAULT_DIFFICULTY;
    animals.clear();
    animals["Корова"] = 3;
    animals["Курица"] = 5;
}

bool saveSettings()
{
    ofstream out(settingsFile.c_str());
    if (!out.is_open())
    {
        cout << "Ошибка: не удалось записать файл настроек " << settingsFile << ".\n";
        return false;
    }

    out << "difficulty=" << difficulty << "\n";
    for (const auto& animal : animals)
        out << ANIMAL_PREFIX << animal.first << "=" << animal.second << "\n";
    return true;
}

bool isValidDifficulty(const string& s)
{
    return s == "Легко" || s == "Нормально" || s == "Сложно";
}

void loadSettings()
{
    setDefaults();

    ifstream in(settingsFile.c_str());
    if (!in.is_open())
    {
        cout << "Файл настроек не найден. Создан " << settingsFile
             << " с настройками по умолчанию.\n";
        saveSettings();
        return;
    }

    string loadedDifficulty;
    map<string, int> loadedAnimals;
    bool hasErrors = false;
    string line;

    while (getline(in, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;

        size_t pos = line.find('=');
        if (pos == string::npos)
        {
            cout << "Предупреждение: пропущена неверная строка в файле настроек: " << line << "\n";
            hasErrors = true;
            continue;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        if (key == "difficulty")
        {
            loadedDifficulty = value;
        }
        else if (key.size() > ANIMAL_PREFIX.size()
                 && key.compare(0, ANIMAL_PREFIX.size(), ANIMAL_PREFIX) == 0)
        {
            try
            {
                size_t used = 0;
                int count = stoi(value, &used);
                if (used != value.size() || count < MIN_COUNT || count > MAX_COUNT)
                    throw out_of_range("count");
                loadedAnimals[key.substr(ANIMAL_PREFIX.size())] = count;
            }
            catch (const exception&)
            {
                cout << "Предупреждение: пропущена неверная строка в файле настроек: " << line << "\n";
                hasErrors = true;
            }
        }
        else
        {
            cout << "Предупреждение: пропущена неизвестная строка в файле настроек: " << line << "\n";
            hasErrors = true;
        }
    }

    if (isValidDifficulty(loadedDifficulty))
    {
        difficulty = loadedDifficulty;
    }
    else
    {
        cout << "Предупреждение: уровень сложности в файле неверный, взят по умолчанию.\n";
        hasErrors = true;
    }

    if (!loadedAnimals.empty())
        animals = loadedAnimals;
    else
        hasErrors = true;

    if (hasErrors)
        saveSettings();
}

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

string trim(const string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
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
    cout << "Уровень сложности: " << difficulty << "\n";
    cout << "Животные на ферме:\n";
    for (const auto& animal : animals)
        cout << "  " << animal.first << ": " << animal.second << "\n";
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

    switch (choice)
    {
        case 1: difficulty = "Легко"; break;
        case 2: difficulty = "Нормально"; break;
        case 3: difficulty = "Сложно"; break;
        default:
            cout << "Ошибка: такого варианта нет.\n";
            return;
    }
    saveSettings();
    cout << "Уровень сложности изменён.\n";
}

void changeAnimalCount(const string& name)
{
    cout << "\n";
    cout << "===== " << name << " =====\n";
    cout << "Введите количество (от " << MIN_COUNT << " до " << MAX_COUNT << "): ";
    int count = readInt();

    if (count < MIN_COUNT || count > MAX_COUNT)
    {
        cout << "Ошибка: количество должно быть от " << MIN_COUNT << " до " << MAX_COUNT << ".\n";
        return;
    }
    animals[name] = count;
    saveSettings();
    cout << "Количество изменено.\n";
}

void animalsMenu()
{
    vector<string> names;
    cout << "\n";
    cout << "===== ЖИВОТНЫЕ =====\n";
    for (const auto& animal : animals)
    {
        names.push_back(animal.first);
        cout << names.size() << ". " << animal.first << " (сейчас: " << animal.second << ")\n";
    }
    cout << "Выберите животное: ";
    int choice = readInt();

    if (choice < 1 || choice > (int)names.size())
    {
        cout << "Ошибка: такого варианта нет.\n";
        return;
    }
    changeAnimalCount(names[choice - 1]);
}

void addAnimal()
{
    cout << "\n";
    cout << "===== НОВОЕ ЖИВОТНОЕ =====\n";
    cout << "Введите название вида: ";
    string name;
    if (!getline(cin, name))
        exit(0);
    name = trim(name);

    if (name.empty() || name.find('=') != string::npos)
    {
        cout << "Ошибка: неверное название.\n";
        return;
    }
    if (animals.count(name) > 0)
    {
        cout << "Ошибка: такой вид уже есть.\n";
        return;
    }

    cout << "Введите начальное количество (от " << MIN_COUNT << " до " << MAX_COUNT << "): ";
    int count = readInt();
    if (count < MIN_COUNT || count > MAX_COUNT)
    {
        cout << "Ошибка: количество должно быть от " << MIN_COUNT << " до " << MAX_COUNT << ".\n";
        return;
    }

    animals[name] = count;
    saveSettings();
    cout << "Вид добавлен и записан в файл настроек.\n";
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
        cout << "2. Количество животных\n";
        cout << "3. Добавить новый вид животного\n";
        cout << "4. Назад\n";
        cout << "=============================\n";
        cout << "Выберите пункт: ";
        choice = readInt();

        switch (choice)
        {
            case DIFFICULTY: changeDifficulty(); break;
            case ANIMALS: animalsMenu(); break;
            case ADD_ANIMAL: addAnimal(); break;
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
    cout << "Лабораторная работа №2\n";
    cout << "Автор: " << STUDENT_NAME << "\n";
    waitForInput();
}

int main()
{
    loadSettings();

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
