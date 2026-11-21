#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
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

// ==========================================
// Копии классов из main.cpp для тестирования
// ==========================================
const char SEP = '|';

class Subject {
private:
    string title;
    int progress;
public:
    Subject(string t = "", int p = 0) : title(t), progress(p) {}
    string getTitle() const { return title; }
    int getProgress() const { return progress; }
    void setProgress(int p) { progress = p; }
};

class Student {
private:
    string name;
    string speciality;
    int knowledge, energy, motivation;
    vector<Subject> subjects;

public:
    Student(string n = "", string s = "") : name(n), speciality(s), knowledge(30), energy(80), motivation(60) {}
    
    void addSubject(const Subject& subj) { subjects.push_back(subj); }
    void clearSubjects() { subjects.clear(); }
    vector<Subject>& getSubjects() { return subjects; }
    
    string getName() const { return name; }
    string getSpeciality() const { return speciality; }
    int getKnowledge() const { return knowledge; }
    int getEnergy() const { return energy; }
    int getMotivation() const { return motivation; }

    void setState(int k, int e, int m) { knowledge = k; energy = e; motivation = m; }
};

// ==========================================
// Тестируемые функции сохранения/загрузки
// ==========================================
bool testSave(const Student& student, int currentDay, int maxDays, const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) return false;

    out << student.getName() << SEP << student.getSpeciality() << SEP
        << student.getKnowledge() << SEP << student.getEnergy() << SEP << student.getMotivation() << SEP
        << currentDay << SEP << maxDays << "\n";

    auto& subs = student.getSubjects();
    out << subs.size() << "\n";
    for (const auto& s : subs) {
        out << s.getTitle() << SEP << s.getProgress() << "\n";
    }
    return true;
}

bool testLoad(Student& student, int& currentDay, int& maxDays, const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) return false;

    string line;
    if (!getline(in, line)) return false;

    stringstream ss(line);
    string token;
    auto next = [&](string& t) -> bool { return (bool)getline(ss, t, SEP); };
    auto toInt = [](const string& v) { try { return stoi(v); } catch (...) { return 0; } };

    string sName, sSpec;
    int k, e, m;
    
    // Читаем шапку
    if (!next(sName) || !next(sSpec) || !next(token)) return false; k = toInt(token);
    if (!next(token)) return false; e = toInt(token);
    if (!next(token)) return false; m = toInt(token);
    if (!next(token)) return false; currentDay = toInt(token);
    if (!next(token)) return false; maxDays = toInt(token);

    // Восстанавливаем студента
    student = Student(sName, sSpec);
    student.setState(k, e, m);

    // Читаем предметы
    if (!getline(in, line)) return false;
    int count = toInt(line);
    student.clearSubjects();
    
    for (int i = 0; i < count; ++i) {
        if (!getline(in, line)) return false;
        size_t pos = line.find(SEP);
        if (pos != string::npos) {
            student.addSubject(Subject(line.substr(0, pos), toInt(line.substr(pos + 1))));
        }
    }
    return true;
}

// ==========================================
// Сами тесты
// ==========================================

void test_save_and_load_integrity() {
    cout << "--- Тест 1: Полный цикл сохранения и загрузки ---\n";
    
    Student original("Кристина", "Программная инженерия");
    original.setState(75, 40, 55);
    original.addSubject(Subject("Математика", 80));
    original.addSubject(Subject("Физика", 60));
    
    int day = 5;
    int maxDays = 10;
    string test_file = "test_integrity.txt";

    // Сохраняем
    check(testSave(original, day, maxDays, test_file), "Файл успешно создан и записан");

    // Загружаем
    Student loaded;
    int loadedDay = 1, loadedMaxDays = 0;
    check(testLoad(loaded, loadedDay, loadedMaxDays, test_file), "Файл успешно прочитан");

    // Проверяем данные
    check(loaded.getName() == "Кристина", "Имя студента восстановлено верно");
    check(loaded.getSpeciality() == "Программная инженерия", "Специальность восстановлена верно");
    check(loaded.getKnowledge() == 75, "Знания восстановлены верно");
    check(loaded.getEnergy() == 40, "Энергия восстановлена верно");
    check(loaded.getMotivation() == 55, "Мотивация восстановлена верно");
    check(loadedDay == 5, "Текущий день восстановлен верно");
    check(loadedMaxDays == 10, "Максимальный день восстановлен верно");
    
    check(loaded.getSubjects().size() == 2, "Количество предметов совпадает");
    check(loaded.getSubjects()[0].getTitle() == "Математика", "Название 1-го предмета верно");
    check(loaded.getSubjects()[0].getProgress() == 80, "Прогресс 1-го предмета верен");
    check(loaded.getSubjects()[1].getProgress() == 60, "Прогресс 2-го предмета верен");

    remove(test_file.c_str());
}

void test_error_handling() {
    cout << "\n--- Тест 2: Обработка ошибок (несуществующий файл) ---\n";
    Student st;
    int d = 0, md = 0;
    
    // Пытаемся загрузить файл, которого нет
    bool result = testLoad(st, d, md, "non_existent_file_12345.txt");
    check(result == false, "Загрузка несуществующего файла возвращает false");
}

void test_auto_save_simulation() {
    cout << "\n--- Тест 3: Симуляция автосохранения ---\n";
    
    // Симулируем, что игрок сыграл 1-й день и произошло автосохранение
    Student player("Иван", "ИИТ");
    player.addSubject(Subject("Алгоритмы", 15)); // +15% за посещение
    int day1 = 2; // День увеличился после действия
    
    string auto_save_file = "test_auto.txt";
    testSave(player, day1, 10, auto_save_file);
    
    // Проверяем, что файл автосохранения реально создался на диске
    ifstream check_file(auto_save_file);
    check(check_file.good(), "Файл автосохранения создан на диске");
    check_file.close();

    // Загружаем автосохранение
    Student restored;
    int r_day = 1, r_max = 0;
    testLoad(restored, r_day, r_max, auto_save_file);
    
    check(r_day == 2, "Автосохранение запомнило переход на 2-й день");
    check(restored.getSubjects()[0].getProgress() == 15, "Автосохранение запомнило прогресс предмета");

    remove(auto_save_file.c_str());
}

int main() {
    cout << "=== Тестирование ЛР4 (Сохранение и загрузка) ===\n\n";
    
    test_save_and_load_integrity();
    test_error_handling();
    test_auto_save_simulation();

    cout << "\n=====================================\n";
    cout << "Итог: Пройдено " << passed_tests << " из " << total_tests << " тестов\n";
    cout << "=====================================\n";

    return (passed_tests == total_tests) ? 0 : 1;
}
