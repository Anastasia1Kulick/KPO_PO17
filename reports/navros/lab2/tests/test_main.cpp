#define main original_main
#include "../src/Main.cpp"
#undef main

#include <cassert>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>

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


void resetGlobals() {
    difficulty = "Легко";
    fieldsize = 64;
    health = 3;
    while (!history.empty()) history.pop();
    settingsFile = "test_settings.txt";   
    std::remove(settingsFile.c_str());
}

void writeFile(const std::string& text) {
    std::ofstream out(settingsFile);
    out << text;
}

std::string readFile() {
    std::ifstream in(settingsFile);
    std::string all, line;
    while (std::getline(in, line)) all += line + "\n";
    return all;
}

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}


template <typename F>
std::string run(const std::string& input, F func) {
    redirectStart(input);
    func();
    std::string out = redirectOutput();
    redirectStop();
    return out;
}

// 1. Конвертеры
void test_converters() {
    assert(sizeConverter(64) == "8x8");
    assert(sizeConverter(100) == "10x10");
    assert(sizeConverter(144) == "12x12");
    assert(diffConverter(Easy) == "Легко");
    assert(diffConverter(Medium) == "Средне");
    assert(diffConverter(Hard) == "Сложно");
    assert(diffConverter(VeryHard) == "Очень Сложно");
    assert(diffConverter(99) == "Неизвестно");
    std::cout << "test_converters passed\n";
}

// 2. Простые экраны
void test_screens() {
    resetGlobals();
    difficulty = "Средне"; fieldsize = 100; health = 5;
    std::string out = run("\n", StartGame);
    assert(contains(out, "НАЧАЛО ИГРЫ"));
    assert(contains(out, "Сложность игры: Средне"));
    assert(contains(out, "Размер карты подземелья: 10x10"));
    assert(contains(out, "Количество очков здоровья: 5"));

    out = run("\n", LoadGame);
    assert(contains(out, "недоступна"));

    out = run("\n", aboutProgramm);
    assert(contains(out, "Приключение в подземелье"));
    assert(contains(out, "Наврось Глеб"));
    std::cout << "test_screens passed\n";
}

// 3. Ввод сложности
void test_DiffSubMenu() {
    resetGlobals();
    run("3\n", DiffSubMenu);
    assert(difficulty == "Сложно");
    run("4\n", DiffSubMenu);
    assert(difficulty == "Очень Сложно");

    run("-1\n", DiffSubMenu);                       
    assert(difficulty == "Очень Сложно");

    std::string out = run("0\n5\nabc\n2\n", DiffSubMenu);
    assert(contains(out, "Недопустимое значение"));
    assert(contains(out, "Ошибка ввода"));
    assert(difficulty == "Средне");
    std::cout << "test_DiffSubMenu passed\n";
}

// 4. Ввод размера
void test_SizeSubMenu() {
    resetGlobals();
    run("10\n", SizeSubMenu);
    assert(fieldsize == 100);
    run("5\n", SizeSubMenu);
    assert(fieldsize == 25);
    run("20\n", SizeSubMenu);
    assert(fieldsize == 400);

    run("-1\n", SizeSubMenu);
    assert(fieldsize == 400);

    std::string out = run("4\n21\nabc\n7\n", SizeSubMenu);
    assert(contains(out, "Недопустимое значение"));
    assert(contains(out, "Ошибка ввода"));
    assert(fieldsize == 49);
    std::cout << "test_SizeSubMenu passed\n";
}

// 5. Ввод здоровья
void test_HealthSubMenu() {
    resetGlobals();
    run("1\n", HealthSubMenu);
    assert(health == 1);
    run("10\n", HealthSubMenu);
    assert(health == 10);

    run("-1\n", HealthSubMenu);
    assert(health == 10);

    std::string out = run("0\n11\nabc\n4\n", HealthSubMenu);
    assert(contains(out, "Недопустимое значение"));
    assert(contains(out, "Ошибка ввода"));
    assert(health == 4);
    std::cout << "test_HealthSubMenu passed\n";
}

// 6. Последовательный ввод всех настроек
void test_AllSettings() {
    resetGlobals();
    std::string out = run("3\n12\n8\n", AllSettings);
    assert(difficulty == "Сложно");
    assert(fieldsize == 144);
    assert(health == 8);
    assert(out.find("Шаг 1 из 3") < out.find("Шаг 2 из 3"));
    assert(out.find("Шаг 2 из 3") < out.find("Шаг 3 из 3"));

    resetGlobals();
    run("-1\n-1\n-1\n", AllSettings);               
    assert(difficulty == "Легко" && fieldsize == 64 && health == 3);
    std::cout << "test_AllSettings passed\n";
}

// 7. Меню настроек
void test_SettingsMenu() {
    resetGlobals();
    std::string out = run("6\n", SettingsMenu);
    assert(contains(out, "НАСТРОЙКИ"));

    run("1\n3\n6\n", SettingsMenu);
    assert(difficulty == "Сложно");
    run("2\n12\n6\n", SettingsMenu);
    assert(fieldsize == 144);
    run("3\n8\n6\n", SettingsMenu);
    assert(health == 8);

    resetGlobals();
    run("4\n4\n10\n6\n6\n", SettingsMenu);          
    assert(difficulty == "Очень Сложно" && fieldsize == 100 && health == 6);

    out = run("99\nabc\n6\n", SettingsMenu);
    assert(contains(out, "Неверный пункт меню"));
    assert(contains(out, "Ошибка ввода"));
    std::cout << "test_SettingsMenu passed\n";
}

// 8. commitSettings: запись в стек и файл
void test_commitSettings() {
    resetGlobals();
    commitSettings();
    commitSettings();                              
    assert(history.size() == 1);
    assert(readFile() == "64 3 Легко\n");

    fieldsize = 100; health = 5; difficulty = "Очень Сложно";
    commitSettings();
    assert(history.size() == 2);
    assert(history.top().size == 100);
    assert(history.top().difficulty == "Очень Сложно");
    assert(readFile() == "64 3 Легко\n100 5 Очень Сложно\n");   
    std::cout << "test_commitSettings passed\n";
}

// 9. Чтение файла в стек
void test_loadHistory() {
    resetGlobals();
    loadHistory();                                  
    assert(history.empty());

    writeFile("");                                  
    loadHistory();
    assert(history.empty());

    writeFile(
        "64 3 Легко\n"         
        "abc\n"                
        "\n"                    
        "100 5\n"               
        "100 5 Неизвестно\n"    
        "7 3 Легко\n"           
        "144 99 Легко\n"       
        "144 5 Очень Сложно\n" 
        "400 10 Сложно\n"    
    );
    loadHistory();
    assert(history.size() == 3);
    assert(history.top().size == 400 && history.top().health == 10);
    history.pop();
    assert(history.top().size == 144);
    assert(history.top().difficulty == "Очень Сложно");
    history.pop();
    assert(history.top().size == 64);
    std::cout << "test_loadHistory passed\n";
}

// 10. Возврат предыдущих значений
void test_RestorePreviousSize() {
    resetGlobals();
    std::string out = run("", RestorePreviousSize); 
    assert(contains(out, "Предыдущие значения не найдены"));

    commitSettings();                            
    out = run("", RestorePreviousSize);          
    assert(contains(out, "Предыдущие значения не найдены"));
    assert(history.size() == 1);

    fieldsize = 100; health = 5; difficulty = "Сложно";
    commitSettings();
    fieldsize = 144; health = 8; difficulty = "Средне";
    commitSettings();

    run("", RestorePreviousSize);
    assert(fieldsize == 100 && health == 5 && difficulty == "Сложно");
    assert(history.size() == 2);
    assert(readFile() == "64 3 Легко\n100 5 Сложно\n");  

    run("", RestorePreviousSize);
    assert(fieldsize == 64 && history.size() == 1);
    assert(readFile() == "64 3 Легко\n");
    std::cout << "test_RestorePreviousSize passed\n";
}

// 11. История через меню настроек
void test_SettingsMenu_history() {
    resetGlobals();
    commitSettings();
    run("2\n10\n3\n7\n6\n", SettingsMenu);        
    assert(history.size() == 3);
    assert(readFile() == "64 3 Легко\n100 3 Легко\n100 7 Легко\n");

    run("5\n6\n", SettingsMenu);                  
    assert(history.size() == 2);
    assert(fieldsize == 100 && health == 3);

    resetGlobals();
    commitSettings();
    run("1\n-1\n2\n-1\n3\n-1\n6\n", SettingsMenu);  
    assert(history.size() == 1);

    run("4\n3\n12\n8\n6\n", SettingsMenu);         
    assert(history.size() == 2);
    std::cout << "test_SettingsMenu_history passed\n";
}

// 12. Главное меню и сохранение между запусками
void test_main() {
    resetGlobals();
    redirectStart("5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0);
    assert(contains(out, "Завершение работы"));
    assert(readFile() == "64 3 Легко\n");         

    out = run("abc\n99\n5\n", [] { original_main(); });
    assert(contains(out, "Ошибка ввода"));
    assert(contains(out, "Неверный пункт меню"));

    resetGlobals();                                
    writeFile("64 3 Легко\n144 8 Сложно\n");
    run("5\n", [] { original_main(); });
    assert(fieldsize == 144 && health == 8 && difficulty == "Сложно");
    assert(history.size() == 2);

    resetGlobals();                                
    run("3\n2\n10\n6\n5\n", [] { original_main(); });
    assert(readFile() == "64 3 Легко\n100 3 Легко\n");
    fieldsize = 64; health = 3; difficulty = "Легко";
    while (!history.empty()) history.pop();
    run("3\n5\n6\n5\n", [] { original_main(); });   
    assert(fieldsize == 64);
    assert(readFile() == "64 3 Легко\n");
    std::cout << "test_main passed\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    test_converters();
    test_screens();
    test_DiffSubMenu();
    test_SizeSubMenu();
    test_HealthSubMenu();
    test_AllSettings();
    test_SettingsMenu();
    test_commitSettings();
    test_loadHistory();
    test_RestorePreviousSize();
    test_SettingsMenu_history();
    test_main();

    std::remove(settingsFile.c_str());       

    std::cout << "\nAll tests passed!\n";
    return 0;
}
