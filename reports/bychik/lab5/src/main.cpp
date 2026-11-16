#define NOMINMAX                 
#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <set>
#include <algorithm>
#include <sstream>
#include <windows.h>
#include <iomanip> 

using namespace std;

const char DELIM = '|';

class Settings {
public:
    string speciality;
    string studentName;
    int subjectsCount;

    Settings() : speciality("Программная инженерия"), studentName("Кристина"), subjectsCount(5) {}
};

class Subject {
private:
    string title;
    int progress;
public:
    Subject(string t = "", int p = 0) : title(t), progress(p) {}
    string getTitle() const { return title; }
    int getProgress() const { return progress; }
    void setProgress(int p) { progress = p; }
    void addProgress(int amount) { progress = min(100, progress + amount); }
};

class Action {
public:
    enum ActionType { VISIT, SKIP, HOME_STUDY, REST };
private:
    ActionType type;
    string label;
    int kChange, eChange, mChange;
public:
    Action(ActionType t, string l, int k, int e, int m) : type(t), label(l), kChange(k), eChange(e), mChange(m) {}
    ActionType getType() const { return type; }
    string getLabel() const { return label; }
    int getK() const { return kChange; }
    int getE() const { return eChange; }
    int getM() const { return mChange; }
};

class Student {
private:
    string name;
    string speciality;
    int knowledge, energy, motivation;
    int money;

    // Вместо vector используем динамический массив
    Subject* subjects;
    int subjectsCount;
    int capacity;

public:
    Student(string n = "", string s = "") : name(n), speciality(s), knowledge(30), energy(80), motivation(60), money(500) {
        capacity = 10;
        subjectsCount = 0;
        subjects = new Subject[capacity];
    }

    // Деструктор для очистки памяти
    ~Student() {
        delete[] subjects;
    }

    // Запрет копирования (чтобы избежать двойного удаления памяти)
    Student(const Student&) = delete;
    Student& operator=(const Student&) = delete;

    void reset(const string& n, const string& s) {
        name = n;
        speciality = s;
        knowledge = 30;
        energy = 80;
        motivation = 60;
        money = 500;
        subjectsCount = 0; // Просто обнуляем счетчик, память перезапишется
    }

    void addSubject(const Subject& subj) {
        if (subjectsCount < capacity) {
            subjects[subjectsCount++] = subj;
        }
    }

    void clearSubjects() { subjectsCount = 0; }

    Subject* getSubjects() { return subjects; }
    int getSubjectsCount() const { return subjectsCount; }

    string getName() const { return name; }
    string getSpeciality() const { return speciality; }
    int getKnowledge() const { return knowledge; }
    int getEnergy() const { return energy; }
    int getMotivation() const { return motivation; }
    int getMoney() const { return money; }

    void setState(int k, int e, int m, int cash) { knowledge = k; energy = e; motivation = m; money = cash; }
    void addMoney(int amount) { money += amount; }

    void executeAction(const Action& act, Subject* target = nullptr) {
        knowledge = max(0, min(100, knowledge + act.getK()));
        energy = max(0, min(100, energy + act.getE()));
        motivation = max(0, min(100, motivation + act.getM()));

        if (target != nullptr) {
            if (act.getType() == Action::VISIT) target->addProgress(15);
            else if (act.getType() == Action::HOME_STUDY) target->addProgress(10);
        }
    }

    void printStatus() const {
        cout << "\n--- " << name << " (" << speciality << ") ---\n";
        cout << "Знания: " << knowledge << " | Энергия: " << energy << " | Мотивация: " << motivation << "\n";
        cout << "Баланс: " << money << " руб.\n";
        for (int i = 0; i < subjectsCount; ++i) {
            cout << " " << i + 1 << ". " << subjects[i].getTitle() << " - " << subjects[i].getProgress() << "%\n";
        }
    }
};

class GameEngine {
private:
    Student student;
    int maxDays;
    int currentDay;

    // Константы игры
    const int MAX_SLOTS = 6;
    const int MONTH_DURATION = 10;
    const int BASE_SALARY = 1000;

public:
    GameEngine(const Settings& cfg) : student(cfg.studentName, cfg.speciality), maxDays(20), currentDay(1) {
        const char* defaultSubjects[] = {
            "Математика", "Программирование", "Физика", "Английский", "История",
            "Базы данных", "Алгоритмы", "Философия", "Экономика", "Право"
        };
        int count = max(1, min(10, cfg.subjectsCount));
        for (int i = 0; i < count; ++i) {
            student.addSubject(Subject(defaultSubjects[i % 10]));
        }
    }

    GameEngine() : maxDays(20), currentDay(1) {}

    string getSlotFilename(int index) const { return "save_slot_" + to_string(index) + ".txt"; }

    bool save(const string& filename) {
        ofstream out(filename);
        if (!out.is_open()) return false;

        out << student.getName() << DELIM << student.getSpeciality() << DELIM
            << student.getKnowledge() << DELIM << student.getEnergy() << DELIM << student.getMotivation() << DELIM
            << student.getMoney() << DELIM
            << currentDay << DELIM << maxDays << "\n";

        out << student.getSubjectsCount() << "\n";
        for (int i = 0; i < student.getSubjectsCount(); ++i) {
            out << student.getSubjects()[i].getTitle() << DELIM << student.getSubjects()[i].getProgress() << "\n";
        }
        return true;
    }

    void saveManualWithSlots() {
        int slotToSave = -1;
        for (int i = 1; i <= MAX_SLOTS; ++i) {
            ifstream check(getSlotFilename(i));
            if (!check.good()) { slotToSave = i; break; }
        }
        if (slotToSave == -1) {
            slotToSave = (currentDay % MAX_SLOTS) + 1;
        }
        if (save(getSlotFilename(slotToSave))) {
            cout << "[Успех] Игра сохранена в слот №" << slotToSave << "\n";
        }
        else { cout << "[Ошибка] Не удалось сохранить!\n"; }
    }

    bool load(const string& filename) {
        ifstream in(filename);
        if (!in.is_open()) return false;

        string line;
        if (!getline(in, line)) return false;

        stringstream ss(line);
        string token;
        auto next = [&](string& t) -> bool { return (bool)getline(ss, t, DELIM); };
        auto toInt = [](const string& v) { try { return stoi(v); } catch (...) { return 0; } };

        string sName, sSpec;
        int k, e, m, cash;
        if (!next(sName) || !next(sSpec) || !next(token)) return false; k = toInt(token);
        if (!next(token)) return false; e = toInt(token);
        if (!next(token)) return false; m = toInt(token);
        if (!next(token)) return false; cash = toInt(token);
        if (!next(token)) return false; currentDay = toInt(token);
        if (!next(token)) return false; maxDays = toInt(token);

        // ИСПОЛЬЗУЕМ НОВЫЙ МЕТОД RESET ВМЕСТО ПРИСВАИВАНИЯ
        student.reset(sName, sSpec);
        student.setState(k, e, m, cash);

        if (!getline(in, line)) return false;
        int count = toInt(line);
        student.clearSubjects();
        for (int i = 0; i < count; ++i) {
            if (!getline(in, line)) return false;
            size_t pos = line.find(DELIM);
            if (pos != string::npos) {
                student.addSubject(Subject(line.substr(0, pos), toInt(line.substr(pos + 1))));
            }
        }
        return true;
    }

    bool peekSlotInfo(int index, int& outDay, int& outKnowledge) {
        ifstream in(getSlotFilename(index));
        if (!in.is_open()) return false;
        string line;
        if (!getline(in, line)) return false;
        stringstream ss(line);
        string token;
        auto next = [&](string& t) -> bool { return (bool)getline(ss, t, DELIM); };
        auto toInt = [](const string& v) { try { return stoi(v); } catch (...) { return 0; } };

        string sName, sSpec;
        if (!next(sName) || !next(sSpec)) return false;
        if (!next(token)) return false; outKnowledge = toInt(token);
        if (!next(token)) return false;
        if (!next(token)) return false;
        if (!next(token)) return false;
        if (!next(token)) return false; outDay = toInt(token);
        return true;
    }

    void play() {
        Action actVisit(Action::VISIT, "Посетить", 10, -15, 5);
        Action actSkip(Action::SKIP, "Прогулять", -5, 10, -10);
        Action actStudy(Action::HOME_STUDY, "Учиться дома", 7, -10, 2);
        Action actRest(Action::REST, "Отдохнуть", 0, 20, 10);

        while (currentDay <= maxDays) {
            student.printStatus();
            cout << "\n--- День " << currentDay << " из " << maxDays << " ---\n";
            cout << "1. Посетить\n2. Прогулять\n3. Учиться дома\n4. Отдохнуть\n5. Сохранить и выйти\nВыбор: ";

            int choice; cin >> choice;
            if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

            if (choice == 5) {
                saveManualWithSlots();
                return;
            }

            Subject* target = nullptr;
            int count = student.getSubjectsCount();
            Subject* subs = student.getSubjects();

            if (choice >= 1 && choice <= 3) {
                cout << "Выберите предмет (0 - отмена):\n";
                for (int i = 0; i < count; ++i) cout << " " << i + 1 << ". " << subs[i].getTitle() << "\n";
                int idx; cin >> idx;
                if (cin.fail() || idx < 0 || idx > count) { cin.clear(); cin.ignore(10000, '\n'); continue; }
                if (idx == 0) continue;
                target = &subs[idx - 1];
            }

            if (choice == 1) student.executeAction(actVisit, target);
            else if (choice == 2) student.executeAction(actSkip, target);
            else if (choice == 3) student.executeAction(actStudy, target);
            else if (choice == 4) student.executeAction(actRest);
            else continue;

            if (currentDay % MONTH_DURATION == 0) {
                processMonthEnd();
            }

            currentDay++;

            if (currentDay <= maxDays) {
                save("save_auto.txt");
            }
        }
        runSession();
    }

private:
    void processMonthEnd() {
        cout << "\n>>> КОНЕЦ МЕСЯЦА. АТТЕСТАЦИЯ <<<\n";
        int knowledge = student.getKnowledge();
        int salary = BASE_SALARY;

        if (knowledge >= 50) {
            cout << "[Успех] Знания (" << knowledge << ") в норме. УРААААА. Стипендия выплачена.\n";
        }
        else if (knowledge >= 30) {
            cout << "[Предупреждение] Низкие знания (" << knowledge << "). О НЕЕЕЕЕТ. Стипендия урезана.\n";
            salary /= 2;
        }
        else {
            cout << "[Провал] Аттестация не пройдена (знания < 30). О НЕЕЕЕТ. Стипендии нет!\n";
            salary = 0;
        }

        student.addMoney(salary);
        cout << "Начислено: " << salary << " руб. (Баланс: " << student.getMoney() << ")\n";
    }

    void runSession() {
        cout << "\n==================================\n          СЕССИЯ НАЧАЛАСЬ!\n==================================\n";
        int totalScore = 0;
        int count = student.getSubjectsCount();
        Subject* subs = student.getSubjects();

        for (int i = 0; i < count; ++i) {
            int baseGrade = max(1, min(5, (subs[i].getProgress() + student.getKnowledge()) / 40));
            cout << "\n--- Экзамен: " << subs[i].getTitle() << " (" << subs[i].getProgress() << "%) ---\n";
            int correct = 0, ans;

            cout << "Вопрос 1. Изучили больше половины? (1/0): "; cin >> ans;
            if (ans == 1 && subs[i].getProgress() >= 50) correct++;
            cout << "Вопрос 2. Регулярно посещали? (1/0): "; cin >> ans;
            if (ans == 1 && subs[i].getProgress() >= 60) correct++;
            cout << "Вопрос 3. Уверены в знаниях? (1/0): "; cin >> ans;
            if (ans == 1 && subs[i].getProgress() >= 70) correct++;

            int bonus = min(2, correct);
            int finalGrade = min(5, baseGrade + bonus);
            cout << "Оценка: " << finalGrade << " (База " << baseGrade << " + Бонус " << bonus << ")\n";
            totalScore += finalGrade;
        }

        double avg = (count == 0) ? 0.0 : (double)totalScore / count;
        cout << "\nСредний балл: " << avg << "\n";
        if (avg >= 4.5) cout << "ОТЛИЧНИК! Стипендия.\n";
        else if (avg >= 3.5) cout << "Хороший результат!\n";
        else if (avg >= 2.5) cout << "Сессия сдана, но есть ДОЛГИ!\n";
        else cout << "ВАС ОТЧИСЛИЛИ...\n";
    }
};

// ================= МЕНЮ =================

void startGame(const Settings& s) {
    GameEngine engine(s);
    engine.play();
    cout << "\nНажмите Enter..."; cin.get(); cin.get();
}

void loadSavedGame() {
    GameEngine tempEngine;
    const int maxSlots = 6;

    cout << "\n=== ДОСТУПНЫЕ СОХРАНЕНИЯ ===\n";
    cout << "0. Автосохранение (save_auto.txt)\n";

    for (int i = 1; i <= maxSlots; ++i) {
        int day, knowledge;
        if (tempEngine.peekSlotInfo(i, day, knowledge)) {
            cout << i << ". Ручное сохранение №" << i << " (День " << day << ", Знания: " << knowledge << ")\n";
        }
        else {
            cout << i << ". [Пустой слот]\n";
        }
    }

    cout << "\nВыберите слот (0 - авто, 1-" << maxSlots << " - ручные, -1 - отмена): ";
    int c; cin >> c;
    if (c == -1) return;

    string file = "";
    if (c == 0) file = "save_auto.txt";
    else if (c >= 1 && c <= maxSlots) file = tempEngine.getSlotFilename(c);
    else return;

    ifstream check(file);
    if (!check.good()) { cout << "[Ошибка] Файл не найден!\n"; cin.get(); cin.get(); return; }

    GameEngine engine;
    if (engine.load(file)) {
        cout << "\n[Успех] Игра загружена.\n";
        engine.play();
    }
    else {
        cout << "[Ошибка] Файл поврежден!\n";
    }
    cin.get(); cin.get();
}

void showAbout() {
    cout << "\n=== О программе ===\nВерсия: 5.0\nСтудент: Бычик К.В.\n";
    cout << "\nНажмите Enter..."; cin.get(); cin.get();
}

void settingsMenu(Settings& s, const list<string>& students, const set<string>& specs) {
    int choice = 0;
    while (choice != 4) {
        cout << "\n--- Настройки ---\n1. Специальность\n2. Имя\n3. Кол-во предметов\n4. Назад\nВыбор: ";
        cin >> choice;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        if (choice == 1) {
            cin.ignore();
            int i = 1;
            for (const auto& item : specs) cout << i++ << ". " << item << "\n";
            cout << "Выберите номер: ";
            int n; cin >> n;
            if (n >= 1 && n <= (int)specs.size()) {
                auto it = specs.begin(); advance(it, n - 1);
                s.speciality = *it;
                ofstream out("settings.txt");
                out << "speciality=" << s.speciality << "\nstudentName=" << s.studentName << "\nsubjectsCount=" << s.subjectsCount << "\n";
            }
        }
        else if (choice == 2) {
            cin.ignore();
            int i = 1;
            for (const auto& item : students) cout << i++ << ". " << item << "\n";
            cout << "Выберите номер: ";
            int n; cin >> n;
            if (n >= 1 && n <= (int)students.size()) {
                auto it = students.begin(); advance(it, n - 1);
                s.studentName = *it;
                ofstream out("settings.txt");
                out << "speciality=" << s.speciality << "\nstudentName=" << s.studentName << "\nsubjectsCount=" << s.subjectsCount << "\n";
            }
        }
        else if (choice == 3) {
            int n; cin >> n;
            if (cin.fail() || n <= 0) { cin.clear(); cin.ignore(10000, '\n'); cout << "Ошибка!\n"; }
            else {
                s.subjectsCount = n;
                ofstream out("settings.txt");
                out << "speciality=" << s.speciality << "\nstudentName=" << s.studentName << "\nsubjectsCount=" << s.subjectsCount << "\n";
            }
        }
    }
}

int main() {
    SetConsoleCP(1251); SetConsoleOutputCP(1251); setlocale(LC_ALL, "Russian");

    Settings cfg;

    // Локальная загрузка настроек
    ifstream inSettings("settings.txt");
    if (inSettings.good()) {
        string line;
        while (getline(inSettings, line)) {
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string key = line.substr(0, pos);
                string val = line.substr(pos + 1);
                if (key == "speciality") cfg.speciality = val;
                else if (key == "studentName") cfg.studentName = val;
                else if (key == "subjectsCount") try { cfg.subjectsCount = stoi(val); }
                catch (...) {}
            }
        }
    }
    else {
        ofstream out("settings.txt");
        out << "speciality=" << cfg.speciality << "\nstudentName=" << cfg.studentName << "\nsubjectsCount=" << cfg.subjectsCount << "\n";
    }

    list<string> studentsList;
    set<string> specialitiesSet;

    // Локальная загрузка списков
    ifstream stFile("students.txt");
    if (!stFile.good()) { ofstream("students.txt") << "Кристина\nИван\nАнна\nПётр\nОльга\n"; }
    stFile.close();
    stFile.open("students.txt");
    string line; while (getline(stFile, line)) if (!line.empty()) studentsList.push_back(line);

    ifstream spFile("specialities.txt");
    if (!spFile.good()) { ofstream("specialities.txt") << "Программная инженерия\nИнформационные технологии\nИскусственный интеллект\nКибербезопасность\nВеб-разработка\n"; }
    spFile.close();
    spFile.open("specialities.txt");
    while (getline(spFile, line)) if (!line.empty()) specialitiesSet.insert(line);

    int menuChoice = 0;
    while (menuChoice != 5) {
        cout << "\n*** Моя жизнь в университете ***\n1. Начать игру\n2. Загрузить игру\n3. Настройки\n4. О программе\n5. Выход\nВыбор: ";
        cin >> menuChoice;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        switch (menuChoice) {
        case 1: startGame(cfg); break;
        case 2: loadSavedGame(); break;
        case 3: settingsMenu(cfg, studentsList, specialitiesSet); break;
        case 4: showAbout(); break;
        case 5: cout << "Выход...\n"; break;
        default: cout << "Нет такого пункта.\n";
        }
    }
    return 0;
}
