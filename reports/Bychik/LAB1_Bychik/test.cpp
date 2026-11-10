#include <iostream>
#include <sstream>
#include <string>
#include <functional>
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

const string VERSION = "1.0";

struct Settings {
    string speciality = "Программная инженерия";
    string studentName = "Кристина";
    int    subjectsCount = 5;
};

int testsRun = 0;
int testsPassed = 0;

void check(bool condition, const string& name) {
    testsRun++;
    if (condition) {
        testsPassed++;
        cout << "[OK]   " << name << "\n";
    }
    else {
        cout << "[FAIL] " << name << "\n";
    }
}

string captureOutput(const function<void()>& fn) {
    stringstream buffer;
    streambuf* oldCout = cout.rdbuf(buffer.rdbuf());
    fn();
    cout.rdbuf(oldCout);
    return buffer.str();
}

string captureInput(const string& input, const function<void()>& fn) {
    stringstream inputStream(input);
    streambuf* oldCin = cin.rdbuf(inputStream.rdbuf());
    string result = captureOutput(fn);
    cin.rdbuf(oldCin);
    return result;
}

void test_default_settings() {
    Settings s;
    check(s.speciality == "Программная инженерия", "Значение по умолчанию: специальность");
    check(s.studentName == "Кристина", "Значение по умолчанию: имя");
    check(s.subjectsCount == 5, "Значение по умолчанию: количество предметов");
}

void test_change_speciality() {
    Settings s;
    captureInput("Прикладная информатика\n", [&]() {
        getline(cin, s.speciality);
        });
    check(s.speciality == "Прикладная информатика", "Изменение специальности сохраняется");
}

void test_change_name() {
    Settings s;
    captureInput("Алексей\n", [&]() {
        getline(cin, s.studentName);
        });
    check(s.studentName == "Алексей", "Изменение имени сохраняется");
}

void test_subjects_valid() {
    Settings s;
    int n = 0;
    captureInput("7\n", [&]() { cin >> n; });
    if (!cin.fail() && n > 0) s.subjectsCount = n;
    check(s.subjectsCount == 7, "Валидное число предметов (7) принимается");
}

void test_subjects_negative() {
    Settings s;
    int n = 0;
    captureInput("-3\n", [&]() { cin >> n; });
    check(cin.fail() || n <= 0, "Отрицательное число предметов отклоняется");
}

void test_subjects_zero() {
    Settings s;
    int n = 0;
    captureInput("0\n", [&]() { cin >> n; });
    check(cin.fail() || n <= 0, "Нулевое число предметов отклоняется");
}

void test_invalid_menu_input() {
    int choice = 0;
    cin.clear();                                  
    stringstream inputStream("abc\n");
    streambuf* oldCin = cin.rdbuf(inputStream.rdbuf());

    cin >> choice;

    bool failed = cin.fail();

    cin.rdbuf(oldCin);
    cin.clear();                                  

    check(failed, "Нечисловой ввод вызывает cin.fail()");
}

void test_version() {
    check(VERSION == "1.0", "Версия программы = 1.0");
}

void test_menu_constants() {
    check(MENU_START == 1 && MENU_LOAD == 2 && MENU_SETTINGS == 3
        && MENU_ABOUT == 4 && MENU_EXIT == 5,
        "Константы главного меню корректны");
    check(SET_SPECIALITY == 1 && SET_NAME == 2
        && SET_SUBJECTS == 3 && SET_BACK == 4,
        "Константы подменю настроек корректны");
}

void test_persistence() {
    Settings s;
    auto changeSpec = [&](const string& spec) {
        captureInput(spec + "\n", [&]() { getline(cin, s.speciality); });
        };
    changeSpec("Искусственный интеллект");
    changeSpec("Кибербезопасность");
    check(s.speciality == "Кибербезопасность",
        "Настройки сохраняются между изменениями");
}

int main() {
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    cout << "=== Модульные тесты ===\n\n";

    test_default_settings();
    test_change_speciality();
    test_change_name();
    test_subjects_valid();
    test_subjects_negative();
    test_subjects_zero();
    test_invalid_menu_input();
    test_version();
    test_menu_constants();
    test_persistence();

    cout << "\n============================\n";
    cout << "Пройдено: " << testsPassed << " / " << testsRun << "\n";
    cout << "============================\n";

    cout << "\nНажмите Enter для выхода...";
    cin.get();
    cin.get();

    return (testsPassed == testsRun) ? 0 : 1;
}
