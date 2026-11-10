#define UNIT_TEST
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

struct GameSettings;
class SettingsManager;
class Building;
class Town;
class Mayor;
void startGame(const GameSettings&);

int pass = 0, fail = 0;

void check(const std::string& name,
           const std::string& hay,
           const std::string& needle,
           bool must = true) {
    bool has = hay.find(needle) != std::string::npos;
    if (has == must) {
        std::cout << "[PASS] " << name << "\n";
        ++pass;
    } else {
        std::cout << "[FAIL] " << name
                  << " (искали \"" << needle << "\")\n";
        ++fail;
    }
}

// Запуск startGame с подменой ввода/вывода
std::string runGame(const std::string& input,
                    const GameSettings& cfg) {
    std::istringstream in(input);
    std::ostringstream out;
    auto* oi = std::cin.rdbuf(in.rdbuf());
    auto* oo = std::cout.rdbuf(out.rdbuf());
    startGame(cfg);
    std::cin.rdbuf(oi);
    std::cout.rdbuf(oo);
    std::cin.clear();
    return out.str();
}

// Сценарий: N недель «пропустить»
std::string skipWeeks(int n) {
    std::string s;
    for (int i = 0; i < n; ++i) s += "3\n\n";
    return s;
}

int main() {
    // === Классы ===
    {
        Town t(10, 1000, 100);
        Building h("Жилой дом", 200, 20, 30, 5);
        t.build(h);
        check("Build: бюджет",   std::to_string(t.budget), "800");
        check("Build: население", std::to_string(t.population), "130");
        check("Build: зданий",   std::to_string((int)t.buildings.size()), "1");
        check("EndWeek: доход",  std::to_string(t.endWeek()), "70");
    }
    {
        Town t(10, 100, 100);
        Building f("Фабрика", 500, 120, 10, -5);
        std::ostringstream tmp;
        auto* o = std::cout.rdbuf(tmp.rdbuf());
        bool ok = t.build(f);
        std::cout.rdbuf(o);
        check("NoMoney: отказ",  ok ? "1" : "0", "0");
        check("NoMoney: бюджет", std::to_string(t.budget), "100");
    }
    {
        Town t(1, 10000, 100);
        Building p("Парк", 150, 5, 0, 15);
        t.build(p);
        std::ostringstream tmp;
        auto* o = std::cout.rdbuf(tmp.rdbuf());
        bool ok = t.build(p);
        std::cout.rdbuf(o);
        check("Limit: отказ", ok ? "1" : "0", "0");
    }
    {
        Town t(10, 10000, 100);
        Building th("Театр", 350, 40, 0, 18);
        for (int i = 0; i < 10; ++i) t.build(th);
        check("Happiness: <=100", t.happiness <= 100 ? "1" : "0", "1");
    }
    {
        Mayor m;
        check("Mayor: 5", std::to_string(m.actions), "5");
        for (int i = 0; i < 5; ++i) m.use();
        check("Mayor: has=0", m.has() ? "1" : "0", "0");
        m.reset();
        check("Mayor: reset", std::to_string(m.actions), "5");
    }

    // === startGame ===
    GameSettings cfg;
    cfg.citySize = 10;
    cfg.budget = 1000;
    cfg.population = 100;

    // Полное прохождение
    std::string out = runGame(skipWeeks(6), cfg);
    check("Game: заголовок", out, "НАЧАЛО ИГРЫ");
    check("Game: неделя 6",  out, "НЕДЕЛЯ 6 / 6");
    check("Game: итоги",     out, "ИТОГИ НЕДЕЛИ");
    check("Game: финал",     out, "ИГРА ЗАВЕРШЕНА");
    check("Game: счёт",      out, "Итоговый счёт");

    // Постройка дома
    std::string b = "1\n1\n\n" + skipWeeks(5);
    out = runGame(b, cfg);
    check("Game: построено",     out, "Построено: Жилой дом");
    check("Game: население 130", out, "Население: 130");

    // Мероприятие
    std::string e = "2\n\n" + skipWeeks(5);
    out = runGame(e, cfg);
    check("Game: мероприятие", out, "Мероприятие проведено");
    check("Game: счастье 60",  out, "Счастье: 60");

    // Плохой ввод
    std::string bad = "abc\n1\n1\n\n" + skipWeeks(5);
    out = runGame(bad, cfg);
    check("Game: ошибка ввода", out, "Ошибка ввода");
    check("Game: дошло до финала", out, "ИГРА ЗАВЕРШЕНА");

    // === SettingsManager ===
    std::remove("settings.txt");
    std::remove("city_size_values.txt");
    {
        SettingsManager sm;
        check("Settings: size",
              std::to_string(sm.get().citySize), "10");
        check("Settings: budget",
              std::to_string(sm.get().budget), "1000");
        sm.nextSize();
        check("Settings: 10->20",
              std::to_string(sm.get().citySize), "20");
        sm.nextSize();
        sm.nextSize();
        check("Settings: цикл",
              std::to_string(sm.get().citySize), "10");
    }

    std::cout << "\nПройдено: " << pass
              << " | Провалено: " << fail << "\n";
    return fail ? 1 : 0;
}
