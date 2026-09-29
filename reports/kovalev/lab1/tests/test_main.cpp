#define main app_main
#include "../src/main.cpp"
#undef main

#include <cstdio>

int passed = 0;
int failed = 0;
bool finished = false;
void onExit()
{
    if (!finished)
    {
        fprintf(stderr, "FAIL : программа завершилась посреди теста (не хватило ввода?)\n");
        _Exit(1);
    }
}

string run(const string& input)
{
    difficulty = 2;
    cows = 3;
    chickens = 5;

    stringstream in(input);
    stringstream out;
    streambuf* oldIn = cin.rdbuf(in.rdbuf());
    streambuf* oldOut = cout.rdbuf(out.rdbuf());
    cin.clear();

    app_main();

    cin.rdbuf(oldIn);
    cout.rdbuf(oldOut);
    return out.str();
}


int countOf(const string& s, const string& text)
{
    int count = 0;
    size_t pos = s.find(text);
    while (pos != string::npos)
    {
        count++;
        pos = s.find(text, pos + text.size());
    }
    return count;
}

void check(const string& name, bool ok)
{
    if (ok)
    {
        cout << "OK   : " << name << "\n";
        passed++;
    }
    else
    {
        cout << "FAIL : " << name << "\n";
        failed++;
    }
}


int readFrom(const string& text)
{
    stringstream in(text);
    streambuf* oldIn = cin.rdbuf(in.rdbuf());
    cin.clear();
    int result = readInt();
    cin.rdbuf(oldIn);
    return result;
}

int main()
{
    atexit(onExit);

    check("readInt: число", readFrom("7\n") == 7);
    check("readInt: буквы дают -1", readFrom("abc\n") == -1);
    check("readInt: лишние символы дают -1", readFrom("3 4\n") == -1);

    string out;

    out = run("5\n");
    check("Выход из программы", out.find("Программа завершена") != string::npos);

    out = run("1\n\n5\n");
    check("Пункт 1: начало игры", out.find("Игра началась") != string::npos);
    check("Пункт 1: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    out = run("2\n\n5\n");
    check("Пункт 2: загрузка недоступна", out.find("недоступна") != string::npos);
    check("Пункт 2: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    out = run("3\n4\n5\n");
    check("Пункт 3: открывается подменю настроек", out.find("НАСТРОЙКИ") != string::npos);

    out = run("4\n\n5\n");
    check("Пункт 4: версия программы", out.find("Версия: " + VERSION) != string::npos);
    check("Пункт 4: автор", out.find("Автор: " + STUDENT_NAME) != string::npos);
    check("Пункт 4: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    out = run("3\n2\n4\n4\n1\n\n5\n");
    check("Коровы: значение в переменной", cows == 10);
    check("Коровы: значение видно в игре", out.find("Количество коров: 10") != string::npos);

    out = run("3\n3\n4\n4\n1\n\n5\n");
    check("Куры: значение в переменной", chickens == 20);
    check("Куры: значение видно в игре", out.find("Количество кур: 20") != string::npos);

    out = run("3\n1\n3\n4\n1\n\n5\n");
    check("Сложность: значение в переменной", difficulty == 3);
    check("Сложность: значение видно в игре", out.find("Уровень сложности: Сложно") != string::npos);

    out = run("3\n2\n4\n4\n3\n4\n1\n\n5\n");
    check("Повторный вход в настройки не сбрасывает значения", out.find("Количество коров: 10") != string::npos);

    out = run("9\n5\n");
    check("Несуществующий пункт меню", out.find("такого пункта нет") != string::npos);

    out = run("abc\n5\n");
    check("Ввод букв вместо числа", out.find("такого пункта нет") != string::npos);

    out = run("3\n2\n9\n4\n5\n");
    check("Неверный вариант в подменю не меняет настройку", cows == 3);

    cout << "----------------------------------\n";
    cout << "Пройдено: " << passed << ", провалено: " << failed << "\n";

    finished = true;
    return failed == 0 ? 0 : 1;
}
