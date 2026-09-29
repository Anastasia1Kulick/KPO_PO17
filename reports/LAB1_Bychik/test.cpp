#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static int passed = 0;
static int failed = 0;

#define CHECK(cond)                                                     \
    do {                                                                \
        if (cond) {                                                     \
            ++passed;                                                   \
            std::cout << "[ OK ] " << #cond << "\n";                    \
        } else {                                                        \
            ++failed;                                                   \
            std::cout << "[FAIL] " << #cond                             \
                      << "  (строка " << __LINE__ << ")\n";             \
        }                                                               \
    } while (0)

static std::string runGame(const std::string& input) {
    // 1. Пишем входные данные в файл
    {
        std::ofstream in("input.txt", std::ios::binary);
        in << input;
    }

    // 2. Запускаем программу, перенаправляя stdin и stdout
#ifdef _WIN32
    std::system("farm.exe < input.txt > output.txt");
#else
    std::system("./farm < input.txt > output.txt");
#endif

    // 3. Читаем результат
    std::ifstream out("output.txt", std::ios::binary);
    std::stringstream ss;
    ss << out.rdbuf();
    return ss.str();
}

static bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}


static void testMainMenuExit() {
    std::cout << "\n--- Пункт 5: выход из программы ---\n";
    std::string out = runGame("5\n");
    CHECK(contains(out, "Моя жизнь в университете"));
    CHECK(contains(out, "Начать игру"));
    CHECK(contains(out, "Загрузить игру"));
    CHECK(contains(out, "Настройки"));
    CHECK(contains(out, "О программе"));
    CHECK(contains(out, "Выход из программы"));
}

static void testStartGame() {
    std::cout << "\n--- Пункт 1: начать игру ---\n";
    std::string out = runGame("1\n\n5\n");  // 1 -> Enter -> выход
    CHECK(contains(out, "Начало игры"));
    CHECK(contains(out, "Вы поступаете в университет"));
}

static void testLoadGame() {
    std::cout << "\n--- Пункт 2: загрузить игру ---\n";
    std::string out = runGame("2\n\n5\n");
    CHECK(contains(out, "Функция загрузки пока недоступна"));
    CHECK(contains(out, "ближайших обновлениях"));
}

static void testAbout() {
    std::cout << "\n--- Пункт 4: о программе ---\n";
    std::string out = runGame("4\n\n5\n");
    CHECK(contains(out, "О программе"));
    CHECK(contains(out, "Версия: 1.0"));
}

static void testInvalidChoice() {
    std::cout << "\n--- Неверный пункт главного меню ---\n";
    std::string out = runGame("99\n5\n");
    CHECK(contains(out, "Такого пункта нет"));
}

static void testInvalidInput() {
    std::cout << "\n--- Некорректный ввод (буква вместо числа) ---\n";
    std::string out = runGame("abc\n5\n");
    CHECK(contains(out, "Некорректный ввод"));
}

static void testChangeName() {
    std::cout << "\n--- Смена имени студента ---\n";
    std::string out = runGame("3\n2\nЕва\n4\n5\n");
    CHECK(contains(out, "Ева"));
}

static void testChangeSpeciality() {
    std::cout << "\n--- Смена специальности ---\n";
    std::string out = runGame("3\n1\nФизика\n4\n5\n");
    CHECK(contains(out, "Физика"));
}

static void testChangeSubjects() {
    std::cout << "\n--- Смена количества предметов ---\n";
    std::string out = runGame("3\n3\n7\n4\n5\n");
    CHECK(contains(out, "(текущее: 7)"));
}

static void testNegativeSubjects() {
    std::cout << "\n--- Отрицательное количество предметов ---\n";
    std::string out = runGame("3\n3\n-5\n4\n5\n");
    CHECK(contains(out, "Некорректное значение"));
}

static void testPersistSettings() {
    std::cout << "\n--- Настройки сохраняются между заходами ---\n";
    std::string out = runGame("3\n2\nЕва\n4\n3\n4\n5\n");
    int count = 0;
    size_t pos = 0;
    while ((pos = out.find("Ева", pos)) != std::string::npos) {
        ++count;
        pos += 3;
    }
    CHECK(count >= 2);   // должно быть минимум 2 упоминания: после ввода и в подменю
}

int main() {
    std::cout << "========================================\n";
    std::cout << " Тесты: Лабораторная работа №1 (вар. 2)\n";
    std::cout << "========================================\n";

    testMainMenuExit();
    testStartGame();
    testLoadGame();
    testAbout();
    testInvalidChoice();
    testInvalidInput();
    testChangeName();
    testChangeSpeciality();
    testChangeSubjects();
    testNegativeSubjects();
    testPersistSettings();

    std::cout << "\n========================================\n";
    std::cout << " Пройдено: " << passed << "\n";
    std::cout << " Провалено: " << failed << "\n";
    std::cout << "========================================\n";

    return failed == 0 ? 0 : 1;
}
