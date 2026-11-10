#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <list>
#include <cstdio>
 
using namespace std;

struct SETTING {
    string specialty = "Программная инженерия";
    string studentName = "Аня";
    int subjectCount = 11;
};
 
void saveSettings(const SETTING& userSet) {
    ofstream outFile("settings.txt");
    if (outFile.is_open()) {
        outFile << userSet.specialty << endl;
        outFile << userSet.studentName << endl;
        outFile << userSet.subjectCount << endl;
        outFile.close();
    }
}
 
void loadSettings(SETTING& userSet) {
    ifstream inFile("settings.txt");
    if (inFile.is_open()) {
        getline(inFile, userSet.specialty);
        getline(inFile, userSet.studentName);
        inFile >> userSet.subjectCount;
        inFile.close();
    }
    else {
        saveSettings(userSet);
    }
}
 
void loadSpecialties(set<string>& specialtiesSet) {
    ifstream inFile("specialties.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                specialtiesSet.insert(item);
            }
        }
        inFile.close();
    }
}
 
void loadNames(list<string>& namesList) {
    ifstream inFile("names.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                namesList.push_back(item);
            }
        }
        inFile.close();
    }
}
 
// ---------------------------------------------------------------
// Мини-фреймворк для тестирования
// ---------------------------------------------------------------
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
 
void writeFile(const string& filename, const string& content) {
    ofstream out(filename);
    out << content;
}
 
void cleanup() {
    remove("settings.txt");
    remove("specialties.txt");
    remove("names.txt");
}
 
// ---------------------------------------------------------------
// Тесты
// ---------------------------------------------------------------
void testSaveAndLoadSettings() {
    cout << "\n[1] Сохранение и загрузка настроек\n";
    cleanup();
 
    SETTING a;
    a.specialty = "Кибербезопасность";
    a.studentName = "Мария";
    a.subjectCount = 8;
    saveSettings(a);
 
    SETTING b;
    loadSettings(b);
    checkEq(b.specialty, string("Кибербезопасность"), "специальность загружена");
    checkEq(b.studentName, string("Мария"), "имя загружено");
    checkEq(b.subjectCount, 8, "количество предметов загружено");
    cleanup();
}
 
void testLoadSettingsNoFile() {
    cout << "\n[2] Загрузка настроек без файла\n";
    cleanup();
 
    SETTING s;
    loadSettings(s);
    checkEq(s.specialty, string("Программная инженерия"), "специальность осталась по умолчанию");
    checkEq(s.studentName, string("Аня"), "имя осталось по умолчанию");
    checkEq(s.subjectCount, 11, "предметы остались по умолчанию");
 
    ifstream f("settings.txt");
    check(f.is_open(), "файл settings.txt создан автоматически");
    f.close();
 
    SETTING s2;
    s2.specialty = "другая";
    s2.studentName = "другое";
    s2.subjectCount = 1;
    loadSettings(s2);
    checkEq(s2.specialty, string("Программная инженерия"), "созданный файл содержит значения по умолчанию");
    cleanup();
}
 
void testOverwriteSettings() {
    cout << "\n[3] Перезапись настроек\n";
    cleanup();
 
    SETTING a;
    saveSettings(a);
    a.studentName = "Иван";
    a.subjectCount = 5;
    saveSettings(a);
 
    SETTING b;
    loadSettings(b);
    checkEq(b.studentName, string("Иван"), "новое имя перезаписало старое");
    checkEq(b.subjectCount, 5, "новое количество перезаписало старое");
    checkEq(b.specialty, string("Программная инженерия"), "специальность не изменилась");
    cleanup();
}
 
void testSettingsBoundaryValues() {
    cout << "\n[4] Граничные значения количества предметов\n";
    cleanup();
 
    SETTING a;
    a.subjectCount = 0;
    saveSettings(a);
    SETTING b;
    b.subjectCount = 99;
    loadSettings(b);
    checkEq(b.subjectCount, 0, "значение 0 сохраняется и загружается");
 
    a.subjectCount = -3;
    saveSettings(a);
    loadSettings(b);
    checkEq(b.subjectCount, -3, "отрицательное значение сохраняется как есть");
    cleanup();
}
 
void testLoadSpecialties() {
    cout << "\n[5] Загрузка специальностей (set)\n";
    cleanup();
 
    writeFile("specialties.txt",
              "Программная инженерия\n"
              "Искусственный интеллект\n"
              "\n"
              "Программная инженерия\n"
              "Кибербезопасность\n");
 
    set<string> sp;
    loadSpecialties(sp);
    checkEq((int)sp.size(), 3, "дубликаты и пустые строки отброшены");
    check(sp.count("Программная инженерия") == 1, "есть 'Программная инженерия'");
    check(sp.count("Искусственный интеллект") == 1, "есть 'Искусственный интеллект'");
    check(sp.count("Кибербезопасность") == 1, "есть 'Кибербезопасность'");
    cleanup();
}
 
void testLoadNames() {
    cout << "\n[6] Загрузка имён (list)\n";
    cleanup();
 
    writeFile("names.txt",
              "Аня\n"
              "Иван\n"
              "\n"
              "Мария\n"
              "Аня\n");
 
    list<string> names;
    loadNames(names);
    checkEq((int)names.size(), 4, "пустые строки отброшены, дубликаты сохранены");
    checkEq(names.front(), string("Аня"), "первое имя в исходном порядке");
    checkEq(names.back(), string("Аня"), "последнее имя в исходном порядке");
 
    auto it = names.begin();
    ++it;
    checkEq(*it, string("Иван"), "второе имя 'Иван'");
    cleanup();
}
 
void testLoadMissingFiles() {
    cout << "\n[7] Отсутствующие файлы со списками\n";
    cleanup();
 
    set<string> sp;
    list<string> names;
    loadSpecialties(sp);
    loadNames(names);
    check(sp.empty(), "без specialties.txt множество пустое");
    check(names.empty(), "без names.txt список пустой");
}
 
void testEmptyFiles() {
    cout << "\n[8] Пустые файлы со списками\n";
    cleanup();
 
    writeFile("specialties.txt", "");
    writeFile("names.txt", "\n\n\n");
 
    set<string> sp;
    list<string> names;
    loadSpecialties(sp);
    loadNames(names);
    check(sp.empty(), "пустой specialties.txt даёт пустое множество");
    check(names.empty(), "names.txt из одних пустых строк даёт пустой список");
    cleanup();
}
 
void testSpecialtiesSorted() {
    cout << "\n[9] Порядок элементов в set\n";
    cleanup();
 
    writeFile("specialties.txt", "В\nА\nБ\n");
    set<string> sp;
    loadSpecialties(sp);
    checkEq(*sp.begin(), string("А"), "set хранит элементы отсортированными");
    cleanup();
}
 
int main() {
    cout << "=============================================\n";
    cout << " Запуск тестов: Моя жизнь в университете (лаб. 2)\n";
    cout << "=============================================\n";
 
    testSaveAndLoadSettings();
    testLoadSettingsNoFile();
    testOverwriteSettings();
    testSettingsBoundaryValues();
    testLoadSpecialties();
    testLoadNames();
    testLoadMissingFiles();
    testEmptyFiles();
    testSpecialtiesSorted();
 
    cleanup();
 
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
