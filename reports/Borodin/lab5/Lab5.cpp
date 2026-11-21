#define NOMINMAX
#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <stack>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <limits>

void DisplayMenu();
void GameStart();
void GameLoad();
void Settings();
void About();
void (* const menuFunctions[])() = {
    GameStart,
    GameLoad,
    Settings
};
enum {
    AboutOp = 4, ExitOp
};

struct Options {
    int health = 10;
    int size = 50;
    std::string diff = "Легко";
};

enum ItemType {
    ItemWeapon = 0,
    ItemPotion = 1,
    ItemAmulet = 2,
    ItemScroll = 3,
    ItemNone = -1
};

struct SaveData {
    bool used = false;
    std::string timeStr;
    std::string heroName;
    int heroHealth = 0;
    int heroMaxHealth = 0;
    std::string weaponName;
    int weaponDamage = 0;
    int weaponBonusDamage = 0;
    std::string weaponArt;
    int floor = 0;
    int totalFloors = 0;
    Options opts;
    bool inBattle = false;
    int monsterIndex = -1;
    std::string monsterName;
    int monsterHealth = 0;
    int monsterMaxHealth = 0;
    int monsterDamage = 0;
    std::string monsterArt;
    int dropIndex = -1;
    int dropType = -1;
    std::string dropName;
    int dropValue = 0;
    std::string dropArt;
    bool monsterSpecialUsed = false;
    int poisonTurnsLeft = 0;
    int pendingBonusType = -1;
    std::string pendingBonusName;
    int pendingBonusValue = 0;
};

static std::string EscapeField(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '|') r += "\\p";
        else r += c;
    }
    return r;
}

static std::string UnescapeField(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[i + 1];
            if (n == 'n') { r += '\n'; ++i; }
            else if (n == '\\') { r += '\\'; ++i; }
            else if (n == 'p') { r += '|'; ++i; }
            else r += s[i];
        }
        else r += s[i];
    }
    return r;
}

static int ReadInt() {
    int value;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ошибка: введите число. Повторите: ";
    }
    return value;
}

static int ReadIntInRange(int lo, int hi, const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int v = ReadInt();
        if (v >= lo && v <= hi) return v;
        std::cout << "Ошибка: неверное значение. Допустимо от " << lo << " до " << hi << ".\n";
    }
}

class Weapon {
public:
    std::string name;
    int damage;
    std::string art;
    Weapon() : name("Кулаки"), damage(2), art(
        "   ______\n"
        "  /      \\\n"
        " |  ____  |\n"
        " | |    | |\n"
        " | |____| |\n"
        "  \\______/"
    ) {
    }
    Weapon(std::string n, int d, std::string a = "") : name(n), damage(d), art(a) {}
};

class Character {
public:
    std::string name;
    int maxHealth;
    int health;
    int bonusDamage;
    Weapon weapon;
    Character() : name("Герой"), maxHealth(10), health(10), bonusDamage(0), weapon() {}
    Character(std::string n, int h) : name(n), maxHealth(h), health(h), bonusDamage(0), weapon() {}
    void EquipWeapon(const Weapon& w) {
        weapon = w;
        std::cout << "Вы экипировали: " << w.name << " (урон: " << w.damage << ")\n";
    }
    int Attack() {
        return weapon.damage + bonusDamage;
    }
    void TakeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }
    void Heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
    }
    void AddMaxHealth(int amount) {
        maxHealth += amount;
        health += amount;
    }
    void AddBonusDamage(int amount) {
        bonusDamage += amount;
    }
    bool IsAlive() const {
        return health > 0;
    }
};

class Monster {
public:
    std::string name;
    int maxHealth;
    int health;
    int damage;
    int typeIndex;
    bool specialUsed;
    Weapon drop;
    bool hasDrop;
    std::string art;
    int index;
    Monster() : name("Слизень"), maxHealth(5), health(5), damage(1), typeIndex(0),
        specialUsed(false), drop(), hasDrop(true), index(0), art(
            "    _____\n"
            "   /     \\\n"
            "  |  o o  |\n"
            "  |   _   |\n"
            "   \\_____/"
        ) {
    }
    Monster(std::string n, int h, int d, Weapon w, bool hd, std::string a = "", int idx = 0)
        : name(n), maxHealth(h), health(h), damage(d), typeIndex(idx),
        specialUsed(false), drop(w), hasDrop(hd), art(a), index(idx) {
    }
    void TakeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }
    bool IsAlive() const {
        return health > 0;
    }
};

static const char* kWeaponNames[] = { "Ржавый меч", "Дубина", "Кинжал", "Топор", "Копьё", "Булава" };
static const char* kWeaponArts[] = {
    "        /\\\n       /  |\n      /   |\n     /    |\n    /     |\n   /______|\n   |______|\n     |  |\n     |  |\n     |  |\n    _|__|_",
    "      ____\n     / .. \\\n    | .  . |\n    |  ..  |\n     \\____/\n      |  |\n      |  |\n      |  |\n      |  |\n     _|__|_",
    "       /\\\n      /  \\\n     /    \\\n    /______\\\n    |______|\n      |  |\n      |  |\n      |  |\n     _|__|_",
    "     _____\n    /     \\\n   /  ___  \\\n  |  /   \\  |\n  |  \\___/  |\n   \\_______/\n      |  |\n      |  |\n      |  |\n     _|__|_",
    "        /\\\n       /  \\\n      /    \\\n     /      \\\n    /________\\\n       |  |\n       |  |\n       |  |\n       |  |\n       |  |\n      _|__|_",
    "     _/\\_/\\_\n    /  o o  \\\n   | o  |  o |\n    \\  o o  /\n     \\_/|\\_/\n       |  |\n       |  |\n       |  |\n      _|__|_"
};
static const int kWeaponCount = 6;

static const char* kMonsterNames[] = { "Слизень", "Гоблин", "Скелет", "Паук", "Орк", "Тролль" };
static const char* kMonsterArts[] = {
    "    _____\n   /     \\\n  |  o o  |\n  |   _   |\n  |  \\_/  |\n   \\_____/",
    "   /\\    /\\\n  /  \\  /  \\\n |    \\/    |\n |  o    o  |\n |    __    |\n  \\________/\n",
    "    _____\n   /     \\\n  |  o o  |\n  |   _   |\n   \\_____/\n     | |\n    /| |\\\n   / | | \\\n     | |\n    /   \\\n   /     \\",
    "  /\\      /\\\n /  \\    /  \\\n/    \\  /    \\\n\\     \\/     /\n \\  o    o  /\n  \\   __   /\n  /\\  \\/  /\\\n /  \\    /  \\\n/    \\  /    \\",
    "    ______\n   /      \\\n  | o    o |\n  |   __   |\n  |  /  \\  |\n  |  \\__/  |\n   \\______/\n    /|  |\\\n   / |  | \\\n  /  |  |  \\",
    "     ______\n    /      \\\n   | o    o |\n   |   __   |\n   |  /  \\  |\n   |  \\__/  |\n    \\______/\n   /|      |\\\n  / |      | \\\n /  |      |  \\\n/   |      |   \\"
};
static const int kMonsterCount = 6;

static Weapon GenerateRandomWeapon(int level, int* outIndex = nullptr) {
    int idx = rand() % kWeaponCount;
    int dmg = 3 + rand() % (3 + level);
    if (outIndex) *outIndex = idx;
    return Weapon(kWeaponNames[idx], dmg, kWeaponArts[idx]);
}

static Monster MakeMonsterByIndex(int idx, int hp, int dmg) {
    if (idx < 0 || idx >= kMonsterCount) idx = 0;
    int dropIdx = rand() % kWeaponCount;
    int dropDmg = 3 + rand() % (3 + idx);
    Weapon drop = Weapon(kWeaponNames[dropIdx], dropDmg, kWeaponArts[dropIdx]);
    return Monster(kMonsterNames[idx], hp, dmg, drop, true, kMonsterArts[idx], idx);
}

static Monster GenerateMonster(int level) {
    int idx = rand() % kMonsterCount;
    int hp = 3 + level + rand() % (3 + level);
    int dmg = 1 + rand() % (1 + level);
    Weapon drop = GenerateRandomWeapon(level);
    return Monster(kMonsterNames[idx], hp, dmg, drop, true, kMonsterArts[idx], idx);
}

static bool RollChance(int percent) {
    return (rand() % 100) < percent;
}

static void GenerateWeaponDrop(int monsterLevel, int* outIndex, std::string* outName,
    int* outValue, std::string* outArt) {
    int idx = rand() % kWeaponCount;
    int dmg = 3 + rand() % (3 + monsterLevel);
    *outIndex = idx;
    *outName = kWeaponNames[idx];
    *outValue = dmg;
    *outArt = kWeaponArts[idx];
}

static bool GenerateBonusDrop(int monsterIdx, int* outType, std::string* outName, int* outValue) {
    int bonusChance = 0;
    switch (monsterIdx) {
    case 0: bonusChance = 30; break;
    case 1: bonusChance = 35; break;
    case 2: bonusChance = 40; break;
    case 3: bonusChance = 45; break;
    case 4: bonusChance = 50; break;
    case 5: bonusChance = 60; break;
    default: bonusChance = 30; break;
    }
    if (!RollChance(bonusChance)) return false;

    int roll = rand() % 100;
    if (roll < 40) {
        *outType = ItemPotion;
        *outName = "Зелье лечения";
        *outValue = 2 + rand() % 4;
    }
    else if (roll < 70) {
        *outType = ItemAmulet;
        *outName = "Амулет жизни";
        *outValue = 2 + rand() % 3;
    }
    else {
        *outType = ItemScroll;
        *outName = "Свиток силы";
        *outValue = 1 + rand() % 2;
    }
    return true;
}

static std::string CurrentTimeString() {
    std::time_t t = std::time(nullptr);
    std::tm ltBuf;
    localtime_s(&ltBuf, &t);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ltBuf);
    return std::string(buf);
}

static void LoadAllSaves(SaveData slots[], int slotCount) {
    for (int i = 0; i < slotCount; i++) slots[i] = SaveData();
    std::ifstream fin("saves.txt");
    if (!fin.is_open()) return;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string token;
        std::getline(ss, token, '|');
        int idx = std::atoi(token.c_str());
        if (idx < 0 || idx >= slotCount) continue;
        SaveData& s = slots[idx];
        std::getline(ss, token, '|'); s.used = (token == "1");
        std::getline(ss, s.timeStr, '|');
        std::getline(ss, s.heroName, '|');
        std::getline(ss, token, '|'); s.heroHealth = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.heroMaxHealth = std::atoi(token.c_str());
        std::getline(ss, s.weaponName, '|');
        std::getline(ss, token, '|'); s.weaponDamage = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.weaponBonusDamage = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.weaponArt = UnescapeField(token);
        std::getline(ss, token, '|'); s.floor = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.totalFloors = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.opts.health = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.opts.size = std::atoi(token.c_str());
        std::getline(ss, s.opts.diff, '|');
        std::getline(ss, token, '|'); s.inBattle = (token == "1");
        std::getline(ss, token, '|'); s.monsterIndex = std::atoi(token.c_str());
        std::getline(ss, s.monsterName, '|');
        std::getline(ss, token, '|'); s.monsterHealth = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.monsterMaxHealth = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.monsterDamage = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.monsterArt = UnescapeField(token);
        std::getline(ss, token, '|'); s.dropIndex = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.dropType = std::atoi(token.c_str());
        std::getline(ss, s.dropName, '|');
        std::getline(ss, token, '|'); s.dropValue = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.dropArt = UnescapeField(token);
        std::getline(ss, token, '|'); s.monsterSpecialUsed = (token == "1");
        std::getline(ss, token, '|'); s.poisonTurnsLeft = std::atoi(token.c_str());
        std::getline(ss, token, '|'); s.pendingBonusType = std::atoi(token.c_str());
        std::getline(ss, s.pendingBonusName, '|');
        std::getline(ss, token, '|'); s.pendingBonusValue = std::atoi(token.c_str());
    }
    fin.close();
}

static void WriteAllSaves(SaveData slots[], int slotCount) {
    std::ofstream fout("saves.txt", std::ios::trunc);
    if (!fout.is_open()) return;
    for (int i = 0; i < slotCount; i++) {
        const SaveData& s = slots[i];
        fout << i << '|'
            << (s.used ? 1 : 0) << '|'
            << s.timeStr << '|'
            << s.heroName << '|'
            << s.heroHealth << '|'
            << s.heroMaxHealth << '|'
            << s.weaponName << '|'
            << s.weaponDamage << '|'
            << s.weaponBonusDamage << '|'
            << EscapeField(s.weaponArt) << '|'
            << s.floor << '|'
            << s.totalFloors << '|'
            << s.opts.health << '|'
            << s.opts.size << '|'
            << s.opts.diff << '|'
            << (s.inBattle ? 1 : 0) << '|'
            << s.monsterIndex << '|'
            << s.monsterName << '|'
            << s.monsterHealth << '|'
            << s.monsterMaxHealth << '|'
            << s.monsterDamage << '|'
            << EscapeField(s.monsterArt) << '|'
            << s.dropIndex << '|'
            << s.dropType << '|'
            << s.dropName << '|'
            << s.dropValue << '|'
            << EscapeField(s.dropArt) << '|'
            << (s.monsterSpecialUsed ? 1 : 0) << '|'
            << s.poisonTurnsLeft << '|'
            << s.pendingBonusType << '|'
            << s.pendingBonusName << '|'
            << s.pendingBonusValue << "\n";
    }
    fout.close();
}

static void SaveSlot(int slotIndex, const SaveData& data, int slotCount) {
    SaveData* slots = new SaveData[slotCount];
    LoadAllSaves(slots, slotCount);
    if (slotIndex >= 0 && slotIndex < slotCount) {
        slots[slotIndex] = data;
        slots[slotIndex].used = true;
        slots[slotIndex].timeStr = CurrentTimeString();
        WriteAllSaves(slots, slotCount);
    }
    delete[] slots;
}

static SaveData MakeSaveFromState(Character& hero, const Options& opts,
    int floor, int totalFloors,
    const Monster* monster,
    int dropType, const std::string& dropName,
    int dropValue, const std::string& dropArt,
    int dropIndex, int poisonTurnsLeft,
    int pendingBonusType, const std::string& pendingBonusName,
    int pendingBonusValue) {
    SaveData sd;
    sd.heroName = hero.name;
    sd.heroHealth = hero.health;
    sd.heroMaxHealth = hero.maxHealth;
    sd.weaponName = hero.weapon.name;
    sd.weaponDamage = hero.weapon.damage;
    sd.weaponBonusDamage = hero.bonusDamage;
    sd.weaponArt = hero.weapon.art;
    sd.floor = floor;
    sd.totalFloors = totalFloors;
    sd.opts = opts;
    sd.poisonTurnsLeft = poisonTurnsLeft;
    sd.pendingBonusType = pendingBonusType;
    sd.pendingBonusName = pendingBonusName;
    sd.pendingBonusValue = pendingBonusValue;
    if (monster) {
        sd.inBattle = true;
        sd.monsterIndex = monster->index;
        sd.monsterName = monster->name;
        sd.monsterHealth = monster->health;
        sd.monsterMaxHealth = monster->maxHealth;
        sd.monsterDamage = monster->damage;
        sd.monsterArt = monster->art;
        sd.monsterSpecialUsed = monster->specialUsed;
        sd.dropIndex = dropIndex;
        sd.dropType = dropType;
        sd.dropName = dropName;
        sd.dropValue = dropValue;
        sd.dropArt = dropArt;
    }
    else {
        sd.inBattle = false;
    }
    return sd;
}

static int HealPercentByDiff(const std::string& diff) {
    if (diff == "Легко" || diff == "Легкая") return 50;
    if (diff == "Нормально") return 25;
    if (diff == "Тяжело") return 10;
    return 25;
}

static void MonsterSpecialAction(Monster& monster, Character& hero,
    bool& skipMonsterTurn, int& poisonTurnsLeft) {
    skipMonsterTurn = false;
    if (monster.specialUsed) return;
    switch (monster.typeIndex) {
    case 0:
        if (rand() % 100 < 45) {
            std::cout << monster.name << " выделяет слизь! Ваша следующая атака ослаблена (-1 урон).\n";
            hero.TakeDamage(0);
            hero.bonusDamage -= 1;
            monster.specialUsed = true;
        }
        break;
    case 1:
        if (rand() % 100 < 55) {
            std::cout << monster.name << " крадёт ваше оружие и ослабляет его!\n";
            if (hero.weapon.damage > 1) hero.weapon.damage -= 1;
            monster.specialUsed = true;
        }
        break;
    case 2:
        if (rand() % 100 < 40) {
            std::cout << monster.name << " пугает вас! Вы пропускаете ход.\n";
            skipMonsterTurn = true;
            monster.specialUsed = true;
        }
        break;
    case 3:
        if (rand() % 100 < 60) {
            poisonTurnsLeft = 3;
            std::cout << monster.name << " кусает вас ядом! Яд будет действовать 3 хода.\n";
            monster.specialUsed = true;
        }
        break;
    case 4:
        if (rand() % 100 < 50) {
            std::cout << monster.name << " впадает в ярость! Урон увеличен на 2.\n";
            monster.damage += 2;
            monster.specialUsed = true;
        }
        break;
    case 5:
        if (rand() % 100 < 60) {
            int regen = 2;
            monster.health += regen;
            if (monster.health > monster.maxHealth) monster.health = monster.maxHealth;
            std::cout << monster.name << " регенерирует " << regen << " HP!\n";
            monster.specialUsed = true;
        }
        break;
    }
}

static void RunDungeon(Options opts, Character hero, int startFloor, int totalFloors,
    const SaveData* resume = nullptr) {
    int diffMod = 1;
    if (opts.diff == "Легко" || opts.diff == "Легкая") diffMod = 1;
    else if (opts.diff == "Нормально") diffMod = 2;
    else if (opts.diff == "Тяжело") diffMod = 3;

    int healPercent = HealPercentByDiff(opts.diff);

    srand((unsigned)time(nullptr));

    bool resumedBattle = (resume && resume->inBattle);
    int poisonTurnsLeft = 0;

    for (int floor = startFloor; floor <= totalFloors; floor++) {
        system("cls");
        std::cout << "=== Этаж " << floor << " из " << totalFloors << " ===\n";

        Monster monster;
        int dropType = ItemWeapon;
        std::string dropName;
        int dropValue = 0;
        std::string dropArt;
        int dropIndex = -1;
        int pendingBonusType = ItemNone;
        std::string pendingBonusName;
        int pendingBonusValue = 0;

        if (resumedBattle && resume->floor == floor) {
            monster = MakeMonsterByIndex(resume->monsterIndex,
                resume->monsterMaxHealth,
                resume->monsterDamage);
            monster.health = resume->monsterHealth;
            monster.specialUsed = resume->monsterSpecialUsed;
            dropType = resume->dropType;
            dropName = resume->dropName;
            dropValue = resume->dropValue;
            dropArt = resume->dropArt;
            dropIndex = resume->dropIndex;
            poisonTurnsLeft = resume->poisonTurnsLeft;
            pendingBonusType = resume->pendingBonusType;
            pendingBonusName = resume->pendingBonusName;
            pendingBonusValue = resume->pendingBonusValue;
            resumedBattle = false;
        }
        else {
            monster = GenerateMonster(floor * diffMod);
            GenerateWeaponDrop(floor * diffMod, &dropIndex, &dropName, &dropValue, &dropArt);
            int bonusType = ItemNone;
            std::string bonusName;
            int bonusValue = 0;
            if (GenerateBonusDrop(monster.index, &bonusType, &bonusName, &bonusValue)) {
                pendingBonusType = bonusType;
                pendingBonusName = bonusName;
                pendingBonusValue = bonusValue;
            }
            else {
                pendingBonusType = ItemNone;
            }
            poisonTurnsLeft = 0;
        }

        std::cout << monster.art << "\n";
        std::cout << "На вас напал " << monster.name << "! (HP: " << monster.health
            << "/" << monster.maxHealth << ", урон: " << monster.damage << ")\n";

        while (hero.IsAlive() && monster.IsAlive()) {
            std::cout << "\nВаше HP: " << hero.health << "/" << hero.maxHealth;
            std::cout << " | Оружие: " << hero.weapon.name << " (урон: " << hero.Attack() << ")\n";
            std::cout << monster.name << " HP: " << monster.health << "/" << monster.maxHealth << "\n";
            if (poisonTurnsLeft > 0) {
                std::cout << "Яд действует ещё " << poisonTurnsLeft << " ход(а).\n";
            }
            std::cout << "1. Атаковать\n2. Защищаться (пропустить ход)\n3. Сохранить игру\n4. Выйти в главное меню\n";
            std::cout << "Выберите действие: ";
            int act = ReadInt();

            if (act == 4) {
                std::cout << "Выход в главное меню...\n";
                std::cout << "Нажмите Enter для продолжения ";
                std::cin.ignore();
                std::cin.get();
                return;
            }

            if (act == 3) {
                SaveData sd = MakeSaveFromState(hero, opts, floor, totalFloors, &monster,
                    dropType, dropName, dropValue, dropArt,
                    dropIndex, poisonTurnsLeft,
                    pendingBonusType, pendingBonusName,
                    pendingBonusValue);
                std::cout << "Выберите слот для сохранения (1-3, 0 - отмена): ";
                int slot = ReadInt();
                if (slot == 0) {
                    std::cout << "Сохранение отменено.\n";
                }
                else if (slot >= 1 && slot <= 3) {
                    SaveSlot(slot - 1, sd, 3);
                    std::cout << "Игра сохранена в слот " << slot << ".\n";
                }
                else {
                    std::cout << "Ошибка: неверный слот. Допустимо от 1 до 3.\n";
                }
                std::cout << "Нажмите Enter для продолжения ";
                std::cin.ignore();
                std::cin.get();
                continue;
            }

            if (act != 1 && act != 2) {
                std::cout << "Ошибка: неверное действие. Введите 1, 2, 3 или 4.\n";
                continue;
            }

            bool defending = false;
            if (act == 1) {
                int dmg = hero.Attack();
                if (dmg < 0) dmg = 0;
                monster.TakeDamage(dmg);
                std::cout << "Вы нанесли " << dmg << " урона!\n";
            }
            else {
                defending = true;
                std::cout << "Вы защищаетесь.\n";
            }
            if (!monster.IsAlive()) {
                std::cout << monster.name << " побеждён!\n";
                break;
            }

            bool skipMonsterTurn = false;
            MonsterSpecialAction(monster, hero, skipMonsterTurn, poisonTurnsLeft);

            if (!hero.IsAlive()) {
                std::cout << "\nВы погибли... Игра окончена.\n";
                std::cout << "\nНажмите Enter, чтобы вернуться в главное меню ";
                std::cin.ignore();
                std::cin.get();
                return;
            }

            if (poisonTurnsLeft > 0) {
                int poisonDmg = 1;
                hero.TakeDamage(poisonDmg);
                poisonTurnsLeft--;
                std::cout << "Яд наносит вам " << poisonDmg << " урона! (осталось ходов: "
                    << poisonTurnsLeft << ")\n";
                if (!hero.IsAlive()) break;
            }

            if (skipMonsterTurn) {
                std::cout << monster.name << " пропускает ход!\n";
                continue;
            }

            int incoming = monster.damage;
            if (defending) {
                incoming /= 2;
                std::cout << "Защита снизила урон вдвое. ";
            }
            hero.TakeDamage(incoming);
            std::cout << monster.name << " наносит вам " << incoming << " урона!\n";
        }

        if (!hero.IsAlive()) {
            std::cout << "\nВы погибли... Игра окончена.\n";
            break;
        }

        poisonTurnsLeft = 0;

        if (healPercent > 0) {
            int before = hero.health;
            int healAmount = hero.maxHealth * healPercent / 100;
            hero.Heal(healAmount);
            int healed = hero.health - before;
            if (healed > 0) {
                std::cout << "\nОтдых после боя: восстановлено " << healed
                    << " HP (" << healPercent << "% от максимума).\n";
            }
        }

        std::cout << "\n--- Оружие ---\n";
        std::cout << dropArt << "\n";
        std::cout << dropName << " (урон: " << dropValue << ")\n";
        std::cout << "1. Экипировать\n2. Оставить текущее\n3. Выйти в главное меню\n";
        int pick = ReadIntInRange(1, 3, "Выбор: ");
        if (pick == 3) {
            std::cout << "Выход в главное меню...\n";
            std::cout << "Нажмите Enter для продолжения ";
            std::cin.ignore();
            std::cin.get();
            return;
        }
        if (pick == 1) hero.EquipWeapon(Weapon(dropName, dropValue, dropArt));

        if (pendingBonusType != ItemNone) {
            std::cout << "\n--- Дополнительный предмет ---\n";
            if (pendingBonusType == ItemPotion) {
                std::cout << "Зелье лечения (лечение: " << pendingBonusValue << " HP)\n";
                std::cout << "1. Использовать\n2. Оставить\n3. Выйти в главное меню\n";
                int pick2 = ReadIntInRange(1, 3, "Выбор: ");
                if (pick2 == 3) {
                    std::cout << "Выход в главное меню...\n";
                    std::cout << "Нажмите Enter для продолжения ";
                    std::cin.ignore();
                    std::cin.get();
                    return;
                }
                if (pick2 == 1) {
                    hero.Heal(pendingBonusValue);
                    std::cout << "Вы выпили зелье и восстановили " << pendingBonusValue << " HP.\n";
                }
            }
            else if (pendingBonusType == ItemAmulet) {
                std::cout << "Амулет жизни (макс. HP +" << pendingBonusValue << ")\n";
                std::cout << "1. Надеть\n2. Оставить\n3. Выйти в главное меню\n";
                int pick2 = ReadIntInRange(1, 3, "Выбор: ");
                if (pick2 == 3) {
                    std::cout << "Выход в главное меню...\n";
                    std::cout << "Нажмите Enter для продолжения ";
                    std::cin.ignore();
                    std::cin.get();
                    return;
                }
                if (pick2 == 1) {
                    hero.AddMaxHealth(pendingBonusValue);
                    std::cout << "Вы надели амулет. Макс. HP: " << hero.maxHealth << "\n";
                }
            }
            else if (pendingBonusType == ItemScroll) {
                std::cout << "Свиток силы (бонус к урону +" << pendingBonusValue << ")\n";
                std::cout << "1. Прочитать\n2. Оставить\n3. Выйти в главное меню\n";
                int pick2 = ReadIntInRange(1, 3, "Выбор: ");
                if (pick2 == 3) {
                    std::cout << "Выход в главное меню...\n";
                    std::cout << "Нажмите Enter для продолжения ";
                    std::cin.ignore();
                    std::cin.get();
                    return;
                }
                if (pick2 == 1) {
                    hero.AddBonusDamage(pendingBonusValue);
                    std::cout << "Вы прочли свиток. Бонус к урону: +" << hero.bonusDamage << "\n";
                }
            }
        }

        if (floor < totalFloors) {
            SaveData sd = MakeSaveFromState(hero, opts, floor + 1, totalFloors, nullptr,
                ItemNone, "", 0, "", -1, 0, ItemNone, "", 0);
            SaveSlot(0, sd, 3);
        }

        std::cout << "\nНажмите Enter для продолжения ";
        std::cin.ignore();
        std::cin.get();
    }

    if (hero.IsAlive()) {
        std::cout << "\nПоздравляем! Вы прошли подземелье!\n";
        std::cout << "Нажмите Enter для продолжения ";
        std::cin.ignore();
        std::cin.get();
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int choice;
    std::ifstream opFile("options.txt");
    if (opFile.is_open()) opFile.close();
    else {
        std::ofstream opFile("options.txt");
        if (opFile.is_open()) {
            opFile << 10 << " " << 50 << " Легко\n";
            opFile.close();
        }
        else std::cerr << "Ошибка при создании файла";
    }
    while (true) {
        system("cls");
        DisplayMenu();
        choice = ReadInt();
        if (choice >= 1 && choice <= 3) {
            menuFunctions[choice - 1]();
        }
        else if (choice == AboutOp) About();
        else if (choice == ExitOp) break;
        else {
            std::cout << "Ошибка: неверный пункт меню. Введите от 1 до 5.\n";
            std::cout << "Нажмите Enter для продолжения ";
            std::cin.ignore();
            std::cin.get();
        }
    }
    return 0;
}

void DisplayMenu() {
    std::cout << "Приключение в подземелье \n";
    std::cout << "1. Начать игру\n";
    std::cout << "2. Загрузить игру\n";
    std::cout << "3. Настройки\n";
    std::cout << "4. О программе\n";
    std::cout << "5. Выход\n";
    std::cout << "Выберите пункт: ";
}

void GameStart() {
    system("cls");
    std::cout << "Начало игры\n";

    Options opts;
    opts.health = 10;
    opts.size = 50;
    opts.diff = "Легко";

    std::ifstream inFile("options.txt");
    if (inFile.is_open()) {
        inFile >> opts.health >> opts.size >> opts.diff;
        inFile.close();
    }

    Character hero("Герой", opts.health);
    std::cout << "Вы спускаетесь в подземелье...\n";
    std::cout << "Здоровье: " << hero.health << "/" << hero.maxHealth << "\n";
    std::cout << "Сложность: " << opts.diff << "\n";
    std::cout << "Размер подземелья: " << opts.size << "\n";
    std::cout << "Нажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();

    int totalFloors = opts.size / 25;
    if (totalFloors < 1) totalFloors = 1;

    RunDungeon(opts, hero, 1, totalFloors);
}

void GameLoad() {
    system("cls");
    std::cout << "=== Загрузка игры ===\n";

    SaveData slots[3];
    LoadAllSaves(slots, 3);

    for (int i = 0; i < 3; i++) {
        std::cout << "Слот " << (i + 1) << ": ";
        if (slots[i].used) {
            std::cout << "Герой " << slots[i].heroName
                << " | HP " << slots[i].heroHealth << "/" << slots[i].heroMaxHealth
                << " | Оружие " << slots[i].weaponName << " (" << slots[i].weaponDamage
                << " +" << slots[i].weaponBonusDamage << ")"
                << " | Этаж " << slots[i].floor << "/" << slots[i].totalFloors
                << " | Сложность " << slots[i].opts.diff;
            if (slots[i].inBattle) {
                std::cout << " | В бою с " << slots[i].monsterName
                    << " (HP " << slots[i].monsterHealth << "/" << slots[i].monsterMaxHealth << ")";
            }
            std::cout << " | Сохранено: " << slots[i].timeStr << "\n";
        }
        else {
            std::cout << "Пусто\n";
        }
    }
    std::cout << "4. Назад\n";
    std::cout << "Выберите слот для загрузки: ";
    int slot = ReadInt();

    if (slot == 4) return;
    if (slot < 1 || slot > 3) {
        std::cout << "Ошибка: неверный слот. Допустимо 1-3 или 4 (назад).\n";
        std::cout << "Нажмите Enter для продолжения ";
        std::cin.ignore();
        std::cin.get();
        return;
    }
    int idx = slot - 1;
    if (!slots[idx].used) {
        std::cout << "Слот пуст.\n";
        std::cout << "Нажмите Enter для продолжения ";
        std::cin.ignore();
        std::cin.get();
        return;
    }

    Options opts = slots[idx].opts;
    Character hero(slots[idx].heroName, slots[idx].heroMaxHealth);
    hero.health = slots[idx].heroHealth;
    hero.bonusDamage = slots[idx].weaponBonusDamage;
    hero.weapon = Weapon(slots[idx].weaponName, slots[idx].weaponDamage, slots[idx].weaponArt);

    std::cout << "Игра загружена. Нажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();

    RunDungeon(opts, hero, slots[idx].floor, slots[idx].totalFloors, &slots[idx]);
}

void Settings() {
    int choice, NewVal, count = 1;
    std::stack<Options> optStack;
    std::ifstream file("options.txt");
    if (!file.is_open()) std::cerr << "Ошибка при открытии файла";
    Options cur;
    file >> cur.health >> cur.size >> cur.diff;
    optStack.push(cur);
    file.close();
    while (true) {
        system("cls");
        Options val = optStack.top();
        std::cout << "Текущие Параметры:\n";
        std::cout << "Здоровье: ";
        std::cout << val.health << std::endl;
        std::cout << "Размер Подземелья: ";
        switch (val.size) {
        case 50:
            std::cout << "Маленькое\n";
            break;
        case 100:
            std::cout << "Среднее\n";
            break;
        case 225:
            std::cout << "Большое\n";
            break;
        }
        std::cout << "Сложность: ";
        std::cout << val.diff << std::endl;
        std::cout << "Восстановление HP после боя: " << HealPercentByDiff(val.diff) << "%\n";
        std::cout << "1. Изменить Параметры\n2. Вернуть Предыдущие параметры\n3. Назад\n";
        choice = ReadInt();
        if (choice < 1 || choice > 3) {
            std::cout << "Ошибка: неверный пункт. Введите от 1 до 3.\n";
            system("pause");
            continue;
        }
        if (choice == 1) {
            system("cls");
            std::cout << "Введите Новые Параметры\nЗдоровье (Значение от 1 до 10): ";
            NewVal = ReadInt();
            if (NewVal < 1 || NewVal > 10) {
                std::cout << "Ошибка: неверное значение. Допустимо от 1 до 10.\n";
                system("pause");
                continue;
            }
            cur.health = NewVal;

            std::cout << "Размер Подземелья (1.Маленькое 2.Среднее 3.Большое): ";
            NewVal = ReadInt();
            if (NewVal < 1 || NewVal > 3) {
                std::cout << "Ошибка: неверное значение. Допустимо от 1 до 3.\n";
                system("pause");
                continue;
            }
            switch (NewVal) {
            case 1: cur.size = 50; break;
            case 2: cur.size = 100; break;
            case 3: cur.size = 225; break;
            }

            std::cout << "Сложность (1.Легко 2.Нормально 3.Тяжело): ";
            NewVal = ReadInt();
            if (NewVal < 1 || NewVal > 3) {
                std::cout << "Ошибка: неверное значение. Допустимо от 1 до 3.\n";
                system("pause");
                continue;
            }
            switch (NewVal) {
            case 1: cur.diff = "Легко"; break;
            case 2: cur.diff = "Нормально"; break;
            case 3: cur.diff = "Тяжело"; break;
            }

            optStack.push(cur);
            count++;
            std::ofstream file("options.txt");
            file << cur.health << " " << cur.size << " " << cur.diff;
            file.close();
        }
        else if (choice == 2) {
            system("cls");
            if (count > 1) {
                optStack.pop();
                std::cout << "Значения изменены на предыдущие\n";
                count--;
                val = optStack.top();
                std::ofstream file("options.txt");
                file << val.health << " " << val.size << " " << val.diff;
                file.close();
            }
            else std::cout << "Предыдущие значения не найдены\n";
        }
        else if (choice == 3) {
            break;
        }
        system("pause");
    }
    return;
}

void About() {
    system("cls");
    std::cout << "Версия игры 1.0\nВыполнил Бородин Кирилл";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
}