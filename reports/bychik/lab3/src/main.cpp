#define NOMINMAX                 
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <windows.h>

using namespace std;

const string APP_VER = "3.0";
const string CONFIG_FILE = "settings.txt";
const string STUDENTS_FILE = "students.txt";
const string SPECIALITIES_FILE = "specialities.txt";

struct Settings {
    string speciality = "Программная инженерия";
    string studentName = "Кристина";
    int subjectsCount = 5;
};

// Вспомогательные функции
void clearStream() { cin.clear(); cin.ignore(10000, '\n'); }
void pressEnter() { cout << "\nНажмите Enter..."; cin.get(); }

void ensureFileExists(const string& filename, const string& defaultContent) {
    ifstream f(filename);
    if (!f.good()) {
        ofstream(filename) << defaultContent;
    }
}

void initFiles(const Settings& s) {
    ensureFileExists(CONFIG_FILE, "speciality=" + s.speciality + "\nstudentName=" + s.studentName + "\nsubjectsCount=" + to_string(s.subjectsCount) + "\n");
    ensureFileExists(STUDENTS_FILE, "Кристина\nИван\nАнна\nПётр\nОльга\n");
    ensureFileExists(SPECIALITIES_FILE, "Программная инженерия\nИнформационные технологии\nИскусственный интеллект\nКибербезопасность\nВеб-разработка\n");
}

void readConfig(Settings& s) {
    ifstream in(CONFIG_FILE);
    string line;
    while (getline(in, line)) {
        size_t pos = line.find('=');
        if (pos == string::npos) continue;
        string key = line.substr(0, pos);
        string val = line.substr(pos + 1);
        
        if (key == "speciality") s.speciality = val;
        else if (key == "studentName") s.studentName = val;
        else if (key == "subjectsCount") {
            try { s.subjectsCount = stoi(val); } catch (...) {}
        }
    }
}

void writeConfig(const Settings& s) {
    ofstream out(CONFIG_FILE);
    out << "speciality=" << s.speciality << "\n"
        << "studentName=" << s.studentName << "\n"
        << "subjectsCount=" << s.subjectsCount << "\n";
}

template <typename Container>
void loadFromFile(const string& file, Container& c) {
    c.clear();
    ifstream in(file);
    string line;
    while (getline(in, line)) {
        if (!line.empty()) c.insert(c.end(), line);
    }
}

template <typename Container>
bool selectFromList(const Container& c, string& out) {
    if (c.empty()) { cout << "Список пуст!\n"; return false; }
    int i = 1;
    for (const auto& item : c) cout << i++ << ". " << item << "\n";
    cout << "Выберите номер (0 - отмена): ";
    int n; cin >> n;
    
    if (cin.fail() || n < 0 || n > (int)c.size()) {
        clearStream();
        cout << "Неверный ввод!\n";
        return false;
    }
    if (n == 0) return false;
    
    auto it = c.begin();
    advance(it, n - 1);
    out = *it;
    return true;
}

// ================= ИГРОВЫЕ КЛАССЫ =================

class Subject {
private:
    string title;
    int progress;
public:
    Subject(string t = "") : title(t), progress(0) {}
    string getTitle() const { return title; }
    int getProgress() const { return progress; }
    void addProgress(int amount) { progress = min(100, progress + amount); }
};

class Action {
public:
    enum ActionType { VISIT, SKIP, HOME_STUDY, REST };
    ActionType type;
    string label;
    int knowledgeChange, energyChange, motivationChange;

    Action(ActionType t, string l, int k, int e, int m) 
        : type(t), label(l), knowledgeChange(k), energyChange(e), motivationChange(m) {}
};

class Student {
private:
    string name;
    string speciality;
    int knowledge;
    int energy;
    int motivation;
    vector<Subject> subjects;

public:
    Student(string n, string s) : name(n), speciality(s), knowledge(30), energy(80), motivation(60) {}

    void addSubject(const Subject& subj) { subjects.push_back(subj); }
    vector<Subject>& getSubjects() { return subjects; }
    string getName() const { return name; }
    string getSpeciality() const { return speciality; }
    int getKnowledge() const { return knowledge; }

    void executeAction(const Action& act, Subject* target = nullptr) {
        knowledge = max(0, min(100, knowledge + act.knowledgeChange));
        energy = max(0, min(100, energy + act.energyChange));
        motivation = max(0, min(100, motivation + act.motivationChange));

        if (target != nullptr) {
            if (act.type == Action::VISIT) target->addProgress(15);
            else if (act.type == Action::HOME_STUDY) target->addProgress(10);
        }
    }

    void printStatus() const {
        cout << "\n--- " << name << " (" << speciality << ") ---\n";
        cout << "Знания: " << knowledge << " | Энергия: " << energy << " | Мотивация: " << motivation << "\n";
        for (size_t i = 0; i < subjects.size(); ++i) {
            cout << " " << i + 1 << ". " << subjects[i].getTitle() << " - " << subjects[i].getProgress() << "%\n";
        }
    }
};

class GameEngine {
private:
    Student student;
    int maxDays;
    int currentDay;

public:
    GameEngine(const Settings& cfg) : student(cfg.studentName, cfg.speciality), maxDays(10), currentDay(1) {
        const char* defaultSubjects[] = {
            "Математика", "Программирование", "Физика", "Английский", "История",
            "Базы данных", "Алгоритмы", "Философия", "Экономика", "Право"
        };
        int count = max(1, min(10, cfg.subjectsCount));
        for (int i = 0; i < count; ++i) {
            student.addSubject(Subject(defaultSubjects[i % 10]));
        }
    }

    void play() {
        Action actVisit(Action::VISIT, "Посетить", 10, -15, 5);
        Action actSkip(Action::SKIP, "Прогулять", -5, 10, -10);
        Action actStudy(Action::HOME_STUDY, "Учиться дома", 7, -10, 2);
        Action actRest(Action::REST, "Отдохнуть", 0, 20, 10);

        while (currentDay <= maxDays) {
            student.printStatus();
            cout << "\n--- День " << currentDay << " из " << maxDays << " ---\n";
            cout << "1. Посетить\n2. Прогулять\n3. Учиться дома\n4. Отдохнуть\nВыбор: ";
            
            int choice;
            cin >> choice;
            if (cin.fail()) { clearStream(); continue; }

            Subject* target = nullptr;
            auto& subs = student.getSubjects();

            if (choice >= 1 && choice <= 3) {
                cout << "Выберите предмет (0 - отмена):\n";
                for (size_t i = 0; i < subs.size(); ++i) {
                    cout << " " << i + 1 << ". " << subs[i].getTitle() << "\n";
                }
                int idx;
                cin >> idx;
                if (cin.fail() || idx < 0 || idx > (int)subs.size()) {
                    clearStream();
                    continue;
                }
                if (idx == 0) continue;
                target = &subs[idx - 1];
            }

            if (choice == 1) student.executeAction(actVisit, target);
            else if (choice == 2) student.executeAction(actSkip, target);
            else if (choice == 3) student.executeAction(actStudy, target);
            else if (choice == 4) student.executeAction(actRest);
            else continue;

            currentDay++;
        }
        runSession();
    }

private:
    void runSession() {
        cout << "\n==================================\n";
        cout << "          СЕССИЯ НАЧАЛАСЬ!\n";
        cout << "==================================\n";
        
        int totalScore = 0;
        auto& subs = student.getSubjects();

        for (auto& subj : subs) {
            int baseGrade = max(1, min(5, (subj.getProgress() + student.getKnowledge()) / 40));
            cout << "\n--- Экзамен: " << subj.getTitle() << " (Прогресс: " << subj.getProgress() << "%) ---\n";
            
            int correctAnswers = 0, ans;
            
            cout << "Вопрос 1. Вы изучили больше половины материала? (1 - Да, 0 - Нет): ";
            cin >> ans;
            if (ans == 1 && subj.getProgress() >= 50) correctAnswers++;
            
            cout << "Вопрос 2. Вы регулярно посещали занятия? (1/0): ";
            cin >> ans;
            if (ans == 1 && subj.getProgress() >= 60) correctAnswers++;
            
            cout << "Вопрос 3. Вы уверены в своих знаниях? (1/0): ";
            cin >> ans;
            if (ans == 1 && subj.getProgress() >= 70) correctAnswers++;

            int bonus = min(2, correctAnswers);
            int finalGrade = min(5, baseGrade + bonus);
            
            cout << "Итоговая оценка: " << finalGrade << " (База: " << baseGrade << " + Бонус: " << bonus << ")\n";
            totalScore += finalGrade;
        }

        double avg = subs.empty() ? 0.0 : (double)totalScore / subs.size();
        cout << "\nВаш средний балл: " << avg << "\n";
        
        if (avg >= 4.5) cout << "Отличник! Стипендия обеспечена.\n";
        else if (avg >= 3.5) cout << "Хороший результат.\n";
        else if (avg >= 2.5) cout << "Сессия сдана, но есть долги.\n";
        else cout << "Вас отчислили...\n";
    }
};

// ================= МЕНЮ =================

void startGame(const Settings& s) {
    GameEngine engine(s);
    engine.play();
    pressEnter();
}

void loadGame() {
    cout << "\nСохранений пока нет.\n";
    pressEnter();
}

void showAbout() {
    cout << "\n=== О программе ===\nВерсия: " << APP_VER << "\nСтудент: Бычик К.В., группа ПО-17\n";
    pressEnter();
}

void settingsMenu(Settings& s, const list<string>& students, const set<string>& specs) {
    int choice = 0;
    while (choice != 4) {
        cout << "\n--- Настройки ---\n";
        cout << "1. Специальность (" << s.speciality << ")\n";
        cout << "2. Имя (" << s.studentName << ")\n";
        cout << "3. Кол-во предметов (" << s.subjectsCount << ")\n";
        cout << "4. Назад\nВыбор: ";
        
        cin >> choice;
        if (cin.fail()) { clearStream(); continue; }

        if (choice == 1) {
            clearStream();
            if (selectFromList(specs, s.speciality)) writeConfig(s);
        } else if (choice == 2) {
            clearStream();
            if (selectFromList(students, s.studentName)) writeConfig(s);
        } else if (choice == 3) {
            int n;
            cin >> n;
            if (cin.fail() || n <= 0) {
                clearStream();
                cout << "Некорректное значение!\n";
            } else {
                s.subjectsCount = n;
                writeConfig(s);
            }
        }
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    Settings cfg;
    initFiles(cfg);
    readConfig(cfg);

    list<string> studentsList;
    set<string> specialitiesSet;
    loadFromFile(STUDENTS_FILE, studentsList);
    loadFromFile(SPECIALITIES_FILE, specialitiesSet);

    int menuChoice = 0;
    while (menuChoice != 5) {
        cout << "\n*** Моя жизнь в университете ***\n";
        cout << "1. Начать игру\n2. Загрузить игру\n3. Настройки\n4. О программе\n5. Выход\nВыбор: ";
        
        cin >> menuChoice;
        if (cin.fail()) { clearStream(); continue; }

        switch (menuChoice) {
            case 1: startGame(cfg); break;
            case 2: loadGame(); break;
            case 3: settingsMenu(cfg, studentsList, specialitiesSet); break;
            case 4: showAbout(); break;
            case 5: cout << "Выход...\n"; break;
            default: cout << "Нет такого пункта.\n";
        }
    }
    return 0;
}
