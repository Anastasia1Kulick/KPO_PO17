#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <stack>
#include <string>
#include <cstdlib>
#include <ctime>

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
    int health;
    int size;
    std::string diff;
};

class Weapon {
public:
    std::string name;
    int damage;
    Weapon() : name("Кулаки"), damage(2) {}
    Weapon(std::string n, int d) : name(n), damage(d) {}
};

class Character {
public:
    std::string name;
    int maxHealth;
    int health;
    Weapon weapon;
    Character() : name("Герой"), maxHealth(10), health(10), weapon() {}
    Character(std::string n, int h) : name(n), maxHealth(h), health(h), weapon() {}
    void EquipWeapon(const Weapon& w) {
        weapon = w;
        std::cout << "Вы экипировали: " << w.name << " (урон: " << w.damage << ")\n";
    }
    int Attack() {
        return weapon.damage;
    }
    void TakeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }
    bool IsAlive() const {
        return health > 0;
    }
};

class Monster {
public:
    std::string name;
    int health;
    int damage;
    Weapon drop;
    bool hasDrop;
    Monster() : name("Слизень"), health(5), damage(1), drop(), hasDrop(true) {}
    Monster(std::string n, int h, int d, Weapon w, bool hd)
        : name(n), health(h), damage(d), drop(w), hasDrop(hd) {
    }
    void TakeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
    }
    bool IsAlive() const {
        return health > 0;
    }
};
static Weapon GenerateRandomWeapon(int level);
static Monster GenerateMonster(int level);
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
            opFile << 10 << " " << 50 << " Легкая\n";
            opFile.close();
        }
        else std::cerr << "Ошибка при создании файла";
    }
    while (true) {
        system("cls");
        DisplayMenu();
        std::cin >> choice;
        if (choice >= 1 && choice <= 3) {
            menuFunctions[choice - 1]();
        }
        else if (choice == AboutOp) About();
        else if (choice == ExitOp) break;
    }
}
void DisplayMenu() {
    std::cout << "Приключение в подземелье \n";
    std::cout << "1. Начать игру\n";
    std::cout << "2. Загрузить игру\n";
    std::cout << "3. Настройки\n";
    std::cout << "4. О программе\n";
    std::cout << "5. Выход\n";
}
static Weapon GenerateRandomWeapon(int level) {
    std::string names[] = { "Ржавый меч", "Дубина", "Кинжал", "Топор", "Копьё", "Булава" };
    int idx = rand() % 6;
    int dmg = 3 + rand() % (3 + level);
    return Weapon(names[idx], dmg);
}

static Monster GenerateMonster(int level) {
    std::string names[] = { "Слизень", "Гоблин", "Скелет", "Паук", "Орк", "Тролль" };
    int idx = rand() % 6;
    int hp = 3 + level + rand() % (3 + level);
    int dmg = 1 + rand() % (1 + level);
    Weapon drop = GenerateRandomWeapon(level);
    return Monster(names[idx], hp, dmg, drop, true);
}

void GameStart() {
    system("cls");
    std::cout << "Начало игры\n";

    Options opts;
    opts.health = 10;
    opts.size = 50;
    opts.diff = "Легкая";

    std::ifstream inFile("options.txt");
    if (inFile.is_open()) {
        inFile >> opts.health >> opts.size >> opts.diff;
        inFile.close();
    }

    int diffMod = 1;
    if (opts.diff == "Легко" || opts.diff == "Легкая") diffMod = 1;
    else if (opts.diff == "Нормально") diffMod = 2;
    else if (opts.diff == "Тяжело") diffMod = 3;

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
    srand((unsigned)time(nullptr));

    for (int floor = 1; floor <= totalFloors; floor++) {
        system("cls");
        std::cout << "=== Этаж " << floor << " из " << totalFloors << " ===\n";
        Monster monster = GenerateMonster(floor * diffMod);
        std::cout << "На вас напал " << monster.name << "! (HP: " << monster.health << ", урон: " << monster.damage << ")\n";

        while (hero.IsAlive() && monster.IsAlive()) {
            std::cout << "\nВаше HP: " << hero.health << "/" << hero.maxHealth;
            std::cout << " | Оружие: " << hero.weapon.name << " (урон: " << hero.weapon.damage << ")\n";
            std::cout << monster.name << " HP: " << monster.health << "\n";
            std::cout << "1. Атаковать\n2. Защищаться (пропустить ход)\n";
            std::cout << "Выберите действие: ";
            int act;
            std::cin >> act;
            bool defending = false;
            if (act == 1) {
                int dmg = hero.Attack();
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

        std::cout << "\nИз монстра выпало оружие: " << monster.drop.name
            << " (урон: " << monster.drop.damage << ")\n";
        std::cout << "1. Экипировать\n2. Оставить текущее\nВыбор: ";
        int pick;
        std::cin >> pick;
        if (pick == 1) hero.EquipWeapon(monster.drop);

        std::cout << "\nНажмите Enter для продолжения ";
        std::cin.ignore();
        std::cin.get();
    }

    if (hero.IsAlive()) {
        std::cout << "\nПоздравляем! Вы прошли подземелье!\n";
    }
    std::cout << "Нажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
}

void GameLoad() {
    system("cls");
    std::cout << "Данная функция недоступна, появится в ближайших обновлениях";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
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
        std::cout << "1. Изменить Параметры\n2. Вернуть Предыдущие параметры\n3. Назад\n";
        std::cin >> choice;
        if (choice < 1 && choice > 3) std::cout << "Неверное Значение";
        else if (choice == 1) {
            system("cls");
            std::cout << "Введите Новые Параметры\nЗдоровье (Значение от 1 до 10): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 10) std::cout << "Неверное Значение\n";
            else cur.health = NewVal;
            std::cout << "Размер Подземелья (1.Маленькое 2.Среднее 3.Большое): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 3) std::cout << "Неверное Значение\n";
            else
                switch (NewVal) {
                case 1:
                    cur.size = 50;
                    break;
                case 2:
                    cur.size = 100;
                    break;
                case 3:
                    cur.size = 225;
                    break;
                }
            std::cout << "Сложность (1.Легко 2.Нормально 3.Тяжело): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 3) std::cout << "Неверное Значение\n";
            else
                switch (NewVal) {
                case 1:
                    cur.diff = "Легко";
                    break;
                case 2:
                    cur.diff = "Нормально";
                    break;
                case 3:
                    cur.diff = "Тяжело";
                    break;
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
            file.close();
        }
        system("pause");
    }
    return;
}
void About() {
    system("cls");
    std::cout << "Версия игры 0.5\nВыполнил Бородин Кирилл";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
}
