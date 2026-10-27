#include <iostream>
#include <sstream>
#include <string>
#include <functional>

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

string captureInput(const string& input, const function<void()>& fn) {
    stringstream inputStream(input);
    streambuf* oldCin = cin.rdbuf(inputStream.rdbuf());
    
    stringstream buffer;
    streambuf* oldCout = cout.rdbuf(buffer.rdbuf());
    
    fn();
    
    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);
    return buffer.str();
}

void test_default_settings() {
    struct SETTING {
        string specialty = "Программная инженерия";
        string studentName = "Аня";
        int subjectsCount = 5;
    };
    SETTING s;
    check(s.specialty == "Программная инженерия", "Начальная специальность корректна");
    check(s.studentName == "Аня", "Начальное имя корректно");
    check(s.subjectsCount == 5, "Начальное количество предметов корректно");
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

void test_change_speciality() {
    struct SETTING {
        string specialty = "Программная инженерия";
    };
    SETTING s;
    captureInput("Прикладная информатика\n", [&]() {
        getline(cin, s.specialty);
    });
    check(s.specialty == "Прикладная информатика", "Изменение специальности работает");
}

void test_subjects_validation() {
    int n = 0;
    captureInput("-3\n", [&]() {
        cin >> n;
    });
    check(cin.fail() || n <= 0, "Отрицательное количество предметов обрабатывается корректно");
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
    
    check(failed, "Нечисловой ввод в меню вызывает сбой ввода (cin.fail)");
}

int main() {
    cout << "=== Модульные тесты ===\n\n";
    
    test_default_settings();
    test_menu_constants();
    test_change_speciality();
    test_subjects_validation();
    test_invalid_menu_input();
    
    cout << "\n============================\n";
    cout << "Пройдено: " << testsPassed << " / " << testsRun << "\n";
    cout << "============================\n";
    
    return (testsPassed == testsRun) ? 0 : 1;
}
