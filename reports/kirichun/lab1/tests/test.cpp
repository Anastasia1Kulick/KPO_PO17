#include <iostream>
#include <string>
#include <vector>

using namespace std;

const string DEFAULT_SPECIALTY = "Программная инженерия";
const string DEFAULT_NAME = "Аня";
const int DEFAULT_SUBJECT_COUNT = 11;

struct GameSettings {
    string specialty = DEFAULT_SPECIALTY;
    string studentName = DEFAULT_NAME;
    int subjectCount = DEFAULT_SUBJECT_COUNT;

    void reset() {
        specialty = DEFAULT_SPECIALTY;
        studentName = DEFAULT_NAME;
        subjectCount = DEFAULT_SUBJECT_COUNT;
    }

    void setSpecialty(const string& spec) {
        specialty = spec;
    }

    void setName(const string& name) {
        studentName = name;
    }

    bool setSubjectCount(int count) {
        if (count < 0) return false;
        subjectCount = count;
        return true;
    }
};

static int g_passed = 0;
static int g_failed = 0;
static vector<string> g_failures;

void check(bool cond, const string& name) {
    if (cond) {
        ++g_passed;
        cout << "  [ OK ] " << name << "\n";
    } else {
        ++g_failed;
        g_failures.push_back(name);
        cout << "  [FAIL] " << name << "\n";
    }
}

template <typename T>
void checkEq(const T& actual, const T& expected, const string& name) {
    if (actual == expected) {
        ++g_passed;
        cout << "  [ OK ] " << name << "\n";
    } else {
        ++g_failed;
        g_failures.push_back(name);
        cout << "  [FAIL] " << name << " (получено: " << actual << ", ожидалось: " << expected << ")\n";
    }
}

void testDefaults() {
    cout << "\n[1] Значения по умолчанию\n";
    GameSettings s;
    checkEq(s.specialty, DEFAULT_SPECIALTY, "специальность по умолчанию");
    checkEq(s.studentName, DEFAULT_NAME, "имя студента по умолчанию");
    checkEq(s.subjectCount, DEFAULT_SUBJECT_COUNT, "количество предметов по умолчанию");
}

void testSetSpecialty() {
    cout << "\n[2] Установка специальности\n";
    GameSettings s;
    s.setSpecialty("Искусственный интеллект");
    checkEq(s.specialty, string("Искусственный интеллект"), "специальность изменена");
    s.setSpecialty("");
    checkEq(s.specialty, string(""), "можно установить пустую строку");
}

void testSetName() {
    cout << "\n[3] Установка имени\n";
    GameSettings s;
    s.setName("Иван");
    checkEq(s.studentName, string("Иван"), "имя изменено");
    s.setName("Анна-Мария");
    checkEq(s.studentName, string("Анна-Мария"), "имя с дефисом");
}

void testSetSubjectCount() {
    cout << "\n[4] Установка количества предметов\n";
    GameSettings s;
    check(s.setSubjectCount(5), "5 — принято");
    checkEq(s.subjectCount, 5, "предметов = 5");
    
    check(s.setSubjectCount(0), "0 — принято");
    checkEq(s.subjectCount, 0, "предметов = 0");

    check(!s.setSubjectCount(-1), "-1 — отклонено");
    checkEq(s.subjectCount, 0, "предметов осталось 0 после ошибки");
}

void testReset() {
    cout << "\n[5] Сброс настроек\n";
    GameSettings s;
    s.specialty = "Другая";
    s.studentName = "Петр";
    s.subjectCount = 99;
    s.reset();
    
    checkEq(s.specialty, DEFAULT_SPECIALTY, "специальность сброшена");
    checkEq(s.studentName, DEFAULT_NAME, "имя сброшено");
    checkEq(s.subjectCount, DEFAULT_SUBJECT_COUNT, "предметы сброшены");
}

int main() {
    cout << " Запуск тестов: МОЯ ЖИЗНЬ В УНИВЕРСИТЕТЕ\n";
    
    testDefaults();
    testSetSpecialty();
    testSetName();
    testSetSubjectCount();
    testReset();

    cout << "\n Итог: пройдено " << g_passed << ", провалено " << g_failed << "\n";
    
    if (g_failed > 0) {
        cout << "\nПроваленные тесты:\n";
        for (const auto& f : g_failures) cout << "  - " << f << "\n";
        return 1;
    }
    cout << "\nВсе тесты пройдены успешно.\n";
    return 0;
}
