#define main app_main
#include "../src/main.cpp"
#undef main

#include <cstdio>

const string TEST_FILE = "build/test_settings.txt";

int passed = 0;
int failed = 0;
bool finished = false;

void onExit()
{
    if (!finished)
    {
        fflush(stdout);
        fprintf(stderr, "FAIL : программа завершилась посреди теста (не хватило ввода?)\n");
        _Exit(1);
    }
}

void removeFile()
{
    remove(TEST_FILE.c_str());
}

void writeFile(const string& text)
{
    ofstream out(TEST_FILE.c_str());
    out << text;
}

string readFile()
{
    ifstream in(TEST_FILE.c_str());
    stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool fileExists()
{
    ifstream in(TEST_FILE.c_str());
    return in.is_open();
}

string run(const string& input)
{
    settingsFile = TEST_FILE;

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

bool has(const string& s, const string& text)
{
    return s.find(text) != string::npos;
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
    string out;

    check("readInt: число", readFrom("7\n") == 7);
    check("readInt: буквы дают -1", readFrom("abc\n") == -1);
    check("readInt: лишние символы дают -1", readFrom("3 4\n") == -1);

    removeFile();
    out = run("5\n");
    check("Выход из программы", has(out, "Программа завершена"));

    removeFile();
    out = run("1\n\n5\n");
    check("Пункт 1: начало игры", has(out, "Игра началась"));
    check("Пункт 1: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    removeFile();
    out = run("2\n\n5\n");
    check("Пункт 2: загрузка недоступна", has(out, "недоступна"));
    check("Пункт 2: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    removeFile();
    out = run("3\n4\n5\n");
    check("Пункт 3: открывается подменю настроек", has(out, "НАСТРОЙКИ"));

    removeFile();
    out = run("4\n\n5\n");
    check("Пункт 4: версия программы", has(out, "Версия: " + VERSION));
    check("Пункт 4: автор", has(out, "Автор: " + STUDENT_NAME));
    check("Пункт 4: возврат в главное меню", countOf(out, "1. Начать игру") == 2);

    removeFile();
    out = run("9\n5\n");
    check("Несуществующий пункт меню", has(out, "такого пункта нет"));

    removeFile();
    out = run("abc\n5\n");
    check("Ввод букв вместо числа", has(out, "такого пункта нет"));

    removeFile();
    out = run("5\n");
    check("Первый запуск: файл настроек создан", fileExists());
    check("Первый запуск: сообщение о создании файла", has(out, "Создан"));
    check("Первый запуск: сложность по умолчанию в файле", has(readFile(), "difficulty=Нормально"));
    check("Первый запуск: корова по умолчанию в файле", has(readFile(), "animal.Корова=3"));
    check("Первый запуск: курица по умолчанию в файле", has(readFile(), "animal.Курица=5"));

    removeFile();
    out = run("3\n1\n3\n4\n5\n");
    check("Сложность: значение в переменной", difficulty == "Сложно");
    check("Сложность: запись в файл", has(readFile(), "difficulty=Сложно"));

    removeFile();
    out = run("3\n2\n1\n10\n4\n5\n");
    check("Количество животных: значение в Map", animals["Корова"] == 10);
    check("Количество животных: запись в файл", has(readFile(), "animal.Корова=10"));

    removeFile();
    out = run("3\n2\n1\n500\n4\n5\n");
    check("Количество вне диапазона не принимается", animals["Корова"] == 3);
    check("Количество вне диапазона: сообщение об ошибке", has(out, "количество должно быть"));

    removeFile();
    out = run("3\n3\nСвинья\n7\n4\n5\n");
    check("Новый вид: добавлен в Map", animals.count("Свинья") == 1 && animals["Свинья"] == 7);
    check("Новый вид: сразу записан в файл", has(readFile(), "animal.Свинья=7"));

    removeFile();
    out = run("3\n3\nКорова\n4\n5\n");
    check("Новый вид: повтор названия не принимается", has(out, "такой вид уже есть"));
    check("Новый вид: старое значение не изменилось", animals["Корова"] == 3);

    removeFile();
    out = run("3\n3\nУтка\nabc\n4\n5\n");
    check("Новый вид: неверное количество не принимается", animals.count("Утка") == 0);
    check("Новый вид: неверное количество не попало в файл", !has(readFile(), "Утка"));

    removeFile();
    out = run("3\n3\n   \n4\n5\n");
    check("Новый вид: пустое название не принимается", has(out, "неверное название"));

    removeFile();
    run("3\n1\n1\n3\nЛошадь\n4\n4\n5\n");
    out = run("1\n\n5\n");
    check("Перезапуск: сложность загружена из файла", has(out, "Уровень сложности: Легко"));
    check("Перезапуск: новый вид загружен из файла", has(out, "Лошадь: 4"));
    check("Перезапуск: второй запуск не создаёт файл заново", !has(out, "Создан"));

    removeFile();
    writeFile("difficulty=Сложно\nanimal.Овца=8\n");
    out = run("1\n\n5\n");
    check("Загрузка: сложность из файла", has(out, "Уровень сложности: Сложно"));
    check("Загрузка: животное из файла", has(out, "Овца: 8"));
    check("Загрузка: животных по умолчанию нет", !has(out, "Корова"));

    removeFile();
    writeFile("мусор\ndifficulty=Невозможно\nanimal.Корова=abc\nanimal.Курица=5\n");
    out = run("5\n");
    check("Ошибки в файле: есть предупреждение", has(out, "Предупреждение"));
    check("Ошибки в файле: сложность по умолчанию", difficulty == "Нормально");
    check("Ошибки в файле: верная строка прочитана", animals.count("Курица") == 1 && animals["Курица"] == 5);
    check("Ошибки в файле: неверная строка пропущена", animals.count("Корова") == 0);
    check("Ошибки в файле: файл исправлен", has(readFile(), "difficulty=Нормально"));

    removeFile();
    writeFile("");
    out = run("5\n");
    check("Пустой файл: настройки по умолчанию", difficulty == "Нормально" && animals["Корова"] == 3);

    removeFile();
    writeFile("difficulty=Легко\r\nanimal.Овца=2\r\n");
    out = run("5\n");
    check("Файл с переводами строк Windows читается", difficulty == "Легко" && animals["Овца"] == 2);

    removeFile();

    cout << "----------------------------------\n";
    cout << "Пройдено: " << passed << ", провалено: " << failed << "\n";

    finished = true;
    return failed == 0 ? 0 : 1;
}
