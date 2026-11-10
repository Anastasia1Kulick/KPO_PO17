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

// Возвращает всё к значениям по умолчанию перед каждым тестом
void resetGlobals() {
    difficulty = "Легко";
    fieldsize = 64;
    health = 3;
    while (!history.empty()) history.pop();
    settingsFile = "test_settings.txt";   // тесты не трогают настоящий settings.txt
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

int countOf(const std::string& text, const std::string& part) {
    int n = 0;
    for (size_t pos = text.find(part); pos != std::string::npos; pos = text.find(part, pos + 1)) n++;
    return n;
}

// Запускает функцию с заданным вводом и возвращает вывод
template <typename F>
std::string run(const std::string& input, F func) {
    redirectStart(input);
    func();
    std::string out = redirectOutput();
    redirectStop();
    return out;
}

// 1. Конвертеры и простые экраны
void test_converters_and_screens() {
    assert(sizeConverter(64) == "8x8");
    assert(sizeConverter(144) == "12x12");
    assert(diffConverter(Easy) == "Легко");
    assert(diffConverter(VeryHard) == "Очень Сложно");
    assert(diffConverter(99) == "Неизвестно");

    resetGlobals();
    difficulty = "Средне"; fieldsize = 100; health = 5;
    std::string out = run("\n", StartGame);          // ввода нет, игра сразу завершается
    assert(contains(out, "НАЧАЛО ИГРЫ"));
    assert(contains(out, "Сложность игры: Средне"));
    assert(contains(out, "Размер карты подземелья: 10x10"));
    assert(contains(out, "Количество очков здоровья: 5"));

    out = run("\n", LoadGame);
    assert(contains(out, "недоступна"));
    out = run("\n", aboutProgramm);
    assert(contains(out, "Наврось Глеб"));
    std::cout << "test_converters_and_screens passed\n";
}

// 2. Ввод настроек с клавиатуры
void test_setting_inputs() {
    resetGlobals();
    run("3\n", DiffSubMenu);
    assert(difficulty == "Сложно");
    std::string out = run("0\n5\nabc\n2\n", DiffSubMenu);
    assert(contains(out, "Недопустимое значение") && contains(out, "Ошибка ввода"));
    assert(difficulty == "Средне");

    run("10\n", SizeSubMenu);
    assert(fieldsize == 100);
    run("-1\n", SizeSubMenu);                        // выход не меняет значение
    assert(fieldsize == 100);
    out = run("4\n21\nabc\n20\n", SizeSubMenu);
    assert(contains(out, "Недопустимое значение") && contains(out, "Ошибка ввода"));
    assert(fieldsize == 400);

    run("1\n", HealthSubMenu);
    assert(health == 1);
    out = run("0\n11\nabc\n10\n", HealthSubMenu);
    assert(contains(out, "Недопустимое значение") && contains(out, "Ошибка ввода"));
    assert(health == 10);

    resetGlobals();
    out = run("3\n12\n8\n", AllSettings);
    assert(difficulty == "Сложно" && fieldsize == 144 && health == 8);
    assert(out.find("Шаг 1 из 3") < out.find("Шаг 2 из 3"));
    assert(out.find("Шаг 2 из 3") < out.find("Шаг 3 из 3"));
    std::cout << "test_setting_inputs passed\n";
}

// 3. Меню настроек и история через него
void test_SettingsMenu() {
    resetGlobals();
    std::string out = run("6\n", SettingsMenu);
    assert(contains(out, "НАСТРОЙКИ"));
    out = run("99\nabc\n6\n", SettingsMenu);
    assert(contains(out, "Неверный пункт меню") && contains(out, "Ошибка ввода"));

    resetGlobals();
    commitSettings();
    run("2\n10\n3\n7\n6\n", SettingsMenu);           // размер, затем здоровье
    assert(history.size() == 3);
    assert(readFile() == "64 3 Легко\n100 3 Легко\n100 7 Легко\n");
    run("5\n6\n", SettingsMenu);                     // пункт 5: вернуть предыдущий
    assert(history.size() == 2 && fieldsize == 100 && health == 3);

    resetGlobals();
    commitSettings();
    run("1\n-1\n2\n-1\n3\n-1\n6\n", SettingsMenu);   // отмена ввода не создаёт записей
    assert(history.size() == 1);
    run("4\n3\n12\n8\n6\n", SettingsMenu);           // три шага дают одну запись
    assert(history.size() == 2 && history.top().size == 144);
    std::cout << "test_SettingsMenu passed\n";
}

// 4. История: запись в файл и чтение в стек
void test_history_file() {
    resetGlobals();
    commitSettings();
    commitSettings();                                // без изменений дубликата нет
    assert(history.size() == 1);
    fieldsize = 100; health = 5; difficulty = "Очень Сложно";
    commitSettings();
    assert(readFile() == "64 3 Легко\n100 5 Очень Сложно\n");   // от старого к новому

    resetGlobals();
    loadHistory();                                   // файла нет
    assert(history.empty());
    writeFile("");
    loadHistory();                                   // файл пустой
    assert(history.empty());

    writeFile(
        "64 3 Легко\n"          // нормальная
        "abc\n"                 // не числа
        "\n"                    // пустая
        "100 5\n"               // нет сложности
        "100 5 Неизвестно\n"    // неизвестная сложность
        "7 3 Легко\n"           // размер не квадрат
        "144 99 Легко\n"        // здоровье вне границ
        "144 5 Очень Сложно\n"  // нормальная, сложность из двух слов
        "400 10 Сложно\n"       // нормальная, границы включены
    );
    loadHistory();
    assert(history.size() == 3);
    assert(history.top().size == 400 && history.top().health == 10);
    history.pop();
    assert(history.top().size == 144 && history.top().difficulty == "Очень Сложно");
    std::cout << "test_history_file passed\n";
}

// 5. Возврат предыдущих значений
void test_RestorePreviousSize() {
    resetGlobals();
    std::string out = run("", RestorePreviousSize);  // стек пуст
    assert(contains(out, "Предыдущие значения не найдены"));
    commitSettings();
    out = run("", RestorePreviousSize);              // одна запись, откатываться некуда
    assert(contains(out, "Предыдущие значения не найдены") && history.size() == 1);

    fieldsize = 100; health = 5; difficulty = "Сложно";
    commitSettings();
    fieldsize = 144; health = 8; difficulty = "Средне";
    commitSettings();

    run("", RestorePreviousSize);
    assert(fieldsize == 100 && health == 5 && difficulty == "Сложно");
    assert(readFile() == "64 3 Легко\n100 5 Сложно\n");   // файл совпадает со стеком
    run("", RestorePreviousSize);
    assert(fieldsize == 64 && history.size() == 1);
    assert(readFile() == "64 3 Легко\n");
    std::cout << "test_RestorePreviousSize passed\n";
}

// 6. Главное меню и сохранение между запусками
void test_main() {
    resetGlobals();
    redirectStart("5\n");
    int code = original_main();
    std::string out = redirectOutput();
    redirectStop();
    assert(code == 0 && contains(out, "Завершение работы"));
    assert(readFile() == "64 3 Легко\n");            // при первом запуске создан файл по умолчанию

    out = run("abc\n99\n5\n", [] { original_main(); });
    assert(contains(out, "Ошибка ввода") && contains(out, "Неверный пункт меню"));

    resetGlobals();                                  // история читается при запуске
    writeFile("64 3 Легко\n144 8 Сложно\n");
    run("5\n", [] { original_main(); });
    assert(fieldsize == 144 && health == 8 && difficulty == "Сложно" && history.size() == 2);

    resetGlobals();                                  // история переживает перезапуск
    run("3\n2\n10\n6\n5\n", [] { original_main(); });
    assert(readFile() == "64 3 Легко\n100 3 Легко\n");
    fieldsize = 64;
    while (!history.empty()) history.pop();
    run("3\n5\n6\n5\n", [] { original_main(); });
    assert(fieldsize == 64 && readFile() == "64 3 Легко\n");
    std::cout << "test_main passed\n";
}

// 7. Weapon и Monster
void test_Weapon_and_Monster() {
    Weapon fists;
    assert(fists.getName() == "Кулаки" && fists.getDamage() == 2);
    Weapon sword("Меч", 7);
    assert(sword.getName() == "Меч" && sword.getDamage() == 7);

    Monster m("Орк", 5, 2, sword);
    assert(m.isAlive() && m.getHealth() == 5 && m.attack() == 2 && m.getName() == "Орк");
    m.takeDamage(3);
    assert(m.getHealth() == 2 && m.isAlive());
    m.takeDamage(100);
    assert(m.getHealth() == 0 && !m.isAlive());      // здоровье не уходит ниже нуля
    assert(m.getDrop().getName() == "Меч");

    assert(makeMonster(0).getName() == "Гоблин");
    assert(makeMonster(1).getName() == "Скелет");
    assert(makeMonster(2).getName() == "Тролль");
    assert(makeMonster(3).getName() == "Гоблин");    // виды идут по кругу
    assert(makeMonster(0).getDrop().getName() == "Ржавый кинжал");
    std::cout << "test_Weapon_and_Monster passed\n";
}

// 8. Character
void test_Character() {
    Character c(10);
    assert(c.getHealth() == 10 && c.isAlive());
    assert(c.getX() == 0 && c.getY() == 0 && c.getWins() == 0);
    assert(c.getWeaponName() == "Кулаки" && c.attack() == 2);

    c.takeDamage(4);
    assert(c.getHealth() == 6);
    c.takeDamage(100);
    assert(c.getHealth() == 0 && !c.isAlive());

    c.moveTo(3, 4);
    assert(c.getX() == 3 && c.getY() == 4);
    c.addWin();
    c.addWin();
    assert(c.getWins() == 2);

    assert(!c.equip(0));                             // инвентарь пуст
    c.pickUp(Weapon("Кинжал", 3));
    c.pickUp(Weapon("Меч", 5));
    assert(c.inventorySize() == 2);
    assert(!c.equip(-1) && !c.equip(2));             // неверные номера
    assert(c.getWeaponName() == "Кулаки");

    assert(c.equip(1));                              // надеваем меч, кулаки уходят в инвентарь
    assert(c.getWeaponName() == "Меч" && c.attack() == 5);
    assert(c.getInventory()[1].getName() == "Кулаки");
    assert(c.inventorySize() == 2);
    std::cout << "test_Character passed\n";
}

// 9. Dungeon: границы, посещённые клетки, отрисовка
void test_Dungeon() {
    Dungeon d(3);
    assert(d.inside(0, 0) && d.inside(2, 2));
    assert(!d.inside(-1, 0) && !d.inside(0, -1) && !d.inside(3, 0) && !d.inside(0, 3));
    assert(!d.isVisited(1, 1));
    d.visit(1, 1);
    assert(d.isVisited(1, 1) && !d.isVisited(0, 1));

    Dungeon map(3);
    Character c(10);
    map.visit(0, 0);
    c.moveTo(1, 0);
    std::string out = run("", [&] { map.draw(c); });
    assert(out == "x @ o \no o o \no o o \n");        // x - посещено, @ - персонаж, o - нет
    std::cout << "test_Dungeon passed\n";
}

// 10. Перемещение по карте
void test_movePlayer() {
    Dungeon d(3);
    Character c(10);
    assert(movePlayer(c, d, 'w') == Wall && c.getY() == 0);   // за границу нельзя
    assert(movePlayer(c, d, 'a') == Wall && c.getX() == 0);
    assert(movePlayer(c, d, 'x') == UnknownKey);
    assert(movePlayer(c, d, 'd') == Moved && c.getX() == 1);
    assert(movePlayer(c, d, 'D') == Moved && c.getX() == 2);  // заглавные тоже работают
    assert(movePlayer(c, d, 'd') == Wall && c.getX() == 2);
    assert(movePlayer(c, d, 's') == Moved && c.getY() == 1);
    assert(movePlayer(c, d, 'S') == Moved && c.getY() == 2);
    assert(movePlayer(c, d, 's') == Wall && c.getY() == 2);
    assert(movePlayer(c, d, 'W') == Moved && c.getY() == 1);
    assert(movePlayer(c, d, 'A') == Moved && c.getX() == 1);
    std::cout << "test_movePlayer passed\n";
}

// 11. Бой и добыча
void test_battle_and_loot() {
    Character p(10);
    Monster m = makeMonster(0);                      // Гоблин: здоровье 4, урон 1
    assert(battleRound(p, m, Attack, 0));            // кулаки наносят 2, гоблин отвечает 1
    assert(m.getHealth() == 2 && p.getHealth() == 9);
    assert(battleRound(p, m, Attack, 0));            // гоблин убит и не отвечает
    assert(!m.isAlive() && p.getHealth() == 9);

    Monster m2 = makeMonster(0);
    assert(!battleRound(p, m2, ChangeWeapon, 0));    // инвентарь пуст, ход не засчитан
    assert(!battleRound(p, m2, 9, 0));               // неизвестное действие
    assert(m2.getHealth() == 4 && p.getHealth() == 9);
    p.pickUp(Weapon("Кинжал", 3));
    assert(battleRound(p, m2, ChangeWeapon, 0));     // смена оружия тоже занимает ход
    assert(p.getWeaponName() == "Кинжал" && p.getHealth() == 8);

    // интерактивный бой: пустой инвентарь, ошибка ввода, неверное действие, затем победа
    Character p2(10);
    Monster m3 = makeMonster(0);
    bool won = false;
    std::string out = run("2\nabc\n9\n1\n1\n", [&] { won = Battle(p2, m3); });
    assert(won);
    assert(contains(out, "В инвентаре нет оружия") && contains(out, "Ошибка ввода") && contains(out, "Неверное действие"));

    // смена оружия в бою и поражение игрока
    Character p3(10);
    p3.pickUp(Weapon("Кинжал", 3));
    Monster m4 = makeMonster(0);
    out = run("2\n5\n2\n1\n1\n1\n", [&] { won = Battle(p3, m4); });
    assert(won && p3.getWeaponName() == "Кинжал");
    assert(contains(out, "Неверное действие"));
    Character weak(1);
    Monster m5 = makeMonster(0);
    run("1\n", [&] { won = Battle(weak, m5); });
    assert(!won && !weak.isAlive());

    // добыча: победа засчитывается, оружие падает в инвентарь, можно надеть сразу
    Character hero(10);
    Monster gob = makeMonster(0);
    run("2\n", [&] { LootMonster(hero, gob); });
    assert(hero.getWins() == 1 && hero.inventorySize() == 1 && hero.getWeaponName() == "Кулаки");
    run("abc\n7\n1\n", [&] { LootMonster(hero, gob); });
    assert(hero.getWins() == 2 && hero.getWeaponName() == "Ржавый кинжал");
    run("", [&] { LootMonster(hero, gob); });        // третья победа: предложения надеть нет
    assert(hero.getWins() == WIN_CONDITION && hero.inventorySize() == 3);
    std::cout << "test_battle_and_loot passed\n";
}

// 12. Полная игра: победа, поражение, стены, посещённые клетки
void test_StartGame() {
    resetGlobals();
    fieldsize = 25;
    health = 10;
    // три новых клетки подряд: гоблин, скелет, тролль; после первых двух побед надеваем трофей
    std::string out = run(
        "d\n1\n1\n1\n"            // гоблин: кулаки (2) бьют дважды, надеваем кинжал
        "d\n1\n1\n1\n"            // скелет: кинжал (3) бьёт дважды, надеваем меч
        "d\n1\n1\n1\n\n",         // тролль: меч (4) бьёт трижды, победа
        StartGame);
    assert(contains(out, "Гоблин") && contains(out, "Скелет") && contains(out, "Тролль"));
    assert(contains(out, "Ржавый кинжал") && contains(out, "Костяной меч"));
    assert(contains(out, "Поздравляем"));
    assert(contains(out, "побед: 2 из 3"));

    // стена, неизвестные команды, возврат на посещённую клетку без боя, выход по Q
    out = run("a\nx\nab\nd\n1\n1\n2\na\nq\n\n", StartGame);
    assert(contains(out, "Там стена"));
    assert(countOf(out, "Неизвестная команда") == 2);
    assert(countOf(out, "Вы встретили монстра") == 1);   // на старой клетке монстра нет
    assert(contains(out, "Игра прервана"));
    assert(contains(out, "x @ o"));                      // карта: посещено, персонаж, не посещено

    // поражение
    health = 1;
    out = run("d\n1\n\n", StartGame);
    assert(contains(out, "Вы погибли"));
    assert(!contains(out, "Поздравляем"));
    std::cout << "test_StartGame passed\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    test_converters_and_screens();
    test_setting_inputs();
    test_SettingsMenu();
    test_history_file();
    test_RestorePreviousSize();
    test_main();
    test_Weapon_and_Monster();
    test_Character();
    test_Dungeon();
    test_movePlayer();
    test_battle_and_loot();
    test_StartGame();

    std::remove(settingsFile.c_str());       // убираем временный файл тестов

    std::cout << "\nAll tests passed!\n";
    return 0;
}
