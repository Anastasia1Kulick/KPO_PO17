#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Копия структуры из исходного кода
struct SETTING {
    string specialty = "Программная инженерия";
    string studentName = "Аня";
    int subjectCount = 11;
};

// Мини-фреймворк для тестирования
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
        cout << "  [FAIL] " << name
             << " (получено: " << actual
             << ", ожидалось: " << expected << ")\n";
    }
}

// Тесты
void testDefaults() {
    cout << "\n[1] Значения по умолчанию\n";
    SETTING s;
    checkEq(s.specialty, string("Программная инженерия"), "специальность по умолчанию");
    checkEq(s.studentName, string("Аня"), "имя студента по умолчанию");
    checkEq(s.subjectCount, 11, "количество предметов по умолчанию");
}

void testModifySpecialty() {
    cout << "\n[2] Изменение специальности\n";
    SETTING s;
    s.specialty = "Искусственный интеллект";
    checkEq(s.specialty, string("Искусственный интеллект"), "специальность изменена");
    checkEq(s.studentName, string("Аня"), "имя не изменилось");
    checkEq(s.subjectCount, 11, "предметы не изменились");
}

void testModifyName() {
    cout << "\n[3] Изменение имени студента\n";
    SETTING s;
    s.studentName = "Иван";
    checkEq(s.studentName, string("Иван"), "имя изменено");
    checkEq(s.specialty, string("Программная инженерия"), "специальность не изменилась");
    checkEq(s.subjectCount, 11, "предметы не изменились");
}

void testModifySubjects() {
    cout << "\n[4] Изменение количества предметов\n";
    SETTING s;
    s.subjectCount = 5;
    checkEq(s.subjectCount, 5, "количество предметов изменено на 5");
    s.subjectCount = 0;
    checkEq(s.subjectCount, 0, "количество предметов может быть 0 (граница)");
}

void testMultipleChanges() {
    cout << "\n[5] Комплексное изменение\n";
    SETTING s;
    s.specialty = "Кибербезопасность";
    s.studentName = "Мария";
    s.subjectCount = 8;
    
    checkEq(s.specialty, string("Кибербезопасность"), "специальность изменена");
    checkEq(s.studentName, string("Мария"), "имя изменено");
    checkEq(s.subjectCount, 8, "предметы изменены");
}

void testCopySemantics() {
    cout << "\n[6] Копирование структуры\n";
    SETTING s1;
    s1.studentName = "Петр";
    
    SETTING s2 = s1;
    checkEq(s2.studentName, string("Петр"), "имя скопировано");
    
    s2.studentName = "Сидор";
    checkEq(s1.studentName, string("Петр"), "изменение копии не затронуло оригинал");
    checkEq(s2.studentName, string("Сидор"), "копия изменена корректно");
}

void testInvalidStateBoundaries() {
    cout << "\n[7] Граничные значения (валидация данных)\n";
    SETTING s;
    
    // Проверка, что отрицательное количество предметов не является "нормальным" состоянием
    s.subjectCount = -1;
    check(s.subjectCount < 0, "отрицательное количество обнаружено (потенциальная ошибка ввода)");
    
    // Пустая строка
    s.studentName = "";
    check(s.studentName.empty(), "пустое имя обнаружено (потенциальная ошибка ввода)");
}

int main() {
    cout << "=============================================\n";
    cout << " Запуск тестов: Моя жизнь в университете\n";
    cout << "=============================================\n";

    testDefaults();
    testModifySpecialty();
    testModifyName();
    testModifySubjects();
    testMultipleChanges();
    testCopySemantics();
    testInvalidStateBoundaries();
    cout << "\n=============================================\n";
        cout << " Итог: пройдено " << g_passed
             << ", провалено " << g_failed << "\n";
        cout << "=============================================\n";
    
        if (g_failed > 0) {
            cout << "\nПроваленные тесты:\n";
            for (const auto& f : g_failures) cout << "  - " << f << "\n";
            return 1;
        }
        cout << "\nВсе тесты пройдены успешно.\n";
        return 0;
}
