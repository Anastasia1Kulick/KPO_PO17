#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <functional>

using namespace std;

const int Start = 1;
const int Load = 2;
const int Setting = 3;
const int Info = 4;
const int Exit = 5;

const int Specialty = 1;
const int Name = 2;
const int Subjects = 3;
const int Back = 4;

struct SETTING {
    string specialty = "Программная инженерия";
    string studentName = "Аня";
    int subjectCount = 11;
};

bool applySettingCommand(SETTING& set, int choice,
                         const string& textValue, int intValue) {
    switch (choice) {
    case Specialty:
        set.specialty = textValue;
        return false;
    case Name:
        set.studentName = textValue;
        return false;
    case Subjects:
        if (intValue < 0) return false;
        set.subjectCount = intValue;
        return false;
    case Back:
        return true;
    default:
        return false;
    }
}

int passed = 0;
int failed = 0;

void check(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << endl;
        passed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        failed++;
    }
}

void testDefaultSettings() {
    cout << "\n--- Тест: значения по умолчанию ---" << endl;
    SETTING s;
    check(s.specialty == "Программная инженерия", "Специальность по умолчанию");
    check(s.studentName == "Аня", "Имя студента по умолчанию");
    check(s.subjectCount == 11, "Количество предметов по умолчанию");
}

void testConstants() {
    cout << "\n--- Тест: константы меню ---" << endl;
    check(Start == 1, "Start == 1");
    check(Load == 2, "Load == 2");
    check(Setting == 3, "Setting == 3");
    check(Info == 4, "Info == 4");
    check(Exit == 5, "Exit == 5");

    check(Specialty == 1, "Specialty == 1");
    check(Name == 2, "Name == 2");
    check(Subjects == 3, "Subjects == 3");
    check(Back == 4, "Back == 4");
}

void testChangeSpecialty() {
    cout << "\n--- Тест: смена специальности ---" << endl;
    SETTING s;
    bool back = applySettingCommand(s, Specialty, "Искусственный интеллект", 0);
    check(!back, "Возврат false при смене специальности");
    check(s.specialty == "Искусственный интеллект", "Специальность изменилась");
}

void testChangeName() {
    cout << "\n--- Тест: смена имени ---" << endl;
    SETTING s;
    bool back = applySettingCommand(s, Name, "Иван", 0);
    check(!back, "Возврат false при смене имени");
    check(s.studentName == "Иван", "Имя изменилось");
}

void testChangeSubjects() {
    cout << "\n--- Тест: смена количества предметов ---" << endl;
    SETTING s;
    applySettingCommand(s, Subjects, "", 7);
    check(s.subjectCount == 7, "Количество предметов изменилось на 7");

    applySettingCommand(s, Subjects, "", 0);
    check(s.subjectCount == 0, "Можно установить 0 предметов");
}

void testNegativeSubjects() {
    cout << "\n--- Тест: отрицательное число предметов ---" << endl;
    SETTING s;
    int old = s.subjectCount;
    applySettingCommand(s, Subjects, "", -5);
    check(s.subjectCount == old, "Отрицательное значение не применяется");
}

void testBackCommand() {
    cout << "\n--- Тест: команда 'Назад' ---" << endl;
    SETTING s;
    bool back = applySettingCommand(s, Back, "", 0);
    check(back, "Возврат true при выборе 'Назад'");
    check(s.specialty == "Программная инженерия", "Настройки не изменились");
    check(s.studentName == "Аня", "Имя не изменилось");
    check(s.subjectCount == 11, "Предметы не изменились");
}

void testInvalidCommand() {
    cout << "\n--- Тест: неизвестная команда ---" << endl;
    SETTING s;
    bool back = applySettingCommand(s, 99, "", 0);
    check(!back, "Возврат false для неизвестной команды");
    check(s.specialty == "Программная инженерия", "Настройки не тронуты");
}

void testEmptySpecialty() {
    cout << "\n--- Тест: пустая строка специальности ---" << endl;
    SETTING s;
    applySettingCommand(s, Specialty, "", 0);
    check(s.specialty == "", "Можно установить пустую специальность");
}

int main() {
    cout << "========================================" << endl;
    cout << "   ЗАПУСК ТЕСТОВ test.cpp" << endl;
    cout << "========================================" << endl;

    testConstants();
    testDefaultSettings();
    testChangeSpecialty();
    testChangeName();
    testChangeSubjects();
    testNegativeSubjects();
    testBackCommand();
    testInvalidCommand();
    testEmptySpecialty();

    cout << "\n========================================" << endl;
    cout << "  РЕЗУЛЬТАТ: passed = " << passed
         << ", failed = " << failed << endl;
    cout << "========================================" << endl;

    return (failed == 0) ? 0 : 1;
}
