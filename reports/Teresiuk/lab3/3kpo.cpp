#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stack>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <vector>

using namespace std;

const int MENU_START_GAME = 1;
const int MENU_LOAD_GAME = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SET_DIFFICULTY = 1;
const int SET_DUNGEON_SIZE = 2;
const int SET_LIVES = 3;
const int SET_ROLLBACK_SIZE = 4;
const int SET_BACK = 5;

const string PROGRAM_VERSION = "3.0";
const string STUDENT_NAME = "Тересюк Д. Ю.";
const string SETTINGS_FILE = "settings.txt";
const string LOG_FILE = "app.log";

const int SMALL_POTION_HEAL = 20;
const int LARGE_POTION_HEAL = 50;

void logMessage(const string& message) {
    try {
        ofstream f(LOG_FILE, ios::app);
        if (!f.is_open()) return;
        time_t t = time(nullptr);
        struct tm timeinfo;
#ifdef _WIN32
        localtime_s(&timeinfo, &t);
#else
        localtime_r(&t, &timeinfo);
#endif
        char buf[32];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        f << "[" << buf << "] " << message << "\n";
    }
    catch (...) {}
}

struct GameSettings {
    int    dungeonSize = 10;
    int    lives = 3;
    string difficulty = "Легко";
};

GameSettings settings;
stack<int> sizeHistory;

bool isValidDungeonSize(int s) { return s >= 5 && s <= 100; }
bool isValidLives(int l) { return l >= 1 && l <= 10; }
bool isValidDifficulty(const string& d) {
    return d == "Легко" || d == "Средне" || d == "Сложно";
}

bool fileExists(const string& path) {
    ifstream f(path);
    return f.good();
}

bool saveSettings() {
    try {
        ofstream f(SETTINGS_FILE);
        if (!f.is_open()) throw runtime_error("Не открыт settings.txt");
        f << "dungeonSize=" << settings.dungeonSize << "\n";
        f << "lives=" << settings.lives << "\n";
        f << "difficulty=" << settings.difficulty << "\n";
        logMessage("Настройки сохранены");
        return true;
    }
    catch (const exception& e) {
        logMessage(string("Ошибка сохранения: ") + e.what());
        return false;
    }
}

bool loadSettings() {
    try {
        ifstream f(SETTINGS_FILE);
        if (!f.is_open()) throw runtime_error("Файл настроек не открывается");
        string line;
        while (getline(f, line)) {
            istringstream iss(line);
            string key;
            if (!getline(iss, key, '=')) continue;
            if (key == "dungeonSize")      iss >> settings.dungeonSize;
            else if (key == "lives")       iss >> settings.lives;
            else if (key == "difficulty") {
                getline(iss, settings.difficulty);
                if (!settings.difficulty.empty() && settings.difficulty[0] == ' ')
                    settings.difficulty.erase(0, 1);
            }
        }
        logMessage("Настройки загружены");
        return true;
    }
    catch (const exception& e) {
        logMessage(string("Ошибка загрузки: ") + e.what());
        return false;
    }
}

void createDefaultSettingsFile() {
    GameSettings def;
    try {
        ofstream f(SETTINGS_FILE);
        if (!f.is_open()) throw runtime_error("Не создать settings.txt");
        f << "dungeonSize=" << def.dungeonSize << "\n";
        f << "lives=" << def.lives << "\n";
        f << "difficulty=" << def.difficulty << "\n";
        logMessage("Создан файл настроек по умолчанию");
    }
    catch (const exception& e) {
        logMessage(string("Ошибка создания файла: ") + e.what());
    }
}

class Weapon {
public:
    Weapon() : name_("Кулаки"), damage_(1), bonus_(0), equipped_(true) {}
    Weapon(const string& name, int damage, int bonus = 0)
        : name_(name), damage_(damage), bonus_(bonus), equipped_(false) {
    }

    string getName()   const { return name_; }
    int    getDamage() const { return damage_; }
    int    getBonus()  const { return bonus_; }
    bool   isEquipped() const { return equipped_; }
    void   setEquipped(bool e) { equipped_ = e; }

private:
    string name_;
    int damage_;
    int bonus_;
    bool equipped_;
};

class Character {
public:
    Character(const string& name = "Герой", int hp = 100, int baseDamage = 5,
        int smallPotions = 2, int largePotions = 1)
        : name_(name), hp_(hp), maxHp_(hp), baseDamage_(baseDamage),
        weapon_(), smallPotions_(smallPotions), largePotions_(largePotions) {
    }

    string getName() const { return name_; }
    int    getHp()  const { return hp_; }
    int    getMaxHp() const { return maxHp_; }
    bool   isAlive()  const { return hp_ > 0; }

    int getSmallPotions() const { return smallPotions_; }
    int getLargePotions() const { return largePotions_; }

    int getTotalDamage() const {
        return baseDamage_ + weapon_.getDamage() + weapon_.getBonus();
    }

    const Weapon& getWeapon() const { return weapon_; }

    void takeDamage(int dmg) { hp_ = max(0, hp_ - dmg); }
    void heal(int amount) { hp_ = min(maxHp_, hp_ + amount); }

    bool equipWeapon(const Weapon& w) {
        if (w.getDamage() <= 0) return false;
        weapon_ = w;
        weapon_.setEquipped(true);
        return true;
    }

    void unequipWeapon() { weapon_ = Weapon(); }

    bool useSmallPotion() {
        if (smallPotions_ <= 0) return false;
        --smallPotions_;
        heal(SMALL_POTION_HEAL);
        return true;
    }

    bool useLargePotion() {
        if (largePotions_ <= 0) return false;
        --largePotions_;
        heal(LARGE_POTION_HEAL);
        return true;
    }

    void addSmallPotion(int n = 1) { smallPotions_ += n; }
    void addLargePotion(int n = 1) { largePotions_ += n; }

private:
    string name_;
    int hp_;
    int maxHp_;
    int baseDamage_;
    Weapon weapon_;
    int smallPotions_;
    int largePotions_;
};

class Monster {
public:
    Monster(const string& name, int hp, int damage,
        Weapon drop, double weaponDropChance,
        double smallPotionChance = 0.0,
        double largePotionChance = 0.0)
        : name_(name), hp_(hp), damage_(damage),
        dropWeapon_(drop),
        weaponDropChance_(weaponDropChance),
        smallPotionChance_(smallPotionChance),
        largePotionChance_(largePotionChance) {
    }

    string getName()   const { return name_; }
    int    getHp()     const { return hp_; }
    int    getDamage() const { return damage_; }
    bool   isAlive()   const { return hp_ > 0; }

    void takeDamage(int dmg) { hp_ = max(0, hp_ - dmg); }

    bool tryDropWeapon(Weapon& out) const {
        if (!rollChance(weaponDropChance_)) return false;
        out = dropWeapon_;
        out.setEquipped(false);
        return true;
    }

    bool tryDropSmallPotion() const { return rollChance(smallPotionChance_); }
    bool tryDropLargePotion() const { return rollChance(largePotionChance_); }

private:
    static bool rollChance(double chance) {
        if (chance <= 0.0) return false;
        double r = static_cast<double>(rand()) / RAND_MAX;
        return r <= chance;
    }

    string name_;
    int hp_;
    int damage_;
    Weapon dropWeapon_;
    double weaponDropChance_;
    double smallPotionChance_;
    double largePotionChance_;
};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) return value;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Ошибка: необходимо ввести число!\n";
    }
}

void waitForEnter() {
    cout << "\nНажмите Enter для возврата в меню...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void showMainMenu() {
    cout << "\n ПРИКЛЮЧЕНИЕ В ПОДЗЕМЕЛЬЕ\n";
    cout << MENU_START_GAME << ". Начать игру\n";
    cout << MENU_LOAD_GAME << ". Загрузить игру\n";
    cout << MENU_SETTINGS << ". Настройки\n";
    cout << MENU_ABOUT << ". О программе\n";
    cout << MENU_EXIT << ". Выход\n";
}

void showSettingsMenu() {
    cout << "\n НАСТРОЙКИ \n";
    cout << SET_DIFFICULTY << ". Уровень сложности (текущий: "
        << settings.difficulty << ")\n";
    cout << SET_DUNGEON_SIZE << ". Величина подземелья (текущая: "
        << settings.dungeonSize << ")\n";
    cout << SET_LIVES << ". Количество жизней (текущее: "
        << settings.lives << ")\n";
    cout << SET_ROLLBACK_SIZE << ". Вернуть предыдущий размер (в истории: "
        << sizeHistory.size() << ")\n";
    cout << SET_BACK << ". Назад в главное меню\n";
}

bool fight(Character& player, Monster& monster) {
    cout << "\n=== Бой: " << player.getName()
        << " vs " << monster.getName() << " ===\n";
    logMessage("Бой: " + player.getName() + " vs " + monster.getName());

    while (player.isAlive() && monster.isAlive()) {
        cout << "\n" << player.getName()
            << " HP: " << player.getHp() << "/" << player.getMaxHp()
            << "   |   " << monster.getName()
            << " HP: " << monster.getHp() << "\n";

        cout << "1. Атаковать\n";
        cout << "2. Малое зелье  (+" << SMALL_POTION_HEAL << " HP) ["
            << player.getSmallPotions() << " шт]\n";
        cout << "3. Большое зелье (+" << LARGE_POTION_HEAL << " HP) ["
            << player.getLargePotions() << " шт]\n";
        cout << "4. Бежать\n";

        int choice = readInt("Действие: ");
        if (choice == 1) {
            int dmg = player.getTotalDamage();
            monster.takeDamage(dmg);
            cout << "Вы нанесли " << dmg << " урона.\n";
            logMessage("Игрок нанёс " + to_string(dmg));
        }
        else if (choice == 2) {
            if (!player.useSmallPotion()) {
                cout << "У вас нет малых зелий!\n";
                continue; 
            }
            cout << "Вы выпили малое зелье (+" << SMALL_POTION_HEAL
                << " HP). Осталось: " << player.getSmallPotions() << "\n";
            logMessage("Игрок выпил малое зелье");
        }
        else if (choice == 3) {
            if (!player.useLargePotion()) {
                cout << "У вас нет больших зелий!\n";
                continue; 
            }
            cout << "Вы выпили большое зелье (+" << LARGE_POTION_HEAL
                << " HP). Осталось: " << player.getLargePotions() << "\n";
            logMessage("Игрок выпил большое зелье");
        }
        else if (choice == 4) {
            cout << "Вы сбежали!\n";
            logMessage("Игрок сбежал");
            return false;
        }
        else {
            cout << "Нет такого действия!\n";
            continue;
        }

        if (!monster.isAlive()) break;

        int mDmg = monster.getDamage();
        player.takeDamage(mDmg);
        cout << monster.getName() << " наносит " << mDmg << " урона.\n";
        logMessage("Монстр нанёс " + to_string(mDmg));
    }

    if (player.isAlive()) {
        cout << "\n>>> ПОБЕДА! " << monster.getName() << " повержен.\n";
        logMessage("Победа над " + monster.getName());

        Weapon dropped;
        if (monster.tryDropWeapon(dropped)) {
            cout << "Выпало оружие: " << dropped.getName()
                << " (урон " << dropped.getDamage();
            if (dropped.getBonus() > 0) cout << ", бонус +" << dropped.getBonus();
            cout << ")\n";

            int eq = readInt("Экипировать? (1 - да, 2 - нет): ");
            if (eq == 1) {
                player.equipWeapon(dropped);
                cout << "Экипировано: " << dropped.getName() << "\n";
                logMessage("Экипировано: " + dropped.getName());
            }
        }
        else {
            cout << "Оружие не выпало.\n";
        }

        if (monster.tryDropSmallPotion()) {
            player.addSmallPotion(1);
            cout << "Выпало МАЛОЕ зелье! (+"
                << SMALL_POTION_HEAL << " HP), теперь: "
                << player.getSmallPotions() << "\n";
            logMessage("Выпало малое зелье");
        }

        if (monster.tryDropLargePotion()) {
            player.addLargePotion(1);
            cout << "Выпало БОЛЬШОЕ зелье! (+"
                << LARGE_POTION_HEAL << " HP), теперь: "
                << player.getLargePotions() << "\n";
            logMessage("Выпало большое зелье");
        }

        return true;
    }

    cout << "\n>>> ВЫ ПОГИБЛИ...\n";
    logMessage("Игрок погиб");
    return false;
}

void runDungeon(Character& player, int rooms) {
    vector<Monster> pool = {
        Monster("Гоблин",     20,  5, Weapon("Кинжал гоблина",  5, 0), 0.5, 0.30, 0.05),
        Monster("Скелет",     30,  8, Weapon("Ржавый меч",      8, 0), 0.4, 0.25, 0.10),
        Monster("Орк",        50, 12, Weapon("Топор орка",     12, 2), 0.3, 0.20, 0.15),
        Monster("Тёмный маг", 40, 15, Weapon("Посох тьмы",     10, 5), 0.5, 0.15, 0.25),
        Monster("Дракончик",  80, 20, Weapon("Коготь дракона", 18, 3), 0.6, 0.20, 0.20),
    };

    for (int room = 1; room <= rooms && player.isAlive(); ++room) {
        cout << "\n=========== Комната " << room << "/" << rooms
            << " ===========\n";

        Monster m = pool[rand() % pool.size()];
        fight(player, m);

        if (!player.isAlive()) {
            cout << "Игра окончена на комнате " << room << "\n";
            return;
        }
    }

    if (player.isAlive())
        cout << "\n Вы прошли подземелье!\n";
}

void startGame() {
    cout << "\n>>> Игра началась!\n";
    cout << "    Сложность: " << settings.difficulty << "\n";
    cout << "    Размер подземелья: " << settings.dungeonSize << "\n";
    cout << "    Жизней: " << settings.lives << "\n";

    int baseDamage = 10;
    int startSmall = 3, startLarge = 2;
    if (settings.difficulty == "Средне") {
        baseDamage = 7;
        startSmall = 2; startLarge = 1;
    }
    if (settings.difficulty == "Сложно") {
        baseDamage = 5;
        startSmall = 1; startLarge = 1;
    }

    cout << "    Стартовые зелья: малых " << startSmall
        << ", больших " << startLarge << "\n";

    Character player("Герой", settings.lives * 50, baseDamage,
        startSmall, startLarge);
    runDungeon(player, settings.dungeonSize);
}

void loadGame() {
    cout << "\n>>> Функция загрузки игры пока недоступна.\n";
}

void showAbout() {
    cout << "\n О ПРОГРАММЕ \n";
    cout << "Название: Приключение в подземелье\n";
    cout << "Версия:   " << PROGRAM_VERSION << "\n";
    cout << "Студент:  " << STUDENT_NAME << "\n";
}

void openSettings() {
    int choice = 0;
    while (true) {
        clearScreen();
        showSettingsMenu();
        choice = readInt("Выберите параметр: ");

        switch (choice) {
        case SET_DIFFICULTY: {
            int d = readInt("1 - Легко, 2 - Средне, 3 - Сложно. Ваш выбор: ");
            string diff = (d == 1) ? "Легко" :
                (d == 2) ? "Средне" :
                (d == 3) ? "Сложно" : "";
            if (isValidDifficulty(diff)) {
                settings.difficulty = diff;
                saveSettings();
                cout << "Сложность сохранена.\n";
            }
            else {
                cout << "Ошибка ввода!\n";
            }
            waitForEnter();
            break;
        }
        case SET_DUNGEON_SIZE: {
            int s = readInt("Введите величину подземелья (5..100): ");
            if (isValidDungeonSize(s)) {
                sizeHistory.push(settings.dungeonSize);
                settings.dungeonSize = s;
                saveSettings();
                cout << "Размер сохранён.\n";
            }
            else {
                cout << "Ошибка ввода! Допустимо 5..100.\n";
            }
            waitForEnter();
            break;
        }
        case SET_LIVES: {
            int l = readInt("Введите количество жизней (1..10): ");
            if (isValidLives(l)) {
                settings.lives = l;
                saveSettings();
                cout << "Жизни сохранены.\n";
            }
            else {
                cout << "Ошибка ввода! Допустимо 1..10.\n";
            }
            waitForEnter();
            break;
        }
        case SET_ROLLBACK_SIZE: {
            if (!sizeHistory.empty()) {
                settings.dungeonSize = sizeHistory.top();
                sizeHistory.pop();
                saveSettings();
                cout << "Размер возвращён к " << settings.dungeonSize << "\n";
            }
            else {
                cout << "История пуста!\n";
            }
            waitForEnter();
            break;
        }
        case SET_BACK:
            return;
        default:
            cout << "Ошибка: такого пункта нет!\n";
            waitForEnter();
        }
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    srand(static_cast<unsigned>(time(nullptr)));
    logMessage("Программа запущена (v" + PROGRAM_VERSION + ")");

    if (!fileExists(SETTINGS_FILE)) {
        cout << "Файл настроек не найден. Создаётся по умолчанию...\n";
        createDefaultSettingsFile();
    }
    loadSettings();

    int choice = 0;
    while (true) {
        clearScreen();
        showMainMenu();
        choice = readInt("Введите номер пункта меню: ");

        switch (choice) {
        case MENU_START_GAME: clearScreen(); startGame();    waitForEnter(); break;
        case MENU_LOAD_GAME:  clearScreen(); loadGame();     waitForEnter(); break;
        case MENU_SETTINGS:   clearScreen(); openSettings(); break;
        case MENU_ABOUT:      clearScreen(); showAbout();    waitForEnter(); break;
        case MENU_EXIT:
            clearScreen();
            logMessage("Программа завершена");
            cout << "\nВыход из программы. До свидания!\n";
            return 0;
        default:
            cout << "\nОшибка: такого пункта нет!\n";
            waitForEnter();
        }
    }
}