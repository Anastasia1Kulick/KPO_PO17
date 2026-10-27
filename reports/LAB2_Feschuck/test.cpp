#define main original_main
#include "КПО2АР.cpp"
#undef main

#include <sstream>
#include <cstdio>

static int passed = 0, failed = 0;

static void check(bool ok, const std::string& msg) {
    if (ok) { ++passed; return; }
    ++failed;
    std::cerr << "  [FAIL] " << msg << "\n";
}

static void rmAll() {
    const char* fs[] = {"settings.txt", "city_size_values.txt",
                        "budget_values.txt", "population_values.txt"};
    for (const char* f : fs) std::remove(f);
}

static void write(const char* f, const char* s) {
    std::ofstream(f, std::ios::trunc) << s;
}

static std::string slurp(const char* f) {
    std::ifstream i(f);
    std::stringstream ss;
    ss << i.rdbuf();
    return ss.str();
}

// Серия переключений: ожидаемые значения по шагам.
static void seq(const std::string& name,
                std::function<void(SettingsManager&)> act,
                std::function<int(const GameSettings&)> get,
                std::vector<int> expected) {
    std::cout << "[TEST] " << name << "\n";
    rmAll();
    SettingsManager m;
    for (size_t i = 0; i < expected.size(); ++i) {
        act(m);
        int got = get(m.settings);
        check(got == expected[i],
              "шаг " + std::to_string(i + 1) +
              ": ожидалось " + std::to_string(expected[i]) +
              ", получено " + std::to_string(got));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    using G = GameSettings;

    seq("citySize: 10→20→30→10",
        [](SettingsManager& m){ m.switchCitySize(); },
        [](const G& s){ return s.citySize; },
        {20, 30, 10});

    seq("budget: 1000→5000→10000→1000",
        [](SettingsManager& m){ m.switchBudget(); },
        [](const G& s){ return s.budget; },
        {5000, 10000, 1000});

    seq("population: 100→500→1000→100",
        [](SettingsManager& m){ m.switchPopulation(); },
        [](const G& s){ return s.population; },
        {500, 1000, 100});

    std::cout << "[TEST] значения по умолчанию\n";
    rmAll();
    { SettingsManager m;
      check(m.settings.citySize == 10,   "citySize = 10");
      check(m.settings.budget == 1000,   "budget = 1000");
      check(m.settings.population == 100, "population = 100"); }

    std::cout << "[TEST] персистентность между экземплярами\n";
    rmAll();
    { int a, b, c;
      { SettingsManager m;
        m.switchCitySize(); m.switchBudget(); m.switchPopulation();
        a = m.settings.citySize;
        b = m.settings.budget;
        c = m.settings.population; }
      SettingsManager m2;
      check(m2.settings.citySize == a,   "citySize восстановлен");
      check(m2.settings.budget == b,     "budget восстановлен");
      check(m2.settings.population == c, "population восстановлен"); }

    std::cout << "[TEST] неизвестное значение → первое из файла\n";
    rmAll();
    write("city_size_values.txt", "10\n20\n30\n");
    write("settings.txt", "999\n1000\n100\n");
    { SettingsManager m;
      m.switchCitySize();
      check(m.settings.citySize == 10, "999 → 10"); }

    std::cout << "[TEST] чужой файл значений не перезаписывается\n";
    rmAll();
    write("budget_values.txt", "7\n42\n");
    { SettingsManager m;
      m.switchBudget();
      check(m.settings.budget == 7, "1000 → 7");
      m.switchBudget();
      check(m.settings.budget == 42, "7 → 42");
      m.switchBudget();
      check(m.settings.budget == 7, "42 → 7 (цикл)");
      check(slurp("budget_values.txt").find("42") != std::string::npos,
            "файл сохранён"); }

    std::cout << "[TEST] пустой файл значений\n";
    rmAll();
    write("city_size_values.txt", "");
    write("settings.txt", "10\n1000\n100\n");
    { SettingsManager m;
      m.switchCitySize();
      check(m.settings.citySize == 10, "значение не изменилось"); }

    std::cout << "[TEST] повреждённый settings.txt\n";
    rmAll();
    write("settings.txt", "мусор\n");
    { SettingsManager m;
      check(m.settings.citySize == 10,   "citySize = 10");
      check(m.settings.budget == 1000,   "budget = 1000"); }

    std::cout << "[TEST] поля независимы\n";
    rmAll();
    { SettingsManager m;
      int cs = m.settings.citySize;
      int pop = m.settings.population;
      m.switchBudget(); m.switchBudget();
      check(m.settings.citySize == cs,   "citySize не тронут");
      check(m.settings.population == pop, "population не тронут"); }

    std::cout << "\nПройдено: " << passed
              << ", провалено: " << failed << "\n";
    return failed ? 1 : 0;
}
