#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <clocale>

#ifdef _WIN32
#define NOMINMAX         
#include <windows.h>
#endif

using namespace std;

// ==================== ЦВЕТА (ANSI) ====================
const string GREEN = "\033[0;32m";
const string RED = "\033[0;31m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[0;34m";
const string NC = "\033[0m";

// ==================== СЧЁТЧИКИ ====================
int passed = 0;
int failed = 0;
int total = 0;

// ==================== ИМЕНА ====================
const string BINARY = "./farm";
const string SETTINGS_FILE = "settings.txt";

// ==================== НАСТРОЙКА ЛОКАЛИ ====================
void setupLocale()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
}

// ==================== УТИЛИТЫ ====================

string runProgram(const string& input)
{
    string inputFile = "test_input.tmp";
    string outputFile = "test_output.tmp";

    ofstream fin(inputFile);
    fin << input;
    fin.close();

    string command = BINARY + " < " + inputFile + " > " + outputFile + " 2>&1";
    system(command.c_str());

    ifstream fout(outputFile);
    stringstream ss;
    ss << fout.rdbuf();
    fout.close();

    remove(inputFile.c_str());
    remove(outputFile.c_str());

    return ss.str();
}

bool contains(const string& text, const string& sub)
{
    return text.find(sub) != string::npos;
}

int countOccurrences(const string& text, const string& sub)
{
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(sub, pos)) != string::npos)
    {
        count++;
        pos += sub.length();
    }
    return count;
}

void printResult(const string& name, bool ok)
{
    total++;
    if (ok)
    {
        cout << GREEN << "[PASS]" << NC << " " << name << "\n";
        passed++;
    }
    else
    {
        cout << RED << "[FAIL]" << NC << " " << name << "\n";
        failed++;
    }
}

void removeSettings()
{
    remove(SETTINGS_FILE.c_str());
}

string readSettingsFile()
{
    ifstream fin(SETTINGS_FILE);
    if (!fin.is_open()) return "";
    stringstream ss;
    ss << fin.rdbuf();
    fin.close();
    return ss.str();
}

bool checkBinaryExists()
{
    ifstream check(BINARY);
    bool ok = check.good();
    check.close();
    return ok;
}

// ==================== ТЕСТЫ ЛР №1 ====================

void testMainMenu()
{
    string output = runProgram("5\n");
    bool ok = contains(output, "ВЕСЕЛАЯ ФЕРМА") &&
        contains(output, "1. Начать игру") &&
        contains(output, "2. Загрузить игру") &&
        contains(output, "3. Настройки") &&
        contains(output, "4. О программе") &&
        contains(output, "5. Выход");
    printResult("Тест 1: Главное меню", ok);
}

void testStartGame()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n\n5\n");
    bool ok = contains(output, "НАЧАЛО ИГРЫ") &&
        contains(output, "Cows") &&
        contains(output, "Chickens");
    printResult("Тест 2: Пункт \"Начать игру\" (Cows, Chickens)", ok);
}

void testLoadGame()
{
    string output = runProgram("2\n\n5\n");
    printResult("Тест 3: Пункт \"Загрузить игру\" (заглушка)",
        contains(output, "недоступна"));
}

void testAbout()
{
    string output = runProgram("4\n\n5\n");
    bool ok = contains(output, "2.0") && contains(output, "Студент");
    printResult("Тест 4: Пункт \"О программе\"", ok);
}

void testInvalidMenu()
{
    string output = runProgram("99\n5\n");
    printResult("Тест 5: Обработка неверного пункта меню",
        contains(output, "Неверный пункт меню"));
}

void testInvalidInput()
{
    string output = runProgram("abc\n5\n");
    printResult("Тест 6: Обработка нечислового ввода",
        contains(output, "Ошибка ввода"));
}

void testReturnToMenu()
{
    string output = runProgram("1\n\n5\n");
    int count = countOccurrences(output, "ВЕСЕЛАЯ ФЕРМА");
    printResult("Тест 7: Возврат в главное меню после игры", count >= 2);
}

void testDifficultyEasy()
{
    removeSettings();
    string output = runProgram("3\n1\n1\n4\n4\n5\n");
    printResult("Тест 8: Изменение сложности на \"Easy\"",
        contains(output, "Уровень сложности установлен: Easy"));
}

void testDifficultyHard()
{
    removeSettings();
    string output = runProgram("3\n1\n3\n4\n4\n5\n");
    printResult("Тест 9: Изменение сложности на \"Hard\"",
        contains(output, "Уровень сложности установлен: Hard"));
}

// ==================== ТЕСТЫ ЛР №2 ====================

void testFirstRunCreatesFile()
{
    removeSettings();
    runProgram("5\n");
    ifstream f(SETTINGS_FILE);
    printResult("Тест 10: Создание settings.txt при первом запуске", f.good());
    f.close();
}

void testDefaultFileContent()
{
    removeSettings();
    runProgram("5\n");
    string content = readSettingsFile();
    bool ok = contains(content, "difficulty=Medium") &&
        contains(content, "animals=Cows:5") &&
        contains(content, "animals=Chickens:10");
    printResult("Тест 11: Значения по умолчанию (Cows, Chickens)", ok);
}

void testSaveDifficulty()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n");
    string content = readSettingsFile();
    printResult("Тест 12: Сохранение сложности в файл",
        contains(content, "difficulty=Hard"));
}

void testLoadDifficulty()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n");
    string output = runProgram("3\n4\n5\n");
    printResult("Тест 13: Загрузка сложности из файла",
        contains(output, "текущий: Easy"));
}

void testSaveAnimalCount()
{
    removeSettings();
    runProgram("3\n2\n1\n50\n3\n0\n4\n5\n");
    string content = readSettingsFile();
    printResult("Тест 14: Сохранение количества Cows",
        contains(content, "animals=Cows:50"));
}

void testLoadAnimalCount()
{
    removeSettings();
    runProgram("3\n2\n1\n42\n3\n0\n4\n5\n");
    string output = runProgram("3\n4\n5\n");
    printResult("Тест 15: Загрузка количества Cows из файла",
        contains(output, "Cows — 42"));
}

void testAddAnimal()
{
    removeSettings();
    runProgram("3\n3\nSheep\n15\n4\n5\n");
    string content = readSettingsFile();
    printResult("Тест 16: Добавление нового животного (Sheep)",
        contains(content, "animals=Sheep:15"));
}

void testAddAnimalPersists()
{
    removeSettings();
    runProgram("3\n3\nGoats\n7\n4\n5\n");
    string output = runProgram("3\n2\n0\n4\n5\n");
    printResult("Тест 17: Новое животное (Goats) сохраняется между запусками",
        contains(output, "Goats — 7"));
}

void testDeleteAnimal()
{
    removeSettings();
    runProgram("3\n2\n1\n2\n0\n4\n5\n");
    string content = readSettingsFile();
    printResult("Тест 18: Удаление Cows из файла",
        !contains(content, "animals=Cows"));
}

void testSettingsPersistAfterRestart()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n"
        "2\n1\n99\n3\n0\n"
        "3\nPigs\n33\n"
        "4\n5\n");

    string output = runProgram("3\n4\n5\n");
    bool ok = contains(output, "текущий: Hard") &&
        contains(output, "Cows — 99") &&
        contains(output, "Pigs — 33");
    printResult("Тест 19: Все настройки сохраняются после перезапуска", ok);
}

void testInvalidAnimalValue()
{
    removeSettings();
    runProgram("3\n2\n1\n2000\n10\n3\n0\n4\n5\n");
    string content = readSettingsFile();
    printResult("Тест 20: Некорректное значение не сохраняется в файл",
        contains(content, "animals=Cows:10"));
}

// ==================== MAIN ====================

int main()
{
    setupLocale();

    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  Тесты: Веселая ферма v2.0\n" << NC;
    cout << BLUE << "  (ЛР №1: меню + ЛР №2: файлы)\n" << NC;
    cout << BLUE << "============================================\n\n" << NC;

    if (!checkBinaryExists())
    {
        cout << RED << "Ошибка: бинарник " << BINARY << " не найден!\n" << NC;
        cout << "Сначала соберите программу:\n";
        cout << "  g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o farm\n";
        return 1;
    }

    cout << YELLOW << ">>> Запуск тестов...\n\n" << NC;

    // --- ЛР №1: меню ---
    cout << BLUE << "--- ЛР №1: главное меню ---\n" << NC;
    testMainMenu();
    testStartGame();
    testLoadGame();
    testAbout();
    testInvalidMenu();
    testInvalidInput();
    testReturnToMenu();
    testDifficultyEasy();
    testDifficultyHard();

    // --- ЛР №2: файлы ---
    cout << "\n" << BLUE << "--- ЛР №2: работа с файлами ---\n" << NC;
    testFirstRunCreatesFile();
    testDefaultFileContent();
    testSaveDifficulty();
    testLoadDifficulty();
    testSaveAnimalCount();
    testLoadAnimalCount();
    testAddAnimal();
    testAddAnimalPersists();
    testDeleteAnimal();
    testSettingsPersistAfterRestart();
    testInvalidAnimalValue();

    // --- Итог ---
    cout << "\n";
    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  ИТОГОВЫЙ ОТЧЁТ\n" << NC;
    cout << BLUE << "============================================\n" << NC;
    cout << "Всего тестов:   " << total << "\n";
    cout << GREEN << "Пройдено:       " << passed << NC << "\n";
    cout << RED << "Провалено:      " << failed << NC << "\n";

    if (failed == 0)
    {
        cout << GREEN << ">>> ВСЕ ТЕСТЫ ПРОЙДЕНЫ! <<<\n" << NC;
        return 0;
    }
    cout << RED << ">>> ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ! <<<\n" << NC;
    return 1;
}