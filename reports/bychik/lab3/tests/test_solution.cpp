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

// Копия класса Subject для тестов
class Subject {
public:
    string title;
    int progress;
    Subject(string t = "") : title(t), progress(0) {}
    void addProgress(int amount) { progress = min(100, progress + amount); }
};

// Копия класса Action
class Action {
public:
    enum ActionType { VISIT, SKIP, HOME_STUDY, REST };
    ActionType type;
    int k, e, m;
    Action(ActionType t, int k, int e, int m) : type(t), k(k), e(e), m(m) {}
};

// Копия класса Student (упрощенная)
class Student {
public:
    int knowledge = 30, energy = 80, motivation = 60;
    vector<Subject> subjects;

    void executeAction(const Action& act, Subject* target = nullptr) {
        knowledge = max(0, min(100, knowledge + act.k));
        energy = max(0, min(100, energy + act.e));
        motivation = max(0, min(100, motivation + act.m));
        if (target != nullptr) {
            if (act.type == Action::VISIT) target->addProgress(15);
            else if (act.type == Action::HOME_STUDY) target->addProgress(10);
        }
    }
};

void test_subject_progress() {
    Subject s("Math");
    check(s.progress == 0, "Начальный прогресс предмета равен 0");
    s.addProgress(50);
    check(s.progress == 50, "Прогресс увеличился до 50");
    s.addProgress(70);
    check(s.progress == 100, "Прогресс не может превысить 100");
}

void test_student_actions() {
    Student st;
    Subject math("Math");
    
    Action visit(Action::VISIT, 10, -15, 5);
    st.executeAction(visit, &math);
    
    check(st.knowledge == 40, "Посещение: знания +10");
    check(st.energy == 65, "Посещение: энергия -15");
    check(math.progress == 15, "Посещение: прогресс предмета +15");

    Action rest(Action::REST, 0, 20, 10);
    st.executeAction(rest);
    
    check(st.energy == 85, "Отдых: энергия +20");
}

void test_limits() {
    Student st;
    Action mega_visit(Action::VISIT, 500, -500, 0);
    st.executeAction(mega_visit);
    
    check(st.knowledge == 100, "Знания не могут быть больше 100");
    check(st.energy == 0, "Энергия не может быть меньше 0");
}

int main() {
    cout << "=== Тесты логики ЛР3 ===\n\n";
    test_subject_progress();
    test_student_actions();
    test_limits();

    cout << "\n============================\n";
    cout << "Пройдено: " << passed_tests << " из " << total_tests << "\n";
    cout << "============================\n";

    return (passed_tests == total_tests) ? 0 : 1;
}
