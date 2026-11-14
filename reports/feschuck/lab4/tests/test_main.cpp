#define UNIT_TEST
#include "../src/main.cpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

int passedCount = 0, failedCount = 0;

void assertContains(const std::string& testName,
                    const std::string& text,
                    const std::string& expected,
                    bool shouldContain = true) {
    bool found = text.find(expected) != std::string::npos;
    if (found == shouldContain) {
        std::cout << "[ОК] " << testName << "\n";
        ++passedCount;
    } else {
        std::cout << "[ПРОВАЛ] " << testName
                  << " (искали: \"" << expected << "\")\n";
        ++failedCount;
    }
}

// Эмуляция запуска игры с подменой потоков ввода/вывода
std::string simulateGame(const std::string& inputs, const GameConfig& config, SaveSlotManager& mgr) {
    std::istringstream input(inputs);
    std::ostringstream output;
    auto* oldIn = std::cin.rdbuf(input.rdbuf());
    auto* oldOut = std::cout.rdbuf(output.rdbuf());
    
    runSimulation(config, mgr);
    
    std::cin.rdbuf(oldIn);
    std::cout.rdbuf(oldOut);
    std::cin.clear();
    return output.str();
}

// Генератор ввода для пропуска недель (нажимаем "3" и Enter)
std::string skipWeeksInput(int weeks) {
    std::string s;
    for (int i = 0; i < weeks; ++i) s += "3\n\n";
    return s;
}

int main() {
    // --- Тестирование логики (из 3 лабы) ---
    {
        CitySimulation city(10, 1000, 100);
        CityObject house("Жилой дом", 200, 20, 30, 5);
        city.construct(house);
        assertContains("Строительство: бюджет", std::to_string(city.currentBudget), "800");
        assertContains("Строительство: люди", std::to_string(city.totalPopulation), "130");
    }

    // --- Тестирование сохранений (из 4 лабы) ---
    {
        std::remove("saves.txt");
        SaveSlotManager mgr;
        CitySimulation city(10, 5000, 500);
        PlayerActions player;
        player.currentWeek = 3;
        player.builtCount = 2;
        player.movesLeft = 2; // Установили остаток ходов
        
        // Сохраняем в 1 слот
        mgr.saveGame(1, city, player);
        
        // Проверяем, что файл создался
        std::ifstream f("saves.txt");
        assertContains("Сохранение: файл создан", f.good() ? "1" : "0", "1");
        f.close();

        // Меняем данные, чтобы проверить, что загрузка их перезапишет
        city.currentBudget = 0;
        player.currentWeek = 0;
        player.movesLeft = 5;

        // Загружаем
        std::vector<CityObject> catalog = getCatalog();
        bool loaded = mgr.loadGame(1, city, player, catalog);
        
        assertContains("Загрузка: успешно", loaded ? "1" : "0", "1");
        assertContains("Загрузка: бюджет", std::to_string(city.currentBudget), "5000");
        assertContains("Загрузка: неделя", std::to_string(player.currentWeek), "3");
        assertContains("Загрузка: ходы", std::to_string(player.movesLeft), "2");
    }

    // --- Тестирование симуляции (UI) ---
    GameConfig cfg;
    SaveSlotManager mgr;
    // Пропускаем 6 недель, чтобы игра дошла до конца
    std::string out = simulateGame(skipWeeksInput(6), cfg, mgr);
    assertContains("Игра: заголовок", out, "СТАРТ СИМУЛЯЦИИ");
    assertContains("Игра: финал", out, "СИМУЛЯЦИЯ ЗАВЕРШЕНА");

    std::cout << "\nРезультаты: " << passedCount
              << " успешно | " << failedCount << " провалено\n";
    return failedCount ? 1 : 0;
}
