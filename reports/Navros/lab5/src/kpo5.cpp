#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <clocale>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

using namespace std;

//Менюшки для выборов
enum MainMenu{
    Start_Game = 1,
    Load_Game,
    Settings,
    About,
    Exit
};
enum OptionsMenu {
    Difficulty = 1,
    Size,
    Health,
    All_Settings,
    Restore_Size,
    Go_Back
};
enum DifficultyOptions {
    Easy = 1,
    Medium,
    Hard,
    VeryHard
};
enum BattleActions {
    Attack = 1,
    ChangeWeapon,
    DrinkPotion,
    Approach                               // сблизиться (пропуск хода, нужно только против дальнобойного врага)
};
enum MoveResult {
    Moved,
    Wall,
    UnknownKey
};
//Что может встретиться на клетке. Порядок важен: по нему идут столбцы таблицы SPAWN_TABLE
enum Encounter {
    EncGoblin,
    EncSkeleton,
    EncTroll,
    EncOrc,
    EncVampire,
    EncChest
};

//Чистит строки когда там возникают ошибки
void Cleaner ()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

struct GameSettings
{
    int size;
    int health;
    string difficulty;
};

//Константы для игры
string difficulty = "Легко";
int fieldsize = 64;
int health = 3;
const float version = 5.0;
const string creator = "Наврось Глеб";
stack<GameSettings> history;
string settingsFile = "settings.txt";
string saveFile = "savegame.txt";

//Границы значений, которые вводятся с клавиатуры
const int MIN_SIDE = 5;
const int MAX_SIDE = 20;
const int MIN_HEALTH = 1;
const int MAX_HEALTH = 30;

//Интервал автосохранения в секундах (30 минут = 1800 секунд)
const int AUTO_SAVE_INTERVAL = 30 * 60;

//Ближняя зона: клетки, у которых x <= 2 и y <= 2 (старт всегда в 0.0)
const int NEAR_ZONE = 2;
//Диапазоны случайного числа: в ближней зоне 0-5, в дальней 0-19
const int NEAR_ROLL_RANGE = 6;
const int FAR_ROLL_RANGE = 20;
//Размер угла карты, где может появиться выход (3x3, а на очень сложной 2x2)
const int EXIT_ZONE_NORMAL = 3;
const int EXIT_ZONE_VERYHARD = 2;
//Сколько здоровья восстанавливает зелье
const int POTION_HEAL = 5;

//Сколько чисел из диапазона 0-19 отдано каждому виду встречи в дальней зоне.
//Строки - сложность (Легко ... Очень сложно), столбцы - Encounter (гоблин, скелет, тролль, орк, вампир, сундук).
//Чем выше сложность, тем уже диапазон гоблина и шире диапазоны сильных монстров. Сумма в каждой строке = 20
const int SPAWN_TABLE[4][6] = {
    { 8, 5, 2, 1, 1, 3 },   // Легко
    { 6, 5, 3, 2, 1, 3 },   // Средне
    { 4, 5, 4, 3, 2, 2 },   // Сложно
    { 2, 4, 5, 4, 3, 2 }    // Очень сложно
};

//Преобразует размер из полного значения в размеры сетки карты
string sizeConverter (int fieldsize)
{
    int conv = static_cast<int> (sqrt(fieldsize));
    return to_string(conv) + "x" + to_string(conv);
}
//Преобразование сложности в понятные слова
string diffConverter (int diff)
{
    switch (diff)
    {
    case Easy: return("Легко");
    case Medium: return("Средне");
    case Hard: return("Сложно");
    case VeryHard: return("Очень Сложно");
    default: return("Неизвестно");
    }
}
//Обратное преобразование: название сложности в номер 1-4 (если название неизвестно, считаем что Легко)
int diffIndex (const string& diff)
{
    for (int i = Easy; i <= VeryHard; i++)
        if (diffConverter(i) == diff) return i;
    return Easy;
}
//Запись информации в файл с полной перезаписью его
void saveHistory()
{
    stack<GameSettings> copy = history;      // делаем всякое с копией чтобы недай бог не повредить оригинал
    vector<GameSettings> items;
    while (!copy.empty()) { items.push_back(copy.top()); copy.pop(); }

    ofstream out(settingsFile);
    for (int i = static_cast<int>(items.size()) - 1; i >= 0; i--)   // от старого к новому
        out << items[i].size << " " << items[i].health << " " << items[i].difficulty << "\n";
}
// Проверка что прочитанная из файла запись имеет допустимые значения
bool settingsAreValid(const GameSettings& s)
{
    if (s.size < MIN_SIDE * MIN_SIDE || s.size > MAX_SIDE * MAX_SIDE) return false;
    int side = static_cast<int>(round(sqrt(s.size)));
    if (side * side != s.size) return false;                       // размер должен быть квадратом
    if (s.health < MIN_HEALTH || s.health > MAX_HEALTH) return false;
    for (int i = Easy; i <= VeryHard; i++)
        if (diffConverter(i) == s.difficulty) return true;
    return false;                                                  // неизвестная сложность
}
// Чтение файла в стек
void loadHistory()
{
    ifstream in(settingsFile);
    string line;
    while (getline(in, line))
    {
        istringstream iss(line);
        GameSettings s;
        if (!(iss >> s.size >> s.health)) continue;     // если в начале не 2 числа значит чтото сломалось, скип
        getline(iss >> ws, s.difficulty);               // забираем нашу настройку сложности через getline тк у нас есть сложность состоящая из 2 слов
        if (!s.difficulty.empty() && s.difficulty.back() == '\r') s.difficulty.pop_back();   // на случай файла с CRLF
        if (!settingsAreValid(s)) continue;             // значения вне допустимых границ значит файл испорчен, скип
        history.push(s);
    }
}
// Кладёт текущие настройки в стек и файл, если они отличаются от верхней записи
void commitSettings()
{
    if (!history.empty())
    {
        const GameSettings& t = history.top();
        if (t.size == fieldsize && t.health == health && t.difficulty == difficulty)
            return;
    }
    history.push({fieldsize, health, difficulty});
    saveHistory();
}

//Оружие: название и урон (по умолчанию кулаки)
class Weapon
{
    string name;
    int damage;
public:
    Weapon(string n = "Кулаки", int d = 2) : name(n), damage(d) {}
    string getName() const { return name; }
    int getDamage() const { return damage; }
};

//Монстр: здоровье, урон и оружие, которое выпадает после победы над ним.
//Особенности по виду: скелет дальнобойный, тролль бьёт раз в два хода, вампир лечится от своих ударов
class Monster
{
    string name;
    int health;
    int maxHealth;
    int damage;
    Weapon drop;
    string art;                            // ASCII-арт, который показывается при появлении монстра
    Encounter kind;
public:
    Monster(string n, int h, int d, Weapon w, string a = "", Encounter k = EncGoblin)
        : name(n), health(h), maxHealth(h), damage(d), drop(w), art(a), kind(k) {}
    string getArt() const { return art; }
    Encounter getKind() const { return kind; }
    bool isAlive() const { return health > 0; }
    void takeDamage(int dmg) { health -= dmg; if (health < 0) health = 0; }
    void heal(int amount) { health += amount; if (health > maxHealth) health = maxHealth; }
    int attack() const { return damage; }
    int getHealth() const { return health; }
    string getName() const { return name; }
    Weapon getDrop() const { return drop; }

    bool isRanged() const { return kind == EncSkeleton; }
    bool isDrainer() const { return kind == EncVampire; }
    //Урон монстра в указанном раунде боя (раунды считаются с 1). Тролль в нечётные раунды замахивается и не бьёт
    int attackInRound(int round) const
    {
        if (kind == EncTroll && round % 2 == 1) return 0;
        return damage;
    }
};

//Персонаж: здоровье, позиция на карте, оружие, броня, зелья, инвентарь и счётчик побед
class Character
{
    int health;
    int maxHealth;
    int x = 0;
    int y = 0;
    int wins = 0;
    int potions = 0;
    string armorName = "Нет";
    int armorValue = 0;
    Weapon weapon;
    vector<Weapon> inventory;
public:
    Character(int h) : health(h), maxHealth(h) {}
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void setMaxHealth(int m) { maxHealth = m; }
    bool isAlive() const { return health > 0; }
    void takeDamage(int dmg) { health -= dmg; if (health < 0) health = 0; }
    //Удар по персонажу с учётом брони (урон уменьшается на значение брони, но не меньше 1). Возвращает реальный урон
    int hit(int dmg)
    {
        int real = max(1, dmg - armorValue);
        takeDamage(real);
        return real;
    }
    int attack() const { return weapon.getDamage(); }
    string getWeaponName() const { return weapon.getName(); }
    int getWeaponDamage() const { return weapon.getDamage(); }
    void setWeapon(const Weapon& w) { weapon = w; }

    int getX() const { return x; }
    int getY() const { return y; }
    void moveTo(int newX, int newY) { x = newX; y = newY; }

    int getWins() const { return wins; }
    void setWins(int w) { wins = w; }
    void addWin() { wins++; }

    int getPotions() const { return potions; }
    void setPotions(int p) { potions = p; }
    void addPotion() { potions++; }
    //Выпить зелье. false если зелий нет или здоровье уже полное (тогда зелье не тратится)
    bool drinkPotion(int& healed)
    {
        healed = 0;
        if (potions <= 0 || health >= maxHealth) return false;
        int before = health;
        health = min(maxHealth, health + POTION_HEAL);
        potions--;
        healed = health - before;
        return true;
    }

    string getArmorName() const { return armorName; }
    int getArmorValue() const { return armorValue; }
    void setArmor(const string& n, int v) { armorName = n; armorValue = v; }
    //Надеть броню, только если она лучше текущей. Возвращает true если надели
    bool wearArmor(const string& n, int v)
    {
        if (v <= armorValue) return false;
        setArmor(n, v);
        return true;
    }

    void pickUp(const Weapon& w) { inventory.push_back(w); }
    int inventorySize() const { return static_cast<int>(inventory.size()); }
    const vector<Weapon>& getInventory() const { return inventory; }
    bool equip(int index)                  // false если оружия с таким номером нет
    {
        if (index < 0 || index >= inventorySize()) return false;
        swap(weapon, inventory[index]);    // старое оружие уходит в инвентарь на то же место
        return true;
    }
};

//Карта подземелья: какие клетки уже посещены и где спрятан выход
class Dungeon
{
    int side;
    vector<vector<bool>> visited;
    int exitX = -1;
    int exitY = -1;
public:
    Dungeon(int s) : side(s), visited(s, vector<bool>(s, false)) {}
    int getSide() const { return side; }
    bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < side && y < side; }
    bool isVisited(int x, int y) const { return visited[y][x]; }
    void visit(int x, int y) { visited[y][x] = true; }

    //Выход всегда в дальнем углу карты, в квадрате zone x zone (старт в левом верхнем углу)
    void placeExit(int zone)
    {
        exitX = side - 1 - rand() % zone;
        exitY = side - 1 - rand() % zone;
    }
    void setExit(int x, int y) { exitX = x; exitY = y; }
    int getExitX() const { return exitX; }
    int getExitY() const { return exitY; }
    bool isExit(int x, int y) const { return x == exitX && y == exitY; }

    void draw(const Character& c) const    // @ - персонаж, x - посещено, o - не посещено
    {
        for (int y = 0; y < side; y++)
        {
            for (int x = 0; x < side; x++)
            {
                if (x == c.getX() && y == c.getY()) cout << '@';
                else if (visited[y][x]) cout << 'x';
                else cout << 'o';
                cout << ' ';
            }
            cout << '\n';
        }
    }
};

//Сохраняет текущее состояние игры в файл savegame.txt
bool saveGame(const Dungeon& dungeon, const Character& player, const string& gameDifficulty)
{
    ofstream out(saveFile);
    if (!out.is_open()) return false;

    out << gameDifficulty << "\n";
    out << dungeon.getSide() << "\n";
    out << player.getHealth() << " " << player.getMaxHealth() << " " << player.getX() << " " << player.getY()
        << " " << player.getWins() << " " << player.getPotions() << "\n";
    out << player.getWeaponName() << "\n";
    out << player.getWeaponDamage() << "\n";
    out << player.getArmorName() << "\n";
    out << player.getArmorValue() << "\n";

    const vector<Weapon>& inv = player.getInventory();
    out << inv.size() << "\n";
    for (int i = 0; i < static_cast<int>(inv.size()); i++)
    {
        out << inv[i].getName() << "\n";
        out << inv[i].getDamage() << "\n";
    }

    out << dungeon.getExitX() << " " << dungeon.getExitY() << "\n";

    int side = dungeon.getSide();
    for (int y = 0; y < side; y++)
    {
        for (int x = 0; x < side; x++)
        {
            out << (dungeon.isVisited(x, y) ? 1 : 0);
            if (x + 1 < side) out << " ";
        }
        out << "\n";
    }
    return !out.fail();
}

//Загружает сохранённую игру из файла savegame.txt
bool loadGame(Dungeon& dungeon, Character& player, string& gameDifficulty)
{
    ifstream in(saveFile);
    if (!in.is_open()) return false;

    string diff;
    if (!getline(in, diff)) return false;
    if (!diff.empty() && diff.back() == '\r') diff.pop_back();

    bool validDiff = false;
    for (int i = Easy; i <= VeryHard; i++)
        if (diffConverter(i) == diff) validDiff = true;
    if (!validDiff) return false;

    int side, pHealth, pMax, px, py, pWins, pPotions;
    if (!(in >> side)) return false;
    if (side < MIN_SIDE || side > MAX_SIDE) return false;
    if (!(in >> pHealth >> pMax >> px >> py >> pWins >> pPotions)) return false;
    if (pMax < MIN_HEALTH || pMax > MAX_HEALTH) return false;
    if (pHealth <= 0 || pHealth > pMax) return false;
    if (px < 0 || px >= side || py < 0 || py >= side) return false;
    if (pWins < 0 || pPotions < 0) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    string wName;
    int wDamage;
    if (!getline(in, wName)) return false;
    if (!wName.empty() && wName.back() == '\r') wName.pop_back();
    if (!(in >> wDamage) || wDamage <= 0) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    string aName;
    int aValue;
    if (!getline(in, aName)) return false;
    if (!aName.empty() && aName.back() == '\r') aName.pop_back();
    if (!(in >> aValue) || aValue < 0) return false;

    int invSize;
    if (!(in >> invSize) || invSize < 0) return false;
    in.ignore(numeric_limits<streamsize>::max(), '\n');

    Dungeon loadedDungeon(side);
    Character loadedPlayer(pHealth);
    loadedPlayer.setMaxHealth(pMax);
    loadedPlayer.moveTo(px, py);
    loadedPlayer.setWins(pWins);
    loadedPlayer.setPotions(pPotions);
    loadedPlayer.setArmor(aName, aValue);
    loadedPlayer.setWeapon(Weapon(wName, wDamage));

    for (int i = 0; i < invSize; i++)
    {
        string itemName;
        int itemDamage;
        if (!getline(in, itemName)) return false;
        if (!itemName.empty() && itemName.back() == '\r') itemName.pop_back();
        if (!(in >> itemDamage) || itemDamage <= 0) return false;
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        loadedPlayer.pickUp(Weapon(itemName, itemDamage));
    }

    int ex, ey;
    if (!(in >> ex >> ey)) return false;
    if (!loadedDungeon.inside(ex, ey)) return false;
    loadedDungeon.setExit(ex, ey);

    for (int y = 0; y < side; y++)
    {
        for (int x = 0; x < side; x++)
        {
            int cell;
            if (!(in >> cell)) return false;
            if (cell == 1) loadedDungeon.visit(x, y);
            else if (cell != 0) return false;
        }
    }

    dungeon = loadedDungeon;
    player = loadedPlayer;
    gameDifficulty = diff;
    return true;
}

//ASCII-арты монстров и сундука: только головы-иконки (сырые строки, чтобы не экранировать символ \)
const string GOBLIN_ART = R"ART(
  /\       /\
 /  \_____/  \
 |  o     o  |
 \    \_/    /
  \  \vvv/  /
   '-------'
)ART";

const string SKELETON_ART = R"ART(
    .-"""""-.
   /         \
  |  (O) (O)  |
   \    ^    /
    | |||||| |
    '-------'
)ART";

const string TROLL_ART = R"ART(
   _.-"""""-._
  /  _     _  \
 |  (o)   (o)  |
 |     ___     |
  \   \_^_/   /
   '-._____.-'
)ART";

const string ORC_ART = R"ART(
   .-"""""""-.
  /  \     /  \
 |  (>)   (<)  |
 |      ^      |
 | \_/VVVVV\_/ |
  \  '-----'  /
   '-._____.-'
)ART";

const string VAMPIRE_ART = R"ART(
  |\         /|
  | \_.---._/ |
  |  (o) (o)  |
   \    ^    /
    \  \_/  /
     \ V V /
      '---'
)ART";

const string CHEST_ART = R"ART(
   .-----------.
   |  _______  |
   |-|_______|-|
   |    [ ]    |
   '-----------'
)ART";

//Создаёт монстра нужного вида
Monster makeMonster(Encounter kind)
{
    switch (kind)
    {
    case EncGoblin:   return Monster("Гоблин", 4, 1, Weapon("Ржавый кинжал", 3), GOBLIN_ART, kind);
    case EncSkeleton: return Monster("Скелет", 6, 2, Weapon("Костяной меч", 4), SKELETON_ART, kind);
    case EncTroll:    return Monster("Тролль", 10, 5, Weapon("Дубина тролля", 6), TROLL_ART, kind);
    case EncOrc:      return Monster("Орк", 8, 3, Weapon("Секира орка", 7), ORC_ART, kind);
    case EncVampire:  return Monster("Вампир", 7, 2, Weapon("Клык вампира", 5), VAMPIRE_ART, kind);
    default:          return Monster("Гоблин", 4, 1, Weapon("Ржавый кинжал", 3), GOBLIN_ART, EncGoblin);
    }
}

//Диапазон случайного числа для клетки: 0-5 в ближней зоне, 0-19 в дальней
int rollRange(bool nearStart)
{
    return nearStart ? NEAR_ROLL_RANGE : FAR_ROLL_RANGE;
}

//Ближняя ли это зона (квадрат от стартовой клетки 0.0 до 2.2)
bool isNearStart(int x, int y)
{
    return x <= NEAR_ZONE && y <= NEAR_ZONE;
}

//Определяет, что встретится на клетке, по выпавшему числу.
//Ближняя зона: 0-2 гоблин, 3-4 скелет, 5 сундук (от сложности не зависит).
//Дальняя зона: диапазоны берутся из SPAWN_TABLE по сложности
Encounter encounterFromRoll(bool nearStart, int diffIdx, int roll)
{
    if (nearStart)
    {
        if (roll <= 2) return EncGoblin;
        if (roll <= 4) return EncSkeleton;
        return EncChest;
    }

    static const Encounter order[6] = { EncGoblin, EncSkeleton, EncTroll, EncOrc, EncVampire, EncChest };
    int row = diffIdx - 1;
    if (row < 0) row = 0;
    if (row > 3) row = 3;

    int edge = 0;
    for (int i = 0; i < 6; i++)
    {
        edge += SPAWN_TABLE[row][i];
        if (roll < edge) return order[i];
    }
    return EncChest;
}

//Читает число с клавиатуры, при ошибке ввода возвращает false
bool readNumber(int& value)
{
    cin >> value;
    if (cin.fail())
    {
        Cleaner();
        return false;
    }
    return true;
}

//Что произошло за раунд боя (чтобы красиво вывести)
struct RoundResult
{
    int dealt = 0;          // сколько урона нанёс игрок
    int taken = 0;          // сколько урона получил игрок (уже с учётом брони)
    int healed = 0;         // сколько здоровья вернуло зелье
    int drained = 0;        // сколько здоровья вампир восстановил себе
    bool windup = false;    // монстр в этом раунде замахивался и не бил (тролль)
};

//Один раунд боя: действие игрока, потом ответный удар монстра (если он ещё жив). Раунды считаются с 1.
//Возвращает false если действие не выполнено, тогда ход не засчитывается
bool battleRound(Character& player, Monster& monster, int action, int weaponIndex, int round, RoundResult& result)
{
    result = RoundResult();
    bool farAway = monster.isRanged() && round == 1;   // в первый раунд дальнобойный враг стреляет, а мы до него не достаём

    if (action == Attack)
    {
        if (farAway) return false;
        int before = monster.getHealth();
        monster.takeDamage(player.attack());
        result.dealt = before - monster.getHealth();
    }
    else if (action == ChangeWeapon)
    {
        if (farAway) return false;
        if (!player.equip(weaponIndex)) return false;
    }
    else if (action == DrinkPotion)
    {
        if (!player.drinkPotion(result.healed)) return false;
    }
    else if (action == Approach)
    {
        if (!farAway) return false;
    }
    else
        return false;

    if (monster.isAlive())
    {
        int dmg = monster.attackInRound(round);
        if (dmg == 0)
            result.windup = true;
        else
        {
            result.taken = player.hit(dmg);
            if (monster.isDrainer())
            {
                int before = monster.getHealth();
                monster.heal(1);
                result.drained = monster.getHealth() - before;
            }
        }
    }
    return true;
}

//Бой с монстром, пока кто-то не погибнет. Возвращает true если победил игрок
bool Battle(Character& player, Monster& monster)
{
    int round = 1;
    while (player.isAlive() && monster.isAlive())
    {
        bool farAway = monster.isRanged() && round == 1;

        cout << "\n--- БОЙ: " << monster.getName() << " (здоровье " << monster.getHealth() << ", урон " << monster.attack() << ") ---\n";
        cout << "Ваше здоровье: " << player.getHealth() << "/" << player.getMaxHealth()
             << ", броня: " << player.getArmorName() << " (-" << player.getArmorValue() << ")"
             << ", зелий: " << player.getPotions() << "\n";
        cout << "Оружие: " << player.getWeaponName() << " (урон " << player.attack() << ")\n";
        if (farAway)
        {
            cout << monster.getName() << " стреляет издалека, атаковать его сейчас нельзя!\n";
        }
        else
        {
            cout << "1.Атаковать\n";
            cout << "2.Сменить оружие\n";
        }
        cout << "3.Выпить зелье лечения\n";
        if (farAway)
            cout << "4.Сблизиться (пропустить ход)\n";
        cout << "Ваш выбор: ";

        int action;
        if (!readNumber(action))
        {
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }

        if (farAway && action != DrinkPotion && action != Approach)
        {
            cout << "Враг слишком далеко: сейчас можно только выпить зелье или сблизиться.\n";
            continue;
        }
        if (action == DrinkPotion)
        {
            if (player.getPotions() == 0)
            {
                cout << "У вас нет зелий.\n";
                continue;
            }
            if (player.getHealth() >= player.getMaxHealth())
            {
                cout << "Ваше здоровье уже полное.\n";
                continue;
            }
        }

        int index = 0;
        if (action == ChangeWeapon)
        {
            if (player.inventorySize() == 0)
            {
                cout << "В инвентаре нет оружия.\n";
                continue;
            }
            cout << "Оружие в инвентаре:\n";
            for (int i = 0; i < player.inventorySize(); i++)
                cout << i + 1 << "." << player.getInventory()[i].getName() << " (урон " << player.getInventory()[i].getDamage() << ")\n";
            cout << "Номер оружия: ";
            if (!readNumber(index))
            {
                cout << "Ошибка ввода. Попробуйте ещё раз\n";
                continue;
            }
            index--;
        }

        RoundResult r;
        if (!battleRound(player, monster, action, index, round, r))
        {
            cout << "Неверное действие.\n";
            continue;
        }

        if (action == Attack)
            cout << "Вы нанесли " << r.dealt << " урона.\n";
        else if (action == ChangeWeapon)
            cout << "Вы надели: " << player.getWeaponName() << "\n";
        else if (action == DrinkPotion)
            cout << "Вы выпили зелье и восстановили " << r.healed << " здоровья.\n";
        else if (action == Approach)
            cout << "Вы сблизились с врагом.\n";

        if (r.windup)
            cout << monster.getName() << " замахивается для сильного удара!\n";
        if (r.taken > 0)
        {
            if (monster.isRanged())
                cout << monster.getName() << " выстрелил и нанёс вам " << r.taken << " урона.\n";
            else
                cout << monster.getName() << " нанёс вам " << r.taken << " урона.\n";
        }
        if (r.drained > 0)
            cout << monster.getName() << " восстановил себе " << r.drained << " здоровья.\n";

        round++;
    }
    return player.isAlive();
}

//Предлагает надеть только что выпавшее оружие (оно лежит последним в инвентаре)
void OfferEquip(Character& player)
{
    while (true)
    {
        cout << "Надеть новое оружие? 1 - надеть, 2 - оставить в инвентаре: ";
        int choice;
        if (!readNumber(choice))
        {
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == 1)
        {
            player.equip(player.inventorySize() - 1);
            cout << "Вы надели: " << player.getWeaponName() << "\n";
            return;
        }
        if (choice == 2)
        {
            return;
        }
        cout << "Неверный пункт меню.\n";
    }
}

//Победа над монстром: засчитываем победу и забираем выпавшее оружие
void LootMonster(Character& player, const Monster& monster)
{
    player.addWin();
    Weapon drop = monster.getDrop();
    player.pickUp(drop);
    cout << "\nМонстр побеждён! Из него выпало оружие: " << drop.getName() << " (урон " << drop.getDamage() << ")\n";
    OfferEquip(player);
}

//Сундук: случайное число 0-5. 0-2 зелье лечения, 3 кожаная броня, 4 кольчуга, 5 латы
void OpenChest(Character& player)
{
    cout << "\nВы нашли сундук!\n" << CHEST_ART;
    int roll = rand() % 6;
    if (roll <= 2)
    {
        player.addPotion();
        cout << "Внутри зелье лечения (восстанавливает " << POTION_HEAL << " здоровья). Зелий у вас: " << player.getPotions() << "\n";
        return;
    }

    string name;
    int value;
    if (roll == 3)      { name = "Кожаная броня"; value = 1; }
    else if (roll == 4) { name = "Кольчуга";      value = 2; }
    else                { name = "Латные доспехи"; value = 3; }

    cout << "Внутри " << name << " (уменьшает получаемый урон на " << value << ").\n";
    if (player.wearArmor(name, value))
        cout << "Вы надели: " << name << "\n";
    else
        cout << "Ваша текущая броня (" << player.getArmorName() << ") не хуже, вы оставляете находку в сундуке.\n";
}

//Выпить зелье на карте (вне боя). Печатает результат
void DrinkPotionOnMap(Character& player)
{
    if (player.getPotions() == 0)
    {
        cout << "У вас нет зелий.\n";
        return;
    }
    int healed;
    if (!player.drinkPotion(healed))
    {
        cout << "Ваше здоровье уже полное.\n";
        return;
    }
    cout << "Вы выпили зелье и восстановили " << healed << " здоровья. Здоровье: "
         << player.getHealth() << "/" << player.getMaxHealth() << ", зелий осталось: " << player.getPotions() << "\n";
}

//Перемещение персонажа по клавише (W/A/S/D), за границы карты выйти нельзя
MoveResult movePlayer(Character& player, const Dungeon& dungeon, char key)
{
    int dx = 0, dy = 0;
    switch (key)
    {
    case 'w': case 'W': dy = -1; break;
    case 's': case 'S': dy = 1; break;
    case 'a': case 'A': dx = -1; break;
    case 'd': case 'D': dx = 1; break;
    default: return UnknownKey;
    }
    int newX = player.getX() + dx;
    int newY = player.getY() + dy;
    if (!dungeon.inside(newX, newY)) return Wall;
    player.moveTo(newX, newY);
    return Moved;
}

//Основной игровой цикл (используется и при новой игре, и при загрузке)
void PlayGame(Dungeon& dungeon, Character& player, const string& gameDifficulty)
{
    time_t lastSaveTime = time(nullptr);             // запоминаем время старта таймера

    while (true)
    {
        cout << "\n";
        dungeon.draw(player);
        cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth()
             << ", броня: " << player.getArmorName() << " (-" << player.getArmorValue() << ")"
             << ", зелий: " << player.getPotions()
             << ", побеждено монстров: " << player.getWins() << "\n";
        cout << "Ход (W - вверх, S - вниз, A - влево, D - вправо, H - выпить зелье, save - сохранить, Q - выход из игры): ";

        string command;
        if (!(cin >> command))                        // ввод закончился
        {
            Cleaner();
            break;
        }
        if (command == "q" || command == "Q")
        {
            cout << "Игра прервана.\n";
            break;
        }
        if (command == "save" || command == "SAVE")
        {
            if (saveGame(dungeon, player, gameDifficulty))
            {
                lastSaveTime = time(nullptr);         // сбрасываем таймер автосохранения
                cout << "Игра успешно сохранена.\n";
            }
            else
            {
                cout << "Не удалось сохранить игру.\n";
            }
            continue;
        }
        if (command == "h" || command == "H")
        {
            DrinkPotionOnMap(player);
            continue;
        }
        if (command.size() != 1)
        {
            cout << "Неизвестная команда.\n";
            continue;
        }

        MoveResult result = movePlayer(player, dungeon, command[0]);
        if (result == UnknownKey)
        {
            cout << "Неизвестная команда.\n";
            continue;
        }
        if (result == Wall)
        {
            cout << "Там стена, идти нельзя.\n";
            continue;
        }

        int px = player.getX();
        int py = player.getY();
        bool firstVisit = !dungeon.isVisited(px, py);
        if (firstVisit)
            dungeon.visit(px, py);

        if (dungeon.isExit(px, py))
        {
            cout << "\nВы нашли выход из подземелья! Поздравляем, вы победили!\n";
            break;
        }

        if (firstVisit)                               // на посещённых клетках ничего нет
        {
            bool nearStart = isNearStart(px, py);
            int roll = rand() % rollRange(nearStart);
            Encounter found = encounterFromRoll(nearStart, diffIndex(gameDifficulty), roll);

            if (found == EncChest)
            {
                OpenChest(player);
            }
            else
            {
                Monster monster = makeMonster(found);
                cout << "\nВы встретили монстра: " << monster.getName() << "!\n";
                cout << monster.getArt();
                if (!Battle(player, monster))
                {
                    cout << "\nВы погибли. Игра окончена.\n";
                    break;
                }
                LootMonster(player, monster);
            }
        }

        // Проверяем, не прошло ли 30 минут с последнего сохранения
        if (difftime(time(nullptr), lastSaveTime) >= AUTO_SAVE_INTERVAL)
        {
            if (saveGame(dungeon, player, gameDifficulty))
            {
                lastSaveTime = time(nullptr);
                cout << "\nПрошло 30 минут — выполнено автосохранение.\n";
            }
        }
    }
    cout << "Нажмите Enter для продолжения";
    Cleaner();
    cin.get();
}

void StartGame()
{
    cout << "\n***НАЧАЛО ИГРЫ***\n";
    cout << "Игра начинается...\n";
    cout << "Сложность игры: " << difficulty <<"\n";
    cout << "Размер карты подземелья: "<< sizeConverter(fieldsize) << "\n";
    cout << "Количество очков здоровья: " << health << "\n";
    cout << "Цель: найти выход. Он спрятан в дальнем от старта углу карты.\n";

    Dungeon dungeon(static_cast<int>(sqrt(fieldsize)));
    Character player(health);
    dungeon.visit(player.getX(), player.getY());      // стартовая клетка уже посещена
    dungeon.placeExit(diffIndex(difficulty) == VeryHard ? EXIT_ZONE_VERYHARD : EXIT_ZONE_NORMAL);

    PlayGame(dungeon, player, difficulty);
}

void LoadGame()
{
    cout << "\n***ЗАГРУЗКА ИГРЫ***\n";
    Dungeon dungeon(MIN_SIDE);
    Character player(MIN_HEALTH);
    string loadedDifficulty;

    if (!loadGame(dungeon, player, loadedDifficulty))
    {
        cout << "Сохранение не найдено или файл повреждён.\n";
        cout << "Нажмите Enter для продолжения";
        Cleaner();
        cin.get();
        return;
    }

    cout << "Сохранение успешно загружено!\n";
    cout << "Сложность игры: " << loadedDifficulty << "\n";
    cout << "Размер карты подземелья: " << dungeon.getSide() << "x" << dungeon.getSide() << "\n";
    cout << "Здоровье: " << player.getHealth() << "/" << player.getMaxHealth() << "\n";

    PlayGame(dungeon, player, loadedDifficulty);
}

void DiffSubMenu()
{
    while (true)
    {
        cout << "\n===== УРОВЕНЬ СЛОЖНОСТИ =====\n";
        cout << "Текущий уровень: " << difficulty << "\n";
        cout << "Введите номер уровня сложности:\n";
        cout << "1 - Легко, 2 - Средне, 3 - Сложно, 4 - Очень сложно\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < Easy || choice > VeryHard)
        {
            cout << "Недопустимое значение. Введите число от 1 до 4\n";
            continue;
        }
        difficulty = diffConverter(choice);
        cout << "Новый уровень сложности - " << difficulty << "\n";
        return;
    }
}

void SizeSubMenu()
{
    while (true)
    {
        cout << "\n***РАЗМЕР ПОДЗЕМЕЛЬЯ***\n";
        cout << "Текущий размер: " << sizeConverter(fieldsize) << "\n";
        cout << "Введите длину стороны карты (" << MIN_SIDE << "-" << MAX_SIDE << ")\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < MIN_SIDE || choice > MAX_SIDE)
        {
            cout << "Недопустимое значение. Значения могут варьироваться от " << MIN_SIDE << " до " << MAX_SIDE << "\n";
            continue;
        }
        fieldsize = choice * choice;
        cout << "Новый размер подземелья - " << sizeConverter(fieldsize) << "\n";
        return;
    }
}
void HealthSubMenu()
{
    while (true)
    {
        cout << "\n***Текущее количество очков здоровья: " << health << "\n";
        cout << "Введите необходимое количество очков здоровья (" << MIN_HEALTH << "-" << MAX_HEALTH << ")\n";
        cout << "Для выхода напишите -1\n";
        cout << "Ваш выбор: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        if (choice == -1)
        {
            return;
        }
        if (choice < MIN_HEALTH || choice > MAX_HEALTH)
        {
            cout << "Недопустимое значение. Значения могут варьироваться от " << MIN_HEALTH << " до " << MAX_HEALTH << "\n";
            continue;
        }
        health = choice;
        cout << "Количество очков здоровья изменено на " << health << "\n";
        return;
    }

}
//Последовательный ввод всех настроек с клавиатуры
void AllSettings()
{
    cout << "\n***ПОСЛЕДОВАТЕЛЬНАЯ НАСТРОЙКА***\n";
    cout << "Шаг 1 из 3\n";
    DiffSubMenu();
    cout << "Шаг 2 из 3\n";
    SizeSubMenu();
    cout << "Шаг 3 из 3\n";
    HealthSubMenu();
    cout << "Настройка завершена\n";
}
//Возврат к предыдущей записи из стека
void RestorePreviousSize()
{
    if (history.size() < 2)
    {
        cout << "Предыдущие значения не найдены.\n";
        return;
    }
    history.pop();                          // убираем текущую запись
    fieldsize = history.top().size;
    health = history.top().health;
    difficulty = history.top().difficulty;
    saveHistory();                          // файл снова совпадает со стеком
    cout << "Возвращены предыдущие значения. Размер карты: " << sizeConverter(fieldsize) << "\n";
}
void SettingsMenu ()
{
    while(true)
    {
        cout << "\n***НАСТРОЙКИ***\n";
        cout << "1.Изменить сложность (текущаяя сложность " << difficulty << " )\n";
        cout << "2.Изменить размер карты (текущий размер карты " << sizeConverter(fieldsize) << " )\n";
        cout << "3.Изменить количество очков здоровья (текущие очки здоровья " << health << " )\n";
        cout << "4.Задать все настройки по очереди\n";
        cout << "5.Вернуть предыдущий размер\n";
        cout << "6.Выход из настроек\n";
        cout << "Выберете нужное: ";

        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }
        switch (choice)
        {
        case Difficulty:
            DiffSubMenu();
            commitSettings();
            break;
        case Size:
            SizeSubMenu();
            commitSettings();
            break;
        case Health:
            HealthSubMenu();
            commitSettings();
            break;
        case All_Settings:
            AllSettings();
            commitSettings();
            break;
        case Restore_Size:
            RestorePreviousSize();
            break;
        case Go_Back:
            return;
        default:
            cout << "Неверный пункт меню.\n";
            break;
        }
    }
}
void aboutProgramm ()
{
    cout << "\n***О ПРОГРАММЕ***\n";
    cout << "Название игры : Приключение в подземелье\n";
    cout << "Версия игры: " << version << "\n";
    cout << "Автор: " << creator << "\n";
    cout << "Для возврата в главное меню нажмите Enter...";
    Cleaner();
    cin.get();
}
void ShowMainMenu()
{
    cout << "\n                    *****ПРИКЛЮЧЕНИЕ В ПОДЗЕМЕЛЬЕ*****\n";
    cout << "1.Начало игры\n";
    cout << "2.Загрузить игру\n";
    cout << "3.Настройки\n";
    cout << "4.Об игре\n";
    cout << "5.Выход\n";
    cout << "Выберите пункт меню: ";
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    srand(static_cast<unsigned>(time(nullptr)));   // чтобы монстры и выход каждый раз появлялись по-новому

    loadHistory();
    if (history.empty())
    {
        commitSettings();                   // значения по умолчанию попадают в историю
    }
    else
    {
        fieldsize = history.top().size;
        health = history.top().health;
        difficulty = history.top().difficulty;
    }

    while (true)
    {
        ShowMainMenu();
        int choice;
        cin >> choice;

        if (cin.fail())
        {
            Cleaner();
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
        }

        switch (choice)
        {
        case Start_Game:
            StartGame();
            break;
        case Load_Game:
            LoadGame();
            break;
        case Settings:
            SettingsMenu();
            break;
        case About:
            aboutProgramm();
            break;
        case Exit:
            cout << "Завершение работы.\n";
            return 0;
        default:
            cout << "Неверный пункт меню.\n";
            break;
        }
    }

    return 0;
}