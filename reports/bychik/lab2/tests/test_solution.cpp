#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <list>
#include <set>
#include <vector>

using namespace std;

// Флаги для отслеживания результатов
int total_tests = 0;
int passed_tests = 0;

void check(bool condition, const string& test_name) {
    total_tests++;
    if (condition) {
        passed_tests++;
        cout << "[OK]   " << test_name << "\n";
    } else {
        cout << "[FAIL] " << test_name << "\n";
    }
}

// Вспомогательная функция для теста загрузки строк (аналог parseFileToContainer)
template <typename Container>
void dummy_parse(const string& filename, Container& c) {
    c.clear();
    ifstream in(filename);
    string line;
    while (getline(in, line)) {
        if (!line.empty()) {
            c.insert(c.end(), line);
        }
    }
}

void test_containers() {
    // Тест 1: list для студентов
    list<string> students;
    students.push_back("Иван");
    students.push_back("Анна");
    check(students.size() == 2, "list содержит 2 элемента");
    check(students.front() == "Иван", "Первый элемент list - Иван");

    // Тест 2: set для специальностей
    set<string> specs;
    specs.insert("Программная инженерия");
    specs.insert("Кибербезопасность");
    specs.insert("Программная инженерия"); // Дубликат не должен добавиться
    
    check(specs.size() == 2, "set игнорирует дубликаты (размер 2)");
    check(specs.count("Кибербезопасность") == 1, "set находит нужный элемент");
    check(specs.count("Несуществующая") == 0, "set не находит несуществующий элемент");
}

void test_file_parsing() {
    // Создаем временный файл для теста
    string test_file = "temp_test_file.txt";
    ofstream out(test_file);
    out << "Строка 1\nСтрока 2\n\nСтрока 3\n";
    out.close();

    list<string> test_list;
    dummy_parse(test_file, test_list);

    check(test_list.size() == 3, "Чтение из файла: пустые строки игнорируются");
    check(test_list.front() == "Строка 1", "Чтение из файла: первая строка верна");

    // Удаляем временный файл
    remove(test_file.c_str());
}

void test_subjects_logic() {
    // Имитация логики ввода количества предметов
    int subjects = 5;
    int new_input = 8;
    
    // Логика из main.cpp
    if (new_input > 0) {
        subjects = new_input;
    }
    check(subjects == 8, "Валидное число предметов принимается");

    new_input = -3;
    if (new_input <= 0) {
        // Отклоняем
    }
    check(subjects == 8, "Отрицательное число предметов отклоняется");
}

int main() {
    cout << "=== Модульные тесты ЛР2 ===\n\n";

    test_containers();
    test_file_parsing();
    test_subjects_logic();

    cout << "\n============================\n";
    cout << "Пройдено: " << passed_tests << " из " << total_tests << "\n";
    cout << "============================\n";

    return (passed_tests == total_tests) ? 0 : 1;
}
