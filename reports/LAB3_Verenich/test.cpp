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

// ==================== ЦВЕТА ДЛЯ ВЫВОДА ====================
const string GREEN = "\033[0;32m";
const string RED = "\033[0;31m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[0;34m";
const string NC = "\033[0m";

// ==================== СЧЁТЧИКИ ====================
int passed = 0, failed = 0, total = 0;

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

// Запуск основной программы с заданным вводом (симуляция действий пользователя)
string runProgram(const string& input)
{
    string inputFile = "test_input.tmp";
    string outputFile = "test_output.tmp";

    ofstream fin(inputFile);
    fin << input;
    fin.close();

    string cmd = BINARY + " < " + inputFile + " > " + outputFile + " 2>&1";
    system(cmd.c_str());

    ifstream fout(outputFile);
    stringstream ss;
    ss << fout.rdbuf();
    fout.close();

    remove(inputFile.c_str());
    remove(outputFile.c_str());
    return ss.str();
}

// Проверка наличия подстроки в тексте
bool contains(const string& text, const string& sub)
{
    return text.find(sub) != string::npos;
}

// Вывод результата теста
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

// Удаление файла настроек перед тестом
void removeSettings()
{
    remove(SETTINGS_FILE.c_str());
}

// Проверка существования исполняемого файла игры
bool checkBinaryExists()
{
    ifstream c(BINARY);
    bool ok = c.good();
    c.close();
    return ok;
}

// ==================== ТЕСТЫ (ЛР №3) ====================

// Тест 1: Запуск игры — отображается день, деньги, действия
void testStartGameShowsState()
{
    removeSettings();
    runProgram("5\n"); // создаём файл настроек по умолчанию
    string output = runProgram("1\n\n\n2\n5\n"); // зашли в игру, завершили день, вышли
    bool ok = contains(output, "День:") &&
        contains(output, "Деньги:") &&
        contains(output, "Действий:");
    printResult("Тест 1: Запуск игры — состояние игрока", ok);
}

// Тест 2: На старте есть животные из настроек (Cows, Chickens)
void testStartGameHasAnimals()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    bool ok = contains(output, "Cows") &&
        contains(output, "Chickens");
    printResult("Тест 2: Стартовые животные из settings", ok);
}

// Тест 3: Действий в день зависит от сложности (Easy = 5)
void testActionsEasy()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Устанавливаем Easy
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Тест 3: Easy — 5 действий в день",
        contains(output, "Действий: 5"));
}

// Тест 4: Действий в день зависит от сложности (Hard = 3)
void testActionsHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n"); // Устанавливаем Hard
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Тест 4: Hard — 3 действия в день",
        contains(output, "Действий: 3"));
}

// Тест 5: Деньги зависят от сложности (Easy = 2000)
void testMoneyEasy()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Тест 5: Easy — 2000 денег",
        contains(output, "Деньги: 2000"));
}

// Тест 6: Деньги зависят от сложности (Hard = 500)
void testMoneyHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Тест 6: Hard — 500 денег",
        contains(output, "Деньги: 500"));
}

// Тест 7: Отображается меню дня (4 действия)
void testDayMenu()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    bool ok = contains(output, "Покормить") &&
        contains(output, "Позаботиться") &&
        contains(output, "Купить еду") &&
        contains(output, "Купить животное");
    printResult("Тест 7: Меню дня со всеми действиями", ok);
}

// Тест 8: Кормление животного (есть еда на складе)
void testFeedAnimal()
{
    removeSettings();
    runProgram("5\n");
    // Игра -> кормить -> 1-е животное -> завершить день -> выйти
    string output = runProgram("1\n1\n1\n2\n2\n5\n");
    printResult("Тест 8: Кормление животного",
        contains(output, "покормлено"));
}

// Тест 9: Забота о животном
void testCareAnimal()
{
    removeSettings();
    runProgram("5\n");
    // Игра -> забота -> 1-е животное -> завершить день -> выйти
    string output = runProgram("1\n2\n1\n2\n2\n5\n");
    printResult("Тест 9: Забота о животном",
        contains(output, "обласкано"));
}

// Тест 10: Покупка еды (Easy — денег хватает)
void testBuyFood()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy
    // Игра -> купить еду -> 1 (Hay) -> 10 шт -> завершить день -> выйти
    string output = runProgram("1\n3\n1\n10\n2\n2\n5\n");
    printResult("Тест 10: Покупка еды",
        contains(output, "Куплено 10"));
}

// Тест 11: Покупка животного
void testBuyAnimal()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy
    // Игра -> купить животное -> 1 (Cows) -> завершить день -> выйти
    string output = runProgram("1\n4\n1\n2\n2\n5\n");
    printResult("Тест 11: Покупка животного",
        contains(output, "Куплено"));
}

// Тест 12: Завершение дня — увеличивает номер дня
void testEndDay()
{
    removeSettings();
    runProgram("5\n");
    // Игра -> завершить день -> выйти
    string output = runProgram("1\n2\n2\n5\n");
    printResult("Тест 12: Завершение дня увеличивает день",
        contains(output, "День: 2"));
}

// Тест 13: Доход от животных начисляется (деньги растут)
void testIncome()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy, денег 2000
    string output = runProgram("1\n2\n2\n5\n");

    // После первого дня доход должен добавиться к 2000
    // Cows: 2 * (15*50/100=7) = 14
    // Chickens: 3 * (5*50/100=2) = 6
    // Итого: 2020
    printResult("Тест 13: Доход начисляется (деньги растут)",
        contains(output, "Деньги: 2020"));
}

// Тест 14: Нет действий — программа требует завершить день
void testNoActionsLeft()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n"); // Hard: 3 действия
    // Тратим 3 действия на заботу -> дальше должно выскочить предупреждение
    string output = runProgram("1\n"
        "2\n1\n2\n1\n2\n1\n" // 3 заботы
        "1\n"                // завершить день
        "2\n5\n");           // выйти
    printResult("Тест 14: При окончании действий — завершение дня",
        contains(output, "Действия на сегодня закончились"));
}

// Тест 15: При выборе 5 (выход из игры) — возврат в главное меню
void testExitFromGame()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n5\n5\n");
    printResult("Тест 15: Выход из игры в главное меню",
        contains(output, "ВЕСЕЛАЯ ФЕРМА"));
}

// Тест 16: Игра работает и на Hard (3 действия)
void testGameHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n");
    string output = runProgram("1\n"
        "1\n1\n1\n1\n1\n1\n" // 3 попытки кормления
        "2\n2\n5\n");        // завершить и выйти
    bool ok = contains(output, "День:") &&
        contains(output, "Действий: 3");
    printResult("Тест 16: Игра на Hard работает", ok);
}

// ==================== MAIN ====================

int main()
{
    setupLocale();

    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  Тесты: Веселая ферма v3.0\n" << NC;
    cout << BLUE << "  (ЛР №3: игровая логика)\n" << NC;
    cout << BLUE << "============================================\n\n" << NC;

    if (!checkBinaryExists())
    {
        cout << RED << "Бинарник " << BINARY << " не найден!\n" << NC;
        cout << "Сначала соберите программу:\n";
        cout << "  g++ -std=c++17 main.cpp -o farm\n";
        return 1;
    }

    cout << YELLOW << ">>> Запуск тестов...\n\n" << NC;

    // Запуск всех тестов
    testStartGameShowsState();
    testStartGameHasAnimals();
    testActionsEasy();
    testActionsHard();
    testMoneyEasy();
    testMoneyHard();
    testDayMenu();
    testFeedAnimal();
    testCareAnimal();
    testBuyFood();
    testBuyAnimal();
    testEndDay();
    testIncome();
    testNoActionsLeft();
    testExitFromGame();
    testGameHard();

    // Итоговый отчёт
    cout << "\n";
    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  ИТОГОВЫЙ ОТЧЁТ\n" << NC;
    cout << BLUE << "============================================\n" << NC;
    cout << "Всего:    " << total << "\n";
    cout << GREEN << "Пройдено: " << passed << NC << "\n";
    cout << RED << "Провалено:" << failed << NC << "\n";

    if (failed == 0)
    {
        cout << GREEN << ">>> ВСЕ ТЕСТЫ ПРОЙДЕНЫ! <<<\n" << NC;
        return 0;
    }

    cout << RED << ">>> ЕСТЬ ПРОВАЛЫ <<<\n" << NC;
    return 1;
}