#define NOMINMAX                 
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <set>
#include <vector>
#include <filesystem> // Для проверки существования файлов
#include <windows.h>

using namespace std;

// Идентификаторы меню
enum MenuItems { M_START = 1, M_LOAD, M_SETTINGS, M_ABOUT, M_EXIT };
enum SettingsItems { S_SPEC = 1, S_NAME, S_SUBJ, S_BACK };

// Глобальные константы
const string APP_VERSION = "2.0";
const string F_SETTINGS = "config.txt";
const string F_STUDENTS = "list_students.txt";
const string F_SPECS = "list_specialities.txt";

// Структура настроек
struct AppConfig {
    string specialty = "Программная инженерия";
    string student = "Кристина";
    int subjects = 5;
};

// Утилиты ввода
void discardLine() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void waitForEnter() {
    cout << "\n[Нажмите Enter для продолжения...]";
    cin.get();
}

// Инициализация файлов, если их нет
void checkAndCreateFiles(const AppConfig& defaultCfg) {
    if (!filesystem::exists(F_SETTINGS)) {
        ofstream out(F_SETTINGS);
        out << "specialty=" << defaultCfg.specialty << "\n"
            << "student=" << defaultCfg.student << "\n"
            << "subjects=" << defaultCfg.subjects << "\n";
    }
    if (!filesystem::exists(F_STUDENTS)) {
        ofstream out(F_STUDENTS);
        out << "Кристина\nИван\nАнна\nПетр\nОльга\n";
    }
    if (!filesystem::exists(F_SPECS)) {
        ofstream out(F_SPECS);
        out << "Программная инженерия\nИнформационные технологии\n"
            << "Искусственный интеллект\nКибербезопасность\nВеб-разработка\n";
    }
}

// Чтение настроек
void readConfig(AppConfig& cfg) {
    ifstream in(F_SETTINGS);
    string line;
    while (getline(in, line)) {
        size_t eqPos = line.find('=');
        if (eqPos == string::npos) continue;
        
        string param = line.substr(0, eqPos);
        string value = line.substr(eqPos + 1);

        if (param == "specialty") cfg.specialty = value;
        else if (param == "student") cfg.student = value;
        else if (param == "subjects") {
            try { cfg.subjects = stoi(value); } 
            catch (...) { cfg.subjects = 5; }
        }
    }
}

// Запись настроек
void writeConfig(const AppConfig& cfg) {
    ofstream out(F_SETTINGS);
    out << "specialty=" << cfg.specialty << "\n"
        << "student=" << cfg.student << "\n"
        << "subjects=" << cfg.subjects << "\n";
}

// Шаблонная загрузка строк из файла в контейнер
template <typename T>
void parseFileToContainer(const string& filename, T& container) {
    container.clear();
    ifstream in(filename);
    string temp;
    while (getline(in, temp)) {
        if (!temp.empty()) {
            container.insert(container.end(), temp);
        }
    }
}

// Универсальный выбор из контейнера
template <typename T>
bool selectFromContainer(const T& container, string& result) {
    if (container.empty()) {
        cout << "Список пуст!\n";
        return false;
    }
    
    int index = 1;
    for (const auto& item : container) {
        cout << index++ << ". " << item << "\n";
    }
    
    cout << "Введите номер (0 - отмена): ";
    int choice;
    cin >> choice;

    if (cin.fail() || choice < 0 || choice > (int)container.size()) {
        discardLine();
        cout << "Ошибка ввода!\n";
        return false;
    }
    if (choice == 0) return false;

    auto iter = container.begin();
    advance(iter, choice - 1);
    result = *iter;
    return true;
}

// Меню настроек
void openSettings(AppConfig& cfg, const list<string>& students, const set<string>& specs) {
    int action = 0;
    while (action != S_BACK) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Специальность (" << cfg.specialty << ")\n";
        cout << "2. Имя (" << cfg.student << ")\n";
        cout << "3. Кол-во предметов (" << cfg.subjects << ")\n";
        cout << "4. Назад\n";
        cout << "Ваш выбор: ";
        cin >> action;

        if (cin.fail()) {
            discardLine();
            cout << "Ошибка! Введите число.\n";
            continue;
        }

        switch (action) {
            case S_SPEC:
                discardLine();
                cout << "Доступные специальности:\n";
                if (selectFromContainer(specs, cfg.specialty)) writeConfig(cfg);
                break;
            case S_NAME:
                discardLine();
                cout << "Доступные студенты:\n";
                if (selectFromContainer(students, cfg.student)) writeConfig(cfg);
                break;
            case S_SUBJ: {
                cout << "Введите количество: ";
                int newSubj;
                cin >> newSubj;
                if (cin.fail() || newSubj <= 0) {
                    discardLine();
                    cout << "Некорректное значение!\n";
                } else {
                    cfg.subjects = newSubj;
                    writeConfig(cfg);
                }
                break;
            }
            case S_BACK:
                break;
            default:
                cout << "Такого пункта нет.\n";
        }
    }
}

// Заглушки
void playGame() {
    cout << "\n=== Игра началась ===\nВы в университете!\n";
    waitForEnter();
}

void loadGame() {
    cout << "\nЗагрузка пока не реализована.\n";
    waitForEnter();
}

void showAbout() {
    cout << "\n=== О программе ===\nВерсия: " << APP_VERSION << "\nСтудент: Бычик К.В.\n";
    waitForEnter();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    AppConfig config;
    
    checkAndCreateFiles(config);
    readConfig(config);

    list<string> studentList;
    set<string> specialtySet;
    
    parseFileToContainer(F_STUDENTS, studentList);
    parseFileToContainer(F_SPECS, specialtySet);

    int menuChoice = 0;
    while (menuChoice != M_EXIT) {
        cout << "\n*** Моя жизнь в университете ***\n";
        cout << "1. Начать игру\n2. Загрузить игру\n3. Настройки\n4. О программе\n5. Выход\n";
        cout << "Выбор: ";
        cin >> menuChoice;

        if (cin.fail()) {
            discardLine();
            cout << "Некорректный ввод!\n";
            continue;
        }

        if (menuChoice == M_START) playGame();
        else if (menuChoice == M_LOAD) loadGame();
        else if (menuChoice == M_SETTINGS) openSettings(config, studentList, specialtySet);
        else if (menuChoice == M_ABOUT) showAbout();
        else if (menuChoice == M_EXIT) cout << "До свидания!\n";
        else cout << "Нет такого пункта.\n";
    }

    return 0;
}
