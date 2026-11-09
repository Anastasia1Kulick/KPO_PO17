#include <iostream>

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

struct Settings
{
    int citySize = 1000;
    int budget = 1000;
    int population = 100;
};

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
    cout << "Версия:  1.0" << endl;
    cout << "Студент: Платов Александр" << endl;
    cout << "=========================" << endl;
}

void showSettings(const Settings& settings)
{
    cout << "Текущий размер города: " << settings.citySize << endl;
    cout << "Текущий бюджет города: " << settings.budget << endl;
    cout << "Текущее количество жителей города: " << settings.population << endl;
}

int main()
{
    setlocale(LC_ALL, "ru");
    int choice = -1;
    Settings settings;
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
                    showSettings(settings);
                    system("pause");
                    system("cls");
                    break;
                }
                case CITY_SIZE:
                {
                    cout << "Текущий размер города: " << settings.citySize << endl;
                    if (settings.citySize == 1000)      settings.citySize = 5000;
                    else if (settings.citySize == 5000) settings.citySize = 10000;
                    else                                settings.citySize = 1000;
                    cout << "Размер города после изменений: " << settings.citySize << endl;
                    system("pause");
                    system("cls");
                    break;
                }
                case BUDGET:
                {
                    cout << "Текущий бюджет города: " << settings.budget << endl;
                    if (settings.budget == 1000)      settings.budget = 5000;
                    else if (settings.budget == 5000) settings.budget = 10000;
                    else                              settings.budget = 1000;
                    cout << "Бюджет города после изменений: " << settings.budget << endl;
                    system("pause");
                    system("cls");
                    break;
                }
                case POPULATION:
                {
                    cout << "Текущее количество жителей города: " << settings.population << endl;
                    if (settings.population == 100)      settings.population = 500;
                    else if (settings.population == 500) settings.population = 1000;
                    else                                 settings.population = 100;
                    cout << "Количество жителей города после изменений: " << settings.population << endl;
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