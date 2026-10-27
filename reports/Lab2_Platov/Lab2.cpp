#include <iostream>
#include <fstream>
#include <queue>
#include <string>

using namespace std;

enum Menu
{
    START_GAME = 1,
    LOAD_GAME,
    SETTINGS,
    ABOUT,
    EXIT
};

enum SettingsMenu
{
    SHOW = 1,
    CITY_SIZE,
    BUDGET,
    POPULATION,
    SET_EXIT
};

queue<int> citySizeValues;
queue<int> budgetValues;
queue<int> populationValues;
const string SETTINGS_FILE = "settings.txt";

void createDefaultSettingsFile()
{
    ofstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Не удалось создать файл " << SETTINGS_FILE << endl;
        return;
    }
    file << "1000 5000 10000" << endl;
    file << "1000 5000 10000" << endl;
    file << "100 500 1000" << endl;
    file.close();
    cout << "Создан файл настроек по умолчанию: " << SETTINGS_FILE << endl;
}

void saveQueue(ofstream& file, queue<int> q)
{
    bool first = true;
    while (!q.empty())
    {
        if (!first) file << " ";
        file << q.front();
        q.pop();
        first = false;
    }
    file << "\n";
}

bool saveSettingsToFile()
{
    ofstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Не удалось сохранить файл " << SETTINGS_FILE << endl;
        return false;
    }
    saveQueue(file, citySizeValues);
    saveQueue(file, budgetValues);
    saveQueue(file, populationValues);
    file.close();
    return true;
}

bool loadSettingsFromFile()
{
    ifstream file(SETTINGS_FILE);
    if (!file.is_open())
    {
        cout << "Файл " << SETTINGS_FILE
            << " не найден. Создаём настройки по умолчанию..." << endl;
        createDefaultSettingsFile();
        file.open(SETTINGS_FILE);
        if (!file.is_open())
        {
            cout << "Не удалось открыть файл " << SETTINGS_FILE << endl;
            return false;
        }
    }
    int value = 0;
    while (file.peek() != '\n' && file >> value)
    {
        citySizeValues.push(value);
    }
    file.ignore(10000, '\n');
    while (file.peek() != '\n' && file >> value)
    {
        budgetValues.push(value);
    }
    file.ignore(10000, '\n');
    while (file >> value)
    {
        populationValues.push(value);
    }
    file.close();
    if (citySizeValues.empty() || budgetValues.empty() || populationValues.empty())
    {
        cout << "Файл настроек пуст или имеет неверный формат." << endl;
        return false;
    }
    return true;
}

void nextValue(queue<int>& q, int& current)
{
    if (q.empty()) return;
    size_t n = q.size();
    while (n-- > 0 && q.front() != current)
    {
        q.push(q.front());
        q.pop();
    }
    int oldValue = q.front();
    q.pop();
    current = q.front();
    q.push(oldValue);
}

void Menu()
{
    cout << "==== ГОРОД МЕЧТЫ ====" << endl;
    cout << START_GAME << ". Начать игру" << endl;
    cout << LOAD_GAME << ". Загрузить игру" << endl;
    cout << SETTINGS << ". Настройки" << endl;
    cout << ABOUT << ". О программе" << endl;
    cout << EXIT << ". Выход" << endl;
    cout << "=====================" << endl;
}

void MenuSettings()
{
    cout << "========= НАСТРОЙКИ =========" << endl;
    cout << SHOW << ". Просмотр текущих настроек" << endl;
    cout << CITY_SIZE << ". Размер города" << endl;
    cout << BUDGET << ". Начальный бюджет" << endl;
    cout << POPULATION << ". Количество жителей" << endl;
    cout << SET_EXIT << ". Выход" << endl;
    cout << "=============================" << endl;
}

void aboutProgramm()
{
    cout << "====== О ПРОГРАММЕ ======" << endl;
    cout << "Игра:    «Город мечты»" << endl;
    cout << "Версия:  2.0" << endl;
    cout << "Студент: Платов Александр" << endl;
    cout << "=========================" << endl;
}

void showSettings(int citySize, int budget, int population)
{
    cout << "Текущий размер города: " << citySize << endl;
    cout << "Текущий бюджет города: " << budget << endl;
    cout << "Текущее количество жителей города: " << population << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    if (!loadSettingsFromFile())
    {
        cout << "Программа завершена: не удалось загрузить настройки." << endl;
        system("pause");
        return 1;
    }
    int choice = -1;
    int citySize = citySizeValues.front();
    int budget = budgetValues.front();
    int population = populationValues.front();
    while (choice != EXIT)
    {
        Menu();
        cin >> choice;
        system("cls");
        switch (choice)
        {
        case START_GAME:
        {
            cout << "Игра началась!" << endl;
            system("pause");
            system("cls");
            break;
        }
        case LOAD_GAME:
        {
            cout << "Данная функция на данный момент недоступна и появится в ближайших обновлениях." << endl;
            system("pause");
            system("cls");
            break;
        }
        case SETTINGS:
        {
            int choice1 = -1;
            while (choice1 != SET_EXIT)
            {
                MenuSettings();
                cin >> choice1;
                system("cls");
                switch (choice1)
                {
                case SHOW:
                {
                    showSettings(citySize, budget, population);
                    system("pause");
                    system("cls");
                    break;
                }
                case CITY_SIZE:
                {
                    cout << "Текущий размер города: " << citySize << endl;
                    nextValue(citySizeValues, citySize);
                    cout << "Размер города после изменений: " << citySize << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                }
                case BUDGET:
                {
                    cout << "Текущий бюджет города: " << budget << endl;
                    nextValue(budgetValues, budget);
                    cout << "Бюджет города после изменений: " << budget << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                }
                case POPULATION:
                {
                    cout << "Текущее количество жителей города: " << population << endl;
                    nextValue(populationValues, population);
                    cout << "Количество жителей города после изменений: " << population << endl;
                    saveSettingsToFile();
                    system("pause");
                    system("cls");
                    break;
                }
                case SET_EXIT:
                {
                    cout << "Выход из настроек." << endl;
                    system("pause");
                    system("cls");
                    break;
                }
                default:
                {
                    cout << "Неверный пункт меню. Попробуйте еще раз." << endl;
                    system("pause");
                    system("cls");
                    break;
                }
                }
            }
            break;
        }
        case ABOUT:
        {
            aboutProgramm();
            system("pause");
            system("cls");
            break;
        }
        case EXIT:
        {
            cout << "Выход из программы." << endl;
            break;
        }
        default:
        {
            cout << "Неверный пункт меню. Попробуйте еще раз." << endl;
            system("pause");
            system("cls");
            break;
        }
        }
    }
    return 0;
}