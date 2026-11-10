#define main original_main
#include "../src/Main.cpp"
#undef main

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>

static std::istringstream g_in;
static std::ostringstream g_out;
static std::streambuf* g_oldCin = nullptr;
static std::streambuf* g_oldCout = nullptr;

void redirectStart(const std::string& input) {
    g_in.str(input);
    g_in.clear();
    g_out.str("");
    g_out.clear();
    g_oldCin = std::cin.rdbuf(g_in.rdbuf());
    g_oldCout = std::cout.rdbuf(g_out.rdbuf());
}

void redirectStop() {
    std::cin.rdbuf(g_oldCin);
    std::cout.rdbuf(g_oldCout);
}

std::string redirectOutput() {
    return g_out.str();
}

// Возвращает глобальные настройки к значениям по умолчанию перед каждым тестом
void resetGlobals() {
    difficulty = "Легко";
    fieldsize = 64;
    health = 3;
}

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

// ---------- Конвертеры ----------

void test_sizeConverter() {
    assert(sizeConverter(64) == "8x8");
    assert(sizeConverter(100) == "10x10");
    assert(sizeConverter(144) == "12x12");
    assert(sizeConverter(1) == "1x1");
    std::cout << "test_sizeConverter passed\n";
}

void test_diffConverter() {
    assert(diffConverter(Easy) == "Легко");
    assert(diffConverter(Medium) == "Средне");
    assert(diffConverter(Hard) == "Сложно");
    assert(diffConverter(VeryHard) == "Очень Сложно");
    assert(diffConverter(0) == "Неизвестно");
    assert(diffConverter(99) == "Неизвестно");
    std::cout << "test_diffConverter passed\n";
}

// ---------- Простые экраны ----------

void test_StartGame() {
    resetGlobals();
    difficulty = "Средне";
    fieldsize = 100;
    health = 5;
    redirectStart("\n");
    StartGame();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "НАЧАЛО ИГРЫ"));
    assert(contains(out, "Сложность игры: Средне"));
    assert(contains(out, "Размер карты подземелья: 10x10"));
    assert(contains(out, "Количество очков здоровья: 5"));
    std::cout << "test_StartGame passed\n";
}

void test_LoadGame() {
    redirectStart("\n");
    LoadGame();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "ЗАГРУЗКА ИГРЫ"));
    assert(contains(out, "недоступна"));
    std::cout << "test_LoadGame passed\n";
}

void test_aboutProgramm() {
    redirectStart("\n");
    aboutProgramm();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Приключение в подземелье"));
    assert(contains(out, "Версия игры: 1"));
    assert(contains(out, "Наврось Глеб"));
    std::cout << "test_aboutProgramm passed\n";
}

// ---------- Подменю сложности (ввод с клавиатуры) ----------

void test_DiffSubMenu_all_levels() {
    resetGlobals();

    redirectStart("2\n");
    DiffSubMenu();
    redirectStop();
    assert(difficulty == "Средне");

    redirectStart("3\n");
    DiffSubMenu();
    redirectStop();
    assert(difficulty == "Сложно");

    redirectStart("4\n");
    DiffSubMenu();
    redirectStop();
    assert(difficulty == "Очень Сложно");

    redirectStart("1\n");
    DiffSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(difficulty == "Легко");
    assert(contains(out, "Новый уровень сложности - Легко"));
    std::cout << "test_DiffSubMenu_all_levels passed\n";
}

void test_DiffSubMenu_exit_keeps_value() {
    resetGlobals();
    difficulty = "Сложно";
    redirectStart("-1\n");
    DiffSubMenu();
    redirectStop();
    assert(difficulty == "Сложно");
    std::cout << "test_DiffSubMenu_exit_keeps_value passed\n";
}

void test_DiffSubMenu_out_of_range_then_valid() {
    resetGlobals();
    redirectStart("0\n5\n-7\n3\n");
    DiffSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Недопустимое значение"));
    assert(difficulty == "Сложно");
    std::cout << "test_DiffSubMenu_out_of_range_then_valid passed\n";
}

void test_DiffSubMenu_invalid_text_then_valid() {
    resetGlobals();
    redirectStart("abc\n2\n");
    DiffSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Ошибка ввода"));
    assert(difficulty == "Средне");
    std::cout << "test_DiffSubMenu_invalid_text_then_valid passed\n";
}

// ---------- Подменю размера (ввод с клавиатуры) ----------

void test_SizeSubMenu_valid() {
    resetGlobals();
    redirectStart("10\n");
    SizeSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(fieldsize == 100);
    assert(contains(out, "Новый размер подземелья - 10x10"));
    std::cout << "test_SizeSubMenu_valid passed\n";
}

void test_SizeSubMenu_boundaries() {
    resetGlobals();

    redirectStart("5\n");
    SizeSubMenu();
    redirectStop();
    assert(fieldsize == 25);

    redirectStart("20\n");
    SizeSubMenu();
    redirectStop();
    assert(fieldsize == 400);
    std::cout << "test_SizeSubMenu_boundaries passed\n";
}

void test_SizeSubMenu_exit_keeps_value() {
    resetGlobals();
    fieldsize = 144;
    redirectStart("-1\n");
    SizeSubMenu();
    redirectStop();
    assert(fieldsize == 144);
    std::cout << "test_SizeSubMenu_exit_keeps_value passed\n";
}

void test_SizeSubMenu_out_of_range_then_valid() {
    resetGlobals();
    redirectStart("4\n21\n7\n");
    SizeSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Недопустимое значение"));
    assert(fieldsize == 49);
    std::cout << "test_SizeSubMenu_out_of_range_then_valid passed\n";
}

void test_SizeSubMenu_invalid_text_then_valid() {
    resetGlobals();
    redirectStart("abc\n8\n");
    SizeSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Ошибка ввода"));
    assert(fieldsize == 64);
    std::cout << "test_SizeSubMenu_invalid_text_then_valid passed\n";
}

// ---------- Подменю здоровья (ввод с клавиатуры) ----------

void test_HealthSubMenu_valid() {
    resetGlobals();
    redirectStart("7\n");
    HealthSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(health == 7);
    assert(contains(out, "Количество очков здоровья изменено на 7"));
    std::cout << "test_HealthSubMenu_valid passed\n";
}

void test_HealthSubMenu_boundaries() {
    resetGlobals();

    redirectStart("1\n");
    HealthSubMenu();
    redirectStop();
    assert(health == 1);

    redirectStart("10\n");
    HealthSubMenu();
    redirectStop();
    assert(health == 10);
    std::cout << "test_HealthSubMenu_boundaries passed\n";
}

void test_HealthSubMenu_exit_keeps_value() {
    resetGlobals();
    health = 6;
    redirectStart("-1\n");
    HealthSubMenu();
    redirectStop();
    assert(health == 6);
    std::cout << "test_HealthSubMenu_exit_keeps_value passed\n";
}

void test_HealthSubMenu_out_of_range_then_valid() {
    resetGlobals();
    redirectStart("0\n11\n-5\n4\n");
    HealthSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Недопустимое значение"));
    assert(health == 4);
    std::cout << "test_HealthSubMenu_out_of_range_then_valid passed\n";
}

void test_HealthSubMenu_invalid_text_then_valid() {
    resetGlobals();
    redirectStart("abc\n9\n");
    HealthSubMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Ошибка ввода"));
    assert(health == 9);
    std::cout << "test_HealthSubMenu_invalid_text_then_valid passed\n";
}

// ---------- Последовательный ввод всех настроек ----------

void test_AllSettings_sequence() {
    resetGlobals();
    // сложность 3, размер 12, здоровье 8
    redirectStart("3\n12\n8\n");
    AllSettings();
    std::string out = redirectOutput();
    redirectStop();
    assert(difficulty == "Сложно");
    assert(fieldsize == 144);
    assert(health == 8);
    assert(contains(out, "Настройка завершена"));
    std::cout << "test_AllSettings_sequence passed\n";
}

void test_AllSettings_order_of_prompts() {
    resetGlobals();
    redirectStart("2\n6\n5\n");
    AllSettings();
    std::string out = redirectOutput();
    redirectStop();
    size_t step1 = out.find("Шаг 1 из 3");
    size_t step2 = out.find("Шаг 2 из 3");
    size_t step3 = out.find("Шаг 3 из 3");
    assert(step1 != std::string::npos);
    assert(step2 != std::string::npos);
    assert(step3 != std::string::npos);
    assert(step1 < step2 && step2 < step3);
    std::cout << "test_AllSettings_order_of_prompts passed\n";
}

void test_AllSettings_skip_all() {
    resetGlobals();
    redirectStart("-1\n-1\n-1\n");
    AllSettings();
    redirectStop();
    assert(difficulty == "Легко");
    assert(fieldsize == 64);
    assert(health == 3);
    std::cout << "test_AllSettings_skip_all passed\n";
}

void test_AllSettings_invalid_values_retry() {
    resetGlobals();
    redirectStart("9\n1\n100\nabc\n9\n0\n2\n");
    AllSettings();
    redirectStop();
    assert(difficulty == "Легко");
    assert(fieldsize == 81);
    assert(health == 2);
    std::cout << "test_AllSettings_invalid_values_retry passed\n";
}

// ---------- Меню настроек ----------

void test_SettingsMenu_back() {
    resetGlobals();
    redirectStart("5\n");
    SettingsMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "НАСТРОЙКИ"));
    assert(contains(out, "Выход из настроек"));
    std::cout << "test_SettingsMenu_back passed\n";
}

void test_SettingsMenu_change_difficulty() {
    resetGlobals();
    redirectStart("1\n3\n5\n");
    SettingsMenu();
    redirectStop();
    assert(difficulty == "Сложно");
    std::cout << "test_SettingsMenu_change_difficulty passed\n";
}

void test_SettingsMenu_change_size() {
    resetGlobals();
    redirectStart("2\n12\n5\n");
    SettingsMenu();
    redirectStop();
    assert(fieldsize == 144);
    std::cout << "test_SettingsMenu_change_size passed\n";
}

void test_SettingsMenu_change_health() {
    resetGlobals();
    redirectStart("3\n8\n5\n");
    SettingsMenu();
    redirectStop();
    assert(health == 8);
    std::cout << "test_SettingsMenu_change_health passed\n";
}

void test_SettingsMenu_all_settings() {
    resetGlobals();
    // пункт 4: сложность 4, размер 10, здоровье 6; затем выход
    redirectStart("4\n4\n10\n6\n5\n");
    SettingsMenu();
    redirectStop();
    assert(difficulty == "Очень Сложно");
    assert(fieldsize == 100);
    assert(health == 6);
    std::cout << "test_SettingsMenu_all_settings passed\n";
}

void test_SettingsMenu_invalid_input() {
    resetGlobals();
    redirectStart("99\nabc\n5\n");
    SettingsMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "Неверный пункт меню"));
    assert(contains(out, "Ошибка ввода"));
    std::cout << "test_SettingsMenu_invalid_input passed\n";
}

// ---------- Главное меню ----------

void test_ShowMainMenu() {
    redirectStart("");
    ShowMainMenu();
    std::string out = redirectOutput();
    redirectStop();
    assert(contains(out, "ПРИКЛЮЧЕНИЕ В ПОДЗЕМЕЛЬЕ"));
    assert(contains(out, "Начало игры"));
    assert(contains(out, "Выход"));
    std::cout << "test_ShowMainMenu passed\n";
}

void test_main_exit() {
    resetGlobals();
    redirectStart("5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0);
    assert(contains(out, "Завершение работы"));
    std::cout << "test_main_exit passed\n";
}

void test_main_invalid_input() {
    resetGlobals();
    redirectStart("abc\n99\n5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0);
    assert(contains(out, "Ошибка ввода"));
    assert(contains(out, "Неверный пункт меню"));
    std::cout << "test_main_invalid_input passed\n";
}

void test_main_start_game_and_about() {
    resetGlobals();
    // 1 -> игра (Enter), 2 -> загрузка (Enter), 4 -> о программе (Enter), 5 -> выход
    redirectStart("1\n\n2\n\n4\n\n5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0);
    assert(contains(out, "НАЧАЛО ИГРЫ"));
    assert(contains(out, "ЗАГРУЗКА ИГРЫ"));
    assert(contains(out, "О ПРОГРАММЕ"));
    std::cout << "test_main_start_game_and_about passed\n";
}

void test_main_settings_then_start() {
    resetGlobals();
    // Настройки: здоровье = 9, назад; затем старт игры; выход
    redirectStart("3\n3\n9\n5\n1\n\n5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0);
    assert(health == 9);
    assert(contains(out, "Количество очков здоровья: 9"));
    std::cout << "test_main_settings_then_start passed\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    test_sizeConverter();
    test_diffConverter();

    test_StartGame();
    test_LoadGame();
    test_aboutProgramm();

    test_DiffSubMenu_all_levels();
    test_DiffSubMenu_exit_keeps_value();
    test_DiffSubMenu_out_of_range_then_valid();
    test_DiffSubMenu_invalid_text_then_valid();

    test_SizeSubMenu_valid();
    test_SizeSubMenu_boundaries();
    test_SizeSubMenu_exit_keeps_value();
    test_SizeSubMenu_out_of_range_then_valid();
    test_SizeSubMenu_invalid_text_then_valid();

    test_HealthSubMenu_valid();
    test_HealthSubMenu_boundaries();
    test_HealthSubMenu_exit_keeps_value();
    test_HealthSubMenu_out_of_range_then_valid();
    test_HealthSubMenu_invalid_text_then_valid();

    test_AllSettings_sequence();
    test_AllSettings_order_of_prompts();
    test_AllSettings_skip_all();
    test_AllSettings_invalid_values_retry();

    test_SettingsMenu_back();
    test_SettingsMenu_change_difficulty();
    test_SettingsMenu_change_size();
    test_SettingsMenu_change_health();
    test_SettingsMenu_all_settings();
    test_SettingsMenu_invalid_input();

    test_ShowMainMenu();
    test_main_exit();
    test_main_invalid_input();
    test_main_start_game_and_about();
    test_main_settings_then_start();

    std::cout << "\nAll tests passed!\n";
    return 0;
}