#define UNIT_TEST
#include "../src/main.cpp"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <string>

// Функция проверки. Счетчики передаются по ссылке, чтобы не делать их глобальными
void assertContains(const std::string& testName,
                    const std::string& text,
                    const std::string& expected,
                    int& passed, int& failed,
                    bool shouldContain = true) {
    bool found = text.find(expected) != std::string::npos;
    if (found == shouldContain) {
        std::cout << "[ОК] " << testName << "\n";
        ++passed;
    } else {
        std::cout << "[ПРОВАЛ] " << testName
                  << " (искали: \"" << expected << "\")\n";
        ++failed;
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
    int passed = 0;
    int failed = 0;

    // ==========================================
    // --- Тестирование логики (из 3 лабы) ---
    // ==========================================
    {
        CitySimulation city(10, 1000, 100);
        CityObject house("Жилой дом", 200, 20, 30, 5);
        city.construct(house);
        assertContains("Строительство: бюджет", std::to_string(city.currentBudget), "800", passed, failed);
        assertContains("Строительство: люди", std::to_string(city.totalPopulation), "130", passed, failed);
    }

    // ==========================================
    // --- Тестирование катаклизмов и защиты (ЛР5) ---
    // ==========================================
    {
        // Проверка пожара БЕЗ защиты
        CitySimulation city(10, 1000, 100);
        city.triggerCataclysm(ProtectionType::FIRE); 
        assertContains("Пожар без защиты: бюджет", std::to_string(city.currentBudget), "500", passed, failed);
        
        // Проверка пожара С защитой
        CitySimulation city2(10, 1000, 100);
        CityObject fireStation("Пожарная станция", 450, 0, 0, 5, ProtectionType::FIRE);
        city2.construct(fireStation); // Потратили 450 (осталось 550)
        city2.triggerCataclysm(ProtectionType::FIRE); // Сгорает еще 100
        assertContains("Пожар с защитой: бюджет", std::to_string(city2.currentBudget), "450", passed, failed);
    }

    // ==========================================
    // --- Тестирование сохранений (из 4 лабы) ---
    // ==========================================
    {
        std::remove("saves.txt");
        SaveSlotManager mgr;
        CitySimulation city(10, 5000, 500);
        PlayerActions player;
        player.currentWeek = 3;
        player.builtCountAction = 2;
        player.movesLeft = 2; // Установили остаток ходов
        
        // Сохраняем в 1 слот
        mgr.saveGame(1, city, player);
        
        // Меняем данные, чтобы проверить, что загрузка их перезапишет
        city.currentBudget = 0;
        player.currentWeek = 0;
        player.movesLeft = 5;

        // Загружаем
        CityObject catalog[CATALOG_SIZE];
        initCatalog(catalog);
        bool loaded = mgr.loadGame(1, city, player, catalog, CATALOG_SIZE);
        
        assertContains("Загрузка: успешно", loaded ? "1" : "0", "1", passed, failed);
        assertContains("Загрузка: бюджет", std::to_string(city.currentBudget), "5000", passed, failed);
        assertContains("Загрузка: неделя", std::to_string(player.currentWeek), "3", passed, failed);
        assertContains("Загрузка: ходы", std::to_string(player.movesLeft), "2", passed, failed);
    }

    // ==========================================
    // --- Тестирование симуляции и UI ---
    // ==========================================
    GameConfig cfg;
    SaveSlotManager mgr;
    
    // Полное прохождение (пропуск всех недель)
    std::string out = simulateGame(skipWeeksInput(6), cfg, mgr);
    assertContains("Игра: заголовок", out, "СТАРТ СИМУЛЯЦИИ", passed, failed);
    assertContains("Игра: финал", out, "СИМУЛЯЦИЯ ЗАВЕРШЕНА", passed, failed);

    // Проверка ввода кастомного названия праздника
    std::string festivalInput = "2\nМой Супер Праздник\n\n" + skipWeeksInput(5);
    out = simulateGame(festivalInput, cfg, mgr);
    assertContains("Праздник: кастомное имя", out, "Праздник \"Мой Супер Праздник\" удался", passed, failed);

    // Итог
    std::cout << "\n========================================\n";
    std::cout << "Результаты: " << passed << " успешно | " << failed << " провалено\n";
    std::cout << "========================================\n";

    return failed ? 1 : 0;
}
