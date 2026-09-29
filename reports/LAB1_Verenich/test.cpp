#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>

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

// ==================== ИМЯ БИНАРНИКА ====================
const string BINARY = "./farm";

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

// Запуск программы с заданным вводом и возврат её вывода
string runProgram(const string& input)
{
    // Создаём временный файл с входными данными
    string inputFile = "test_input.tmp";
    string outputFile = "test_output.tmp";

    ofstream fin(inputFile);
    fin << input;
    fin.close();

    // Формируем команду: ./farm < test_input.tmp > test_output.tmp 2>&1
    string command = BINARY + " < " + inputFile + " > " + outputFile + " 2>&1";
    system(command.c_str());

    // Читаем результат
    ifstream fout(outputFile);
    stringstream ss;
    ss << fout.rdbuf();
    fout.close();

    // Удаляем временные файлы
    remove(inputFile.c_str());
    remove(outputFile.c_str());

    return ss.str();
}

// Проверка наличия подстроки в выводе
bool contains(const string& text, const string& substring)
{
    return text.find(substring) != string::npos;
}

// Подсчёт вхождений подстроки
int countOccurrences(const string& text, const string& substring)
{
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(substring, pos)) != string::npos)
    {
        count++;
        pos += substring.length();
    }
    return count;
}

// Печать результата теста
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

// ==================== ТЕСТЫ ====================

// Тест 1: Выход из программы
void testExit()
{
    string output = runProgram("5\n");
    printResult("Тест 1: Выход из программы (пункт 5)",
        contains(output, "Выход из программы"));
}

// Тест 2: Отображение главного меню
void testMainMenu()
{
    string output = runProgram("5\n");
    bool ok = contains(output, "ВЕСЕЛАЯ ФЕРМА") &&
        contains(output, "1. Начать игру") &&
        contains(output, "2. Загрузить игру") &&
        contains(output, "3. Настройки") &&
        contains(output, "4. О программе") &&
        contains(output, "5. Выход");
    printResult("Тест 2: Отображение главного меню", ok);
}

// Тест 3: Пункт "Начать игру"
void testStartGame()
{
    string output = runProgram("1\n\n5\n");
    printResult("Тест 3: Пункт \"Начать игру\"",
        contains(output, "НАЧАЛО ИГРЫ"));
}

// Тест 4: Пункт "Загрузить игру"
void testLoadGame()
{
    string output = runProgram("2\n\n5\n");
    printResult("Тест 4: Пункт \"Загрузить игру\" (заглушка)",
        contains(output, "недоступна"));
}

// Тест 5: Пункт "О программе"
void testAbout()
{
    string output = runProgram("4\n\n5\n");
    bool ok = contains(output, "Версия программы") &&
        contains(output, "Студент");
    printResult("Тест 5: Пункт \"О программе\"", ok);
}

// Тест 6: Открытие подменю "Настройки"
void testSettingsOpen()
{
    string output = runProgram("3\n4\n5\n");
    bool ok = contains(output, "НАСТРОЙКИ") &&
        contains(output, "Уровень сложности");
    printResult("Тест 6: Открытие подменю \"Настройки\"", ok);
}

// Тест 7: Изменение уровня сложности
void testDifficultyChange()
{
    string output = runProgram("3\n1\n3\n4\n4\n5\n");
    printResult("Тест 7: Изменение сложности на \"Сложный\"",
        contains(output, "Уровень сложности установлен: Сложный"));
}

// Тест 8: Сохранение настроек между заходами
void testSettingsPersistence()
{
    // Ставим "Легкий", выходим, заходим снова — должно быть "Легкий"
    string output = runProgram("3\n1\n1\n4\n4\n3\n4\n5\n");
    int count = countOccurrences(output, "текущий: Легкий");
    printResult("Тест 8: Сохранение настроек между заходами", count >= 2);
}

// Тест 9: Изменение количества коров
void testCowsChange()
{
    string output = runProgram("3\n2\n50\n4\n4\n5\n");
    printResult("Тест 9: Изменение количества коров",
        contains(output, "Количество (Коровы) установлено: 50"));
}

// Тест 10: Изменение количества куриц
void testChickensChange()
{
    string output = runProgram("3\n3\n25\n4\n4\n5\n");
    printResult("Тест 10: Изменение количества куриц",
        contains(output, "Количество (Курицы) установлено: 25"));
}

// Тест 11: Некорректный пункт меню
void testInvalidMenu()
{
    string output = runProgram("99\n5\n");
    printResult("Тест 11: Обработка неверного пункта меню",
        contains(output, "Неверный пункт меню"));
}

// Тест 12: Некорректный ввод (буква вместо цифры)
void testInvalidInput()
{
    string output = runProgram("abc\n5\n");
    printResult("Тест 12: Обработка нечислового ввода",
        contains(output, "Ошибка ввода"));
}

// Тест 13: Проверка диапазона количества животных
void testAnimalRange()
{
    string output = runProgram("3\n2\n2000\n10\n4\n4\n5\n");
    printResult("Тест 13: Проверка диапазона количества животных",
        contains(output, "Недопустимое значение"));
}

// Тест 14: Возврат в главное меню после игры
void testReturnToMenu()
{
    string output = runProgram("1\n\n5\n");
    int count = countOccurrences(output, "ВЕСЕЛАЯ ФЕРМА");
    printResult("Тест 14: Возврат в главное меню после игры", count >= 2);
}

// Тест 15: Значения по умолчанию
void testDefaults()
{
    string output = runProgram("3\n4\n5\n");
    bool ok = contains(output, "текущий: Средний") &&
        contains(output, "коров (текущее: 5)") &&
        contains(output, "куриц (текущее: 10)");
    printResult("Тест 15: Значения по умолчанию", ok);
}

// Тест 16: Смена сложности на "Легкий"
void testDifficultyEasy()
{
    string output = runProgram("3\n1\n1\n4\n4\n5\n");
    printResult("Тест 16: Изменение сложности на \"Легкий\"",
        contains(output, "Уровень сложности установлен: Легкий"));
}

// Тест 17: Смена сложности на "Средний"
void testDifficultyNormal()
{
    string output = runProgram("3\n1\n2\n4\n4\n5\n");
    printResult("Тест 17: Изменение сложности на \"Средний\"",
        contains(output, "Уровень сложности установлен: Средний"));
}

// Тест 18: Отмена изменения количества (ввод -1)
void testAnimalCancel()
{
    string output = runProgram("3\n2\n-1\n4\n4\n5\n");
    // Значение должно остаться 5 (по умолчанию)
    printResult("Тест 18: Отмена изменения количества (ввод -1)",
        contains(output, "Текущее значение: 5"));
}

// ==================== MAIN ====================

int main()
{
    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  Тестирование программы \"Веселая ферма\"\n" << NC;
    cout << BLUE << "============================================\n" << NC;
    cout << "\n";

    // Проверка наличия бинарника
    ifstream check(BINARY);
    if (!check.good())
    {
        cout << RED << "Ошибка: бинарник " << BINARY << " не найден!\n" << NC;
        cout << "Сначала соберите программу:\n";
        cout << "  g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o farm\n";
        return 1;
    }
    check.close();

    cout << YELLOW << ">>> Запуск тестов...\n" << NC << "\n";

    // Запуск всех тестов
    testExit();
    testMainMenu();
    testStartGame();
    testLoadGame();
    testAbout();
    testSettingsOpen();
    testDifficultyChange();
    testSettingsPersistence();
    testCowsChange();
    testChickensChange();
    testInvalidMenu();
    testInvalidInput();
    testAnimalRange();
    testReturnToMenu();
    testDefaults();
    testDifficultyEasy();
    testDifficultyNormal();
    testAnimalCancel();

    // Итоговый отчёт
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
    else
    {
        cout << RED << ">>> ЕСТЬ ПРОВАЛЕННЫЕ ТЕСТЫ! <<<\n" << NC;
        return 1;
    }
}