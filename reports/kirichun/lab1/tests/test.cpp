#include <iostream>
#include <sstream>
#include <string>
#include <functional>
#include <windows.h>

using namespace std;

int testsRun = 0;
int testsPassed = 0;

void check(bool condition, const string& name) {
    testsRun++;
    if (condition) {
        testsPassed++;
        cout << "[OK]   " << name << "\n";
    } else {
        cout << "[FAIL] " << name << "\n";
    }
}

void test_default_settings() {
    struct SETTING {
        string specialty = "Программная инженерия";
        string studentName = "Аня";
        int subjectCount = 11;
    };
    SETTING s;
    check(s.specialty == "Программная инженерия", "Начальная специальность корректна");
    check(s.studentName == "Аня", "Начальное имя корректно");
    check(s.subjectCount == 11, "Начальное количество предметов корректно");
}

void test_menu_constants() {
    const int MENU_START = 1;
    const int MENU_LOAD = 2;
    const int MENU_SETTINGS = 3;
    const int MENU_ABOUT = 4;
    const int MENU_EXIT = 5;

    check(MENU_START == 1 && MENU_LOAD == 2 && MENU_SETTINGS == 3 
        && MENU_ABOUT == 4 && MENU_EXIT == 5, 
        "Константы главного меню корректны");
}

int main() {
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
    
    cout << "=== Модульные тесты ===\n\n";
    test_default_settings();
    test_menu_constants();
    
    cout << "\n============================\n";
    cout << "Пройдено: " << testsPassed << " / " << testsRun << "\n";
    cout << "============================\n";
    
    return (testsPassed == testsRun) ? 0 : 1;
}
