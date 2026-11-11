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
    ChangeWeapon
};
enum MoveResult {
    Moved,
    Wall,
    UnknownKey
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
const float version = 1.0;
const string creator = "Наврось Глеб";
stack<GameSettings> history;
string settingsFile = "settings.txt";

//Границы значений, которые вводятся с клавиатуры
const int MIN_SIDE = 5;
const int MAX_SIDE = 20;
const int MIN_HEALTH = 1;
const int MAX_HEALTH = 10;

//Сколько монстров надо победить для победы в игре
const int WIN_CONDITION = 3;

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

//Монстр: здоровье, урон и оружие, которое выпадает после победы над ним
class Monster
{
    string name;
    int health;
    int damage;
    Weapon drop;
public:
    Monster(string n, int h, int d, Weapon w) : name(n), health(h), damage(d), drop(w) {}
    bool isAlive() const { return health > 0; }
    void takeDamage(int dmg) { health -= dmg; if (health < 0) health = 0; }
    int attack() const { return damage; }
    int getHealth() const { return health; }
    string getName() const { return name; }
    Weapon getDrop() const { return drop; }
};

//Персонаж: здоровье, позиция на карте, оружие, инвентарь и счётчик побед
class Character
{
    int health;
    int x = 0;
    int y = 0;
    int wins = 0;
    Weapon weapon;
    vector<Weapon> inventory;
public:
    Character(int h) : health(h) {}
    int getHealth() const { return health; }
    bool isAlive() const { return health > 0; }
    void takeDamage(int dmg) { health -= dmg; if (health < 0) health = 0; }
    int attack() const { return weapon.getDamage(); }
    string getWeaponName() const { return weapon.getName(); }

    int getX() const { return x; }
    int getY() const { return y; }
    void moveTo(int newX, int newY) { x = newX; y = newY; }

    int getWins() const { return wins; }
    void addWin() { wins++; }

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

//Карта подземелья: какие клетки уже посещены
class Dungeon
{
    int side;
    vector<vector<bool>> visited;
public:
    Dungeon(int s) : side(s), visited(s, vector<bool>(s, false)) {}
    bool inside(int x, int y) const { return x >= 0 && y >= 0 && x < side && y < side; }
    bool isVisited(int x, int y) const { return visited[y][x]; }
    void visit(int x, int y) { visited[y][x] = true; }
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

//Создаёт монстра по номеру (по кругу три вида)
Monster makeMonster(int number)
{
    switch (number % 3)
    {
    case 0: return Monster("Гоблин", 4, 1, Weapon("Ржавый кинжал", 3));
    case 1: return Monster("Скелет", 6, 2, Weapon("Костяной меч", 4));
    default: return Monster("Тролль", 10, 2, Weapon("Дубина тролля", 6));
    }
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

//Один раунд боя: действие игрока, потом ответный удар монстра (если он ещё жив)
//Возвращает false если действие не выполнено, тогда ход не засчитывается
bool battleRound(Character& player, Monster& monster, int action, int weaponIndex)
{
    if (action == Attack)
        monster.takeDamage(player.attack());
    else if (action == ChangeWeapon)
    {
        if (!player.equip(weaponIndex)) return false;
    }
    else
        return false;

    if (monster.isAlive())
        player.takeDamage(monster.attack());
    return true;
}

//Бой с монстром, пока кто-то не погибнет. Возвращает true если победил игрок
bool Battle(Character& player, Monster& monster)
{
    while (player.isAlive() && monster.isAlive())
    {
        cout << "\n--- БОЙ: " << monster.getName() << " (здоровье " << monster.getHealth() << ", урон " << monster.attack() << ") ---\n";
        cout << "Ваше здоровье: " << player.getHealth() << ", оружие: " << player.getWeaponName() << " (урон " << player.attack() << ")\n";
        cout << "1.Атаковать\n";
        cout << "2.Сменить оружие\n";
        cout << "Ваш выбор: ";

        int action;
        if (!readNumber(action))
        {
            cout << "Ошибка ввода. Попробуйте ещё раз\n";
            continue;
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

        int monsterBefore = monster.getHealth();
        int playerBefore = player.getHealth();
        if (!battleRound(player, monster, action, index))
        {
            cout << "Неверное действие.\n";
            continue;
        }

        if (action == Attack)
            cout << "Вы нанесли " << monsterBefore - monster.getHealth() << " урона.\n";
        else
            cout << "Вы надели: " << player.getWeaponName() << "\n";
        if (player.getHealth() < playerBefore)
            cout << monster.getName() << " нанёс вам " << playerBefore - player.getHealth() << " урона.\n";
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
    if (player.getWins() < WIN_CONDITION)
        OfferEquip(player);
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

void StartGame()
{
    cout << "\n***НАЧАЛО ИГРЫ***\n";
    cout << "Игра начинается...\n";
    cout << "Сложность игры: " << difficulty <<"\n";
    cout << "Размер карты подземелья: "<< sizeConverter(fieldsize) << "\n";
    cout << "Количество очков здоровья: " << health << "\n";

    Dungeon dungeon(static_cast<int>(sqrt(fieldsize)));
    Character player(health);
    dungeon.visit(player.getX(), player.getY());      // стартовая клетка уже посещена

    while (true)
    {
        cout << "\n";
        dungeon.draw(player);
        cout << "Здоровье: " << player.getHealth() << ", побед: " << player.getWins() << " из " << WIN_CONDITION << "\n";
        cout << "Ход (W - вверх, S - вниз, A - влево, D - вправо, Q - выход из игры): ";

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
        if (dungeon.isVisited(player.getX(), player.getY()))
            continue;                                 // на посещённых клетках монстров нет

        dungeon.visit(player.getX(), player.getY());
        Monster monster = makeMonster(player.getWins());
        cout << "\nВы встретили монстра: " << monster.getName() << "!\n";
        if (!Battle(player, monster))
        {
            cout << "\nВы погибли. Игра окончена.\n";
            break;
        }
        LootMonster(player, monster);
        if (player.getWins() >= WIN_CONDITION)
        {
            cout << "\nПоздравляем! Вы победили " << WIN_CONDITION << " монстров и прошли подземелье!\n";
            break;
        }
    }
    cout << "Нажмите Enter для продолжения";
    Cleaner();
    cin.get();
}
void LoadGame()
{
    cout << "\n***ЗАГРУЗКА ИГРЫ***\n";
    cout << "Пока что данная функция недоступна. Ждите обновлений!\n";
    cout << "Нажмите Enter для продолжения";
    Cleaner();
    cin.get();
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