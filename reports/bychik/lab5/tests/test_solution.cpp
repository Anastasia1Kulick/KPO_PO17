#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int total_tests = 0;
int passed_tests = 0;

void check(bool condition, const string& name) {
    total_tests++;
    if (condition) {
        passed_tests++;
        cout << "[OK]   " << name << "\n";
    } else {
        cout << "[FAIL] " << name << "\n";
    }
}

// Копии классов для тестов
class Subject {
public:
    string title;
    int progress;
    Subject(string t = "", int p = 0) : title(t), progress(p) {}
    void addProgress(int amount) { progress = min(100, progress + amount); }
};

class Student {
public:
    string name;
    string speciality;
    int knowledge, energy, motivation;
    int money;
    Subject* subjects;
    int subjectsCount;
    int capacity;

    Student(string n = "", string s = "") : name(n), speciality(s), knowledge(30), energy(80), motivation(60), money(500) {
        capacity = 10;
        subjectsCount = 0;
        subjects = new Subject[capacity];
    }

    ~Student() { delete[] subjects; }

    void reset(const string& n, const string& s) {
        name = n;
        speciality = s;
        knowledge = 30;
        energy = 80;
        motivation = 60;
        money = 500;
        subjectsCount = 0;
    }

    void addSubject(const Subject& subj) {
        if (subjectsCount < capacity) {
            subjects[subjectsCount++] = subj;
        }
    }

    void addMoney(int amount) { money += amount; }
};

// Тест 1: Проверка логики начисления стипендии
void test_scholarship_logic() {
    cout << "--- Тест 1: Логика аттестации ---\n";
    int base_salary = 1000;

    // Хорошие знания (>= 50) - полная стипендия
    int knowledge_good = 65;
    int salary_good = base_salary;
    if (knowledge_good < 30) salary_good = 0;
    else if (knowledge_good < 50) salary_good /= 2;
    check(salary_good == 1000, "Знания >= 50: полная стипендия (1000)");

    // Средние знания (30-49) - половина стипендии
    int knowledge_mid = 40;
    int salary_mid = base_salary;
    if (knowledge_mid < 30) salary_mid = 0;
    else if (knowledge_mid < 50) salary_mid /= 2;
    check(salary_mid == 500, "Знания 30-49: урезанная стипендия (500)");

    // Плохие знания (< 30) - нет стипендии
    int knowledge_bad = 15;
    int salary_bad = base_salary;
    if (knowledge_bad < 30) salary_bad = 0;
    else if (knowledge_bad < 50) salary_bad /= 2;
    check(salary_bad == 0, "Знания < 30: стипендия не начислена (0)");
}

// Тест 2: Изменение баланса
void test_money_balance() {
    cout << "\n--- Тест 2: Работа с балансом ---\n";
    Student st;
    check(st.money == 500, "Стартовый баланс равен 500 руб.");
    
    st.addMoney(1000);
    check(st.money == 1500, "Баланс корректно увеличился на 1000");
    
    st.addMoney(-500);
    check(st.money == 1000, "Баланс корректно уменьшился на 500");
}

// Тест 3: Проверка сброса (reset) при загрузке
void test_reset_load_logic() {
    cout << "\n--- Тест 3: Логика сброса при загрузке ---\n";
    Student st("Иван", "ПО");
    st.addMoney(2000);
    st.addSubject(Subject("Math", 50));
    
    check(st.money == 2500, "Баланс до сброса (500 + 2000)");
    
    // Симулируем загрузку
    st.reset("Анна", "ИИТ");
    
    check(st.name == "Анна", "Имя изменено при загрузке");
    check(st.speciality == "ИИТ", "Специальность изменена при загрузке");
    check(st.money == 500, "Баланс сброшен к дефолтному при загрузке");
    check(st.subjectsCount == 0, "Список предметов очищен для загрузки новых");
}

int main() {
    cout << "=== Тестирование ЛР5 (Финансы и Аттестация) ===\n\n";
    
    test_scholarship_logic();
    test_money_balance();
    test_reset_load_logic();

    cout << "\n=====================================\n";
    cout << "Итог: Пройдено " << passed_tests << " из " << total_tests << " тестов\n";
    cout << "=====================================\n";

    return (passed_tests == total_tests) ? 0 : 1;
}
