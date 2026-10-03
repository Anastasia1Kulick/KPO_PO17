#include <iostream>
#include <cstring>
#include <windows.h>
void DisplayMenu();
void GameStart(int& HpOp, int& SizeOp, int& DiffOp);
void GameLoad(int& HpOpa,int& SizeOp, int& DiffOp);
void Settings(int& HpOp,int& SizeOp, int& DiffOp);
void About();
void (*const menuFunctions[])(int& HpOp, int& SizeOp, int& DiffOp) = {
    GameStart,
    GameLoad,
    Settings
};
enum {
    AboutOp = 4, ExitOp
};
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    int choice;
    int HpSet = 10, DnSizeSet = 1, DiffSet = 1;
    while (true) {
        system("cls");
        DisplayMenu();
        std::cin >> choice;
        if (choice >= 1 && choice <= 3) {
            menuFunctions[choice - 1](HpSet, DnSizeSet, DiffSet);
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
void GameStart(int& HpOp, int& SizeOp, int& DiffOp) {
    system("cls");
    std::cout << "Начало игры";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
}
void GameLoad(int& HpOp,int& SizeOp, int& DiffOp) {
    system("cls");
    std::cout << "Данная функция недоступна, появится в ближайших обновлениях";
    std::cout << "\nНажмите Enter для продолжения ";
    std::cin.ignore();
    std::cin.get();
    return;
}
void Settings(int& HpOp, int& SizeOp, int& DiffOp) {
    int choice, NewVal;
    while (true) {
    system("cls");
    std::cout << "Текущие Параметры:\n";
    std::cout << "Здоровье: ";
    std::cout << HpOp << std::endl;
    std::cout << "Размер Подземелья: ";
    switch (SizeOp) {
    case 1:
        std::cout << "Маленькое\n";
        break;
    case 2:
        std::cout << "Среднее\n";
        break;
    case 3:
        std::cout << "Большое\n";
        break;
    }
    std::cout << "Сложность: ";
    switch (DiffOp) {
    case 1:
        std::cout << "Легко\n";
        break;
    case 2:
        std::cout << "Нормально\n";
        break;
    case 3:
        std::cout << "Тяжело\n";
        break;
    }
    std::cout << "1. Изменить Параметры\n2. Назад\n";
        std::cin >> choice;
        if (choice != 1 && choice != 2) std::cout << "Неверное Значение";
        else if (choice == 1) {
            system("cls");
            std::cout << "Введите Новые Параметры\nЗдоровье (Значение от 1 до 10): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 10) std::cout << "Неверное Значение\n";
            else HpOp = NewVal;
            std::cout << "Размер Подземелья (1.Маленькое 2.Среднее 3.Большое): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 3) std::cout << "Неверное Значение\n";
            else SizeOp = NewVal;
            std::cout << "Сложность (1.Легко 2.Нормально 3.Тяжело): ";
            std::cin >> NewVal;
            if (NewVal < 1 || NewVal > 3) std::cout << "Неверное Значение\n";
            else DiffOp = NewVal;
        }
        else if (choice == 2) break;
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

