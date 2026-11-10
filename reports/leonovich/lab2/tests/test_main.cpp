#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cstdio>

using namespace std;

const string DEFAULT_DIFFICULTY    = "Легкий";
const int    DEFAULT_ALL_PLAYERS   = 4;
const int    DEFAULT_HUMAN_PLAYERS = 2;

const string SETTINGS_FILE       = "test_settings.txt";
const string COMPUTER_NAMES_FILE = "test_computer_names.txt";

struct Settings {
    string difficulty   = DEFAULT_DIFFICULTY;
    int    allPlayers   = DEFAULT_ALL_PLAYERS;
    int    humanPlayers = DEFAULT_HUMAN_PLAYERS;

    int aiPlayers() const { return allPlayers - humanPlayers; }

    void reset() {
        difficulty   = DEFAULT_DIFFICULTY;
        allPlayers   = DEFAULT_ALL_PLAYERS;
        humanPlayers = DEFAULT_HUMAN_PLAYERS;
    }

    bool setDifficulty(int c) {
        if (c == 1) { difficulty = "Легкий";  return true; }
        if (c == 2) { difficulty = "Средний"; return true; }
        if (c == 3) { difficulty = "Сложный"; return true; }
        return false;
    }

    bool setAllPlayers(int n) {
        if (n < 2 || n > 5) return false;
        allPlayers = n;
        if (humanPlayers > allPlayers) humanPlayers = allPlayers;
        return true;
    }

    bool setHumanPlayers(int n) {
        if (n < 1 || n > allPlayers) return false;
        humanPlayers = n;
        return true;
    }
};

void createDefaultSettingsFile() {
    ifstream file(SETTINGS_FILE);
    if (!file.is_open()) {
        ofstream nf(SETTINGS_FILE);
        if (nf.is_open()) {
            nf << DEFAULT_DIFFICULTY << '\n'
               << DEFAULT_ALL_PLAYERS << '\n'
               << DEFAULT_HUMAN_PLAYERS << '\n';
        }
    }
}

bool loadSettings(Settings& s) {
    ifstream file(SETTINGS_FILE);
    if (!file.is_open()) return false;

    string d;
    int a, h;
    if (getline(file, d) && file >> a && file >> h) {
        if (a >= 2 && a <= 5 && h >= 1 && h <= a &&
            (d == "Легкий" || d == "Средний" || d == "Сложный")) {
            s.difficulty   = d;
            s.allPlayers   = a;
            s.humanPlayers = h;
            return true;
        }
    }
    return false;
}

bool saveSettings(const Settings& s) {
    ofstream file(SETTINGS_FILE);
    if (!file.is_open()) return false;
    file << s.difficulty << '\n' << s.allPlayers << '\n' << s.humanPlayers << '\n';
    return true;
}

void createComputerNamesFile() {
    ifstream file(COMPUTER_NAMES_FILE);
    if (!file.is_open()) {
        ofstream nf(COMPUTER_NAMES_FILE);
        if (nf.is_open()) {
            const char* names[] = {
                "Берсерк","Феникс","Скорпион","Данте","Терминатор",
                "Элой","Гладиатор","Тайфун","Атлант","Шторм",
                "Леон","Торнадо","Титан","Морбиус","Альтер"
            };
            for (auto n : names) nf << n << '\n';
        }
    }
}

set<string> loadComputerNames() {
    set<string> names;
    ifstream file(COMPUTER_NAMES_FILE);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) if (!line.empty()) names.insert(line);
    }
    return names;
}

static int g_passed = 0, g_failed = 0;
static vector<string> g_failures;

void check(bool cond, const string& name) {
    if (cond) { ++g_passed; cout << "  [ OK ] " << name << "\n"; }
    else      { ++g_failed; g_failures.push_back(name); cout << "  [FAIL] " << name << "\n"; }
}

template <typename T>
void checkEq(const T& a, const T& e, const string& name) {
    if (a == e) { ++g_passed; cout << "  [ OK ] " << name << "\n"; }
    else {
        ++g_failed; g_failures.push_back(name);
        cout << "  [FAIL] " << name << " (получено: " << a << ", ожидалось: " << e << ")\n";
    }
}

void cleanup() {
    remove(SETTINGS_FILE.c_str());
    remove(COMPUTER_NAMES_FILE.c_str());
}

void testDefaults() {
    cout << "\n[1] Значения по умолчанию\n";
    Settings s;
    checkEq(s.difficulty,   string("Легкий"), "сложность = Легкий");
    checkEq(s.allPlayers,   4,                "всего = 4");
    checkEq(s.humanPlayers, 2,                "живых = 2");
    checkEq(s.aiPlayers(),  2,                "ИИ = 2");
}

void testSettingsValidation() {
    cout << "\n[2] Валидация настроек в памяти\n";
    Settings s;
    check(s.setDifficulty(2), "сложность 2 принята");
    check(!s.setDifficulty(9), "сложность 9 отклонена");
    check(s.setAllPlayers(5),  "всего=5 принято");
    check(!s.setAllPlayers(1), "всего=1 отклонено");
    check(s.setHumanPlayers(3),"живых=3 принято");
    check(!s.setHumanPlayers(6), "живых=6 при всего=5 отклонено");
}

void testCreateSettingsFile() {
    cout << "\n[3] Создание файла настроек по умолчанию\n";
    cleanup();
    createDefaultSettingsFile();
    ifstream f(SETTINGS_FILE);
    check(f.is_open(), "файл настроек создан");
    string d; int a, h;
    bool ok = (getline(f, d) && (f >> a) && (f >> h));
    check(ok, "файл содержит 3 строки");
    checkEq(d, string("Легкий"), "сложность = Легкий");
    checkEq(a, 4, "всего = 4");
    checkEq(h, 2, "живых = 2");
    f.close();
}

void testSaveAndLoad() {
    cout << "\n[4] Сохранение и загрузка настроек\n";
    cleanup();
    Settings s;
    s.difficulty = "Сложный";
    s.allPlayers = 5;
    s.humanPlayers = 3;
    check(saveSettings(s), "saveSettings вернул true");

    Settings loaded;
    check(loadSettings(loaded), "loadSettings вернул true");
    checkEq(loaded.difficulty,   string("Сложный"), "загружено: сложность = Сложный");
    checkEq(loaded.allPlayers,   5,                 "загружено: всего = 5");
    checkEq(loaded.humanPlayers, 3,                 "загружено: живых = 3");
}

void testLoadInvalidFile() {
    cout << "\n[5] Загрузка некорректного файла\n";
    cleanup();
    {
        ofstream f(SETTINGS_FILE);
        f << "Сложный\n10\n3\n"; // allPlayers > 5
    }
    Settings s;
    check(!loadSettings(s), "некорректный allPlayers отклонён");
    checkEq(s.allPlayers, 4, "значения остались по умолчанию");

    {
        ofstream f(SETTINGS_FILE);
        f << "Непонятно\n4\n2\n"; // неверная сложность
    }
    Settings s2;
    check(!loadSettings(s2), "некорректная сложность отклонена");

    {
        ofstream f(SETTINGS_FILE);
        f << "Легкий\n4\n9\n"; // human > all
    }
    Settings s3;
    check(!loadSettings(s3), "humanPlayers > allPlayers отклонён");
}

void testLoadMissingFile() {
    cout << "\n[6] Загрузка при отсутствии файла\n";
    cleanup();
    Settings s;
    check(!loadSettings(s), "loadSettings вернул false");
    checkEq(s.allPlayers, 4, "значения по умолчанию не изменены");
}

void testComputerNamesFile() {
    cout << "\n[7] Создание и загрузка файла имён ИИ\n";
    cleanup();
    createComputerNamesFile();
    auto names = loadComputerNames();
    checkEq((int)names.size(), 15, "загружено 15 имён");
    check(names.count("Берсерк") == 1, "содержит 'Берсерк'");
    check(names.count("Альтер")  == 1, "содержит 'Альтер'");
}

void testComputerNamesUnique() {
    cout << "\n[8] Уникальность имён (set)\n";
    cleanup();
    {
        ofstream f(COMPUTER_NAMES_FILE);
        f << "Альфа\nАльфа\nБета\n\nГамма\n";
    }
    auto names = loadComputerNames();
    checkEq((int)names.size(), 3, "дубликаты удалены, пустые строки пропущены");
}

void testReset() {
    cout << "\n[9] Сброс настроек\n";
    Settings s;
    s.difficulty = "Сложный"; s.allPlayers = 5; s.humanPlayers = 5;
    s.reset();
    checkEq(s.difficulty,   string("Легкий"), "сложность = Легкий");
    checkEq(s.allPlayers,   4,                "всего = 4");
    checkEq(s.humanPlayers, 2,                "живых = 2");
    checkEq(s.aiPlayers(),  2,                "ИИ = 2");
}

void testInvariantAiNonNegative() {
    cout << "\n[10] Инвариант: ИИ >= 0 для всех валидных комбинаций\n";
    bool allOk = true;
    for (int total = 2; total <= 5; ++total) {
        for (int human = 1; human <= total; ++human) {
            Settings s;
            s.allPlayers = total; s.humanPlayers = human;
            if (s.aiPlayers() < 0) { allOk = false; }
        }
    }
    check(allOk, "ai >= 0 для всех валидных (total, human)");
}

int main() {
    cout << " Запуск тестов: настройка взаимодействия с файлами\n";

    testDefaults();
    testSettingsValidation();
    testCreateSettingsFile();
    testSaveAndLoad();
    testLoadInvalidFile();
    testLoadMissingFile();
    testComputerNamesFile();
    testComputerNamesUnique();
    testReset();
    testInvariantAiNonNegative();

    cleanup();

    cout << " Итог: пройдено " << g_passed << ", провалено " << g_failed << "\n";

    if (g_failed > 0) {
        cout << "\nПроваленные тесты:\n";
        for (auto& f : g_failures) cout << "  - " << f << "\n";
        return 1;
    }
    cout << "\nВсе тесты пройдены успешно.\n";
    return 0;
}
