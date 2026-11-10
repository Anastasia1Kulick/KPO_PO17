#define NOMINMAX

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <set>
#include <windows.h>

using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_SPECIALITY = 1;
const int SET_NAME = 2;
const int SET_SUBJECTS = 3;
const int SET_BACK = 4;

const string VERSION = "2.0";

const string SETTINGS_FILE = "settings.txt";
const string STUDENTS_FILE = "students.txt";
const string SPECIALITY_FILE = "specialities.txt";

struct Settings {
    string speciality = "Программная инженерия";
    string studentName = "Кристина";
    int subjectsCount = 5;
};

void clearInput() {
    cin.clear();
    cin.ignore(10000, '\n');
}

void waitEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню...";
    cin.get();
}

void ensureFile(const string& name, const string& content) {
    ifstream f(name);
    if (!f.good()) {
        ofstream(name) << content;
    }
}

void initFiles(const Settings& d) {
    ensureFile(SETTINGS_FILE,
        "speciality=" + d.speciality + "\n"
        "studentName=" + d.studentName + "\n"
        "subjectsCount=" + to_string(d.subjectsCount) + "\n");

    ensureFile(STUDENTS_FILE,
        "Кристина\nИван\nАнна\nПётр\nОльга\n");

    ensureFile(SPECIALITY_FILE,
        "Программная инженерия\n"
        "Информационные технологии\n"
        "Искусственный интеллект\n"
        "Кибербезопасность\n"
        "Веб-разработка\n");
}

void loadSettings(Settings& s) {
    ifstream in(SETTINGS_FILE);
    string line;
    while (getline(in, line)) {
        size_t p = line.find('=');
        if (p == string::npos) continue;
        string key = line.substr(0, p);
        string val = line.substr(p + 1);

        if (key == "speciality")        s.speciality = val;
        else if (key == "studentName")  s.studentName = val;
        else if (key == "subjectsCount") {
            try { s.subjectsCount = stoi(val); }
            catch (...) { s.subjectsCount = 5; }
        }
    }
}

void saveSettings(const Settings& s) {
    ofstream out(SETTINGS_FILE);
    out << "speciality=" << s.speciality << "\n"
        << "studentName=" << s.studentName << "\n"
        << "subjectsCount=" << s.subjectsCount << "\n";
}

template <typename Container>
void loadLines(const string& file, Container& c) {
    c.clear();
    ifstream in(file);
    string line;
    while (getline(in, line)) {
        if (!line.empty()) c.insert(c.end(), line);
    }
}

template <typename Container>
bool chooseFrom(const Container& c, string& out) {
    if (c.empty()) { cout << "Список пуст.\n"; return false; }

    int i = 1;
    for (const auto& v : c) cout << "  " << i++ << ". " << v << "\n";
    cout << "Выберите номер (0 - отмена): ";

    int n;
    cin >> n;
    if (cin.fail() || n < 0 || n >(int)c.size()) {
        clearInput();
        cout << "Некорректный выбор!\n";
        return false;
    }
    if (n == 0) return false;

    auto it = c.begin();
    advance(it, n - 1);
    out = *it;
    return true;
}

void startGame() {
    cout << "\n=== Начало игры \"Моя жизнь в университете\" ===\n";
    cout << "Вы поступаете в университет. Удачи!\n";
    waitEnter();
}

void loadGame() {
    cout << "\nФункция загрузки пока недоступна.\n";
    cout << "Она появится в ближайших обновлениях.\n";
    waitEnter();
}

void aboutProgram() {
    cout << "\n=== О программе ===\n";
    cout << "Версия: " << VERSION << "\n";
    cout << "Студент: Бычик К.В., группа ПО-17\n";
    waitEnter();
}

void settingsMenu(Settings& s,
    const list<string>& students,
    const set<string>& specialities) {
    int choice = 0;
    while (choice != SET_BACK) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Специальность (текущая: " << s.speciality << ")\n";
        cout << "2. Имя студента (текущее: " << s.studentName << ")\n";
        cout << "3. Количество предметов (текущее: " << s.subjectsCount << ")\n";
        cout << "4. Назад\n";
        cout << "Выбор: ";

        cin >> choice;
        if (cin.fail()) {
            clearInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        switch (choice) {
        case SET_SPECIALITY:
            clearInput();
            cout << "\nДоступные специальности:\n";
            if (chooseFrom(specialities, s.speciality)) {
                saveSettings(s);   // сохраняем сразу
            }
            break;

        case SET_NAME:
            clearInput();
            cout << "\nДоступные студенты:\n";
            if (chooseFrom(students, s.studentName)) {
                saveSettings(s);
            }
            break;

        case SET_SUBJECTS: {
            cout << "Введите количество предметов: ";
            int n;
            cin >> n;
            if (cin.fail() || n <= 0) {
                clearInput();
                cout << "Некорректное значение!\n";
            }
            else {
                s.subjectsCount = n;
                saveSettings(s);
            }
            break;
        }

        case SET_BACK:
            break;

        default:
            cout << "Такого пункта нет.\n";
        }
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    Settings settings;

    // НОВОЕ: файлы и контейнеры
    initFiles(settings);
    loadSettings(settings);

    list<string> students;
    set<string> specialities;
    loadLines(STUDENTS_FILE, students);
    loadLines(SPECIALITY_FILE, specialities);

    int choice = 0;
    while (choice != MENU_EXIT) {
        cout << "\n*** Моя жизнь в университете ***\n";
        cout << "1. Начать игру\n";
        cout << "2. Загрузить игру\n";
        cout << "3. Настройки\n";
        cout << "4. О программе\n";
        cout << "5. Выход\n";
        cout << "Выбор: ";

        cin >> choice;
        if (cin.fail()) {
            clearInput();
            cout << "Некорректный ввод!\n";
            continue;
        }

        switch (choice) {
        case MENU_START:    startGame(); break;
        case MENU_LOAD:     loadGame();  break;
        case MENU_SETTINGS: settingsMenu(settings, students, specialities); break;
        case MENU_ABOUT:    aboutProgram(); break;
        case MENU_EXIT:
            cout << "Выход из программы...\n";
            break;
        default:
            cout << "Такого пункта нет.\n";
        }
    }

    return 0;
}
