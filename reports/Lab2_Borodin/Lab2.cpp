#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>
#include <stack>
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
void GameStart() {
    system("cls");
    std::cout << "Начало игры";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
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
    if(!file.is_open()) std::cerr << "Ошибка при открытии файла";
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
    std::cout << "Версия игры 0.1\nВыполнил Бородин Кирилл";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
}

