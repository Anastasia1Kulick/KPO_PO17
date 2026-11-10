#define main app_main
#include "../src/main.cpp"
#undef main

#include <cstdio>
#include <set>

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

bool onlyAscii(const string& text)
{
    for (unsigned char c : text)
        if (c >= 128)
            return false;
    return true;
}

string repeat(const string& text, int times)
{
    string result;
    for (int i = 0; i < times; i++)
        result += text;
    return result;
}

string wholeGame()
{
    return "1\n" + repeat("5\n", 7) + "\n5\n";
}

int main()
{
    atexit(onExit);
    string out;

    check("readInt: число", readFrom("7\n") == 7);
    check("readInt: буквы дают -1", readFrom("abc\n") == -1);
    check("readInt: лишние символы дают -1", readFrom("3 4\n") == -1);

    check("Рисунки: ферма не пустая и состоит из нескольких строк", splitLines(farmArt()).size() >= 8);
    size_t widest = 0;
    for (const string& line : splitLines(farmArt()))
        if (line.size() > widest)
            widest = line.size();
    check("Рисунки: ферма помещается в окно шириной 80 символов", widest > 0 && widest <= 78);
    check("Рисунки: все рисунки только из ASCII-символов",
          onlyAscii(farmArt()) && onlyAscii(cowArt()) && onlyAscii(chickenArt()));
    check("Рисунки: кицунэ нарисована, рисунок из ASCII и не такой, как у других",
          onlyAscii(kitsuneArt()) && !kitsuneArt().empty() && kitsuneArt() != cowArt() && kitsuneArt() != chickenArt());
    check("Рисунки: у кицунэ есть свой рисунок", animalArt("Кицунэ") == kitsuneArt());
    check("Рисунки: корова и курица разные", cowArt() != chickenArt() && !cowArt().empty() && !chickenArt().empty());
    check("Рисунки: у коровы и курицы есть свой рисунок",
          animalArt("Корова") == cowArt() && animalArt("Курица") == chickenArt());
    check("Рисунки: у нового вида рисунка нет", animalArt("Утка").empty());
    check("Рисунки: строки делятся правильно", splitLines("a\nbb\n").size() == 2);
    check("Рисунки: рисунки ставятся рядом и выравниваются по низу",
          sideBySide("a\nb\n", "c\n") == "a\nb    c\n");
    check("Рисунки: если левый рисунок ниже, он тоже выравнивается по низу",
          sideBySide("a\n", "b\nc\n") == "     b\na    c\n");
    check("Рисунки: в конце строк нет лишних пробелов", sideBySide("abc\n", "\n") == "abc\n");

    Food hay("Сено", 5);
    check("Food: название и цена", hay.getName() == "Сено" && hay.getPrice() == 5);
    check("Food: в списке пять видов корма", foodList().size() == 5);

    check("Корм: корова ест сено", foodNameForSpecies("Корова") == "Сено");
    check("Корм: курица ест зерно", foodNameForSpecies("Курица") == "Зерно");
    check("Корм: свинья ест овощи", foodNameForSpecies("Свинья") == "Овощи");
    check("Корм: овца ест траву", foodNameForSpecies("Овца") == "Трава");
    check("Корм: кицунэ ест тофу", foodNameForSpecies("Кицунэ") == "Тофу");
    check("Корм: у разных видов корм разный", foodNameForSpecies("Корова") != foodNameForSpecies("Курица"));
    bool unknownOk = false;
    for (const Food& f : foodList())
        if (f.getName() == foodNameForSpecies("Утка"))
            unknownOk = true;
    check("Корм: для нового вида подбирается существующий корм", unknownOk);
    check("Корм: для нового вида выбор всегда одинаковый", foodNameForSpecies("Утка") == foodNameForSpecies("Утка"));
    set<string> unknownFoods;
    unknownFoods.insert(foodNameForSpecies("Утка"));
    unknownFoods.insert(foodNameForSpecies("Гусь"));
    unknownFoods.insert(foodNameForSpecies("Кролик"));
    unknownFoods.insert(foodNameForSpecies("Коза"));
    check("Корм: у новых видов корм бывает разный", unknownFoods.size() > 1);

    Animal cow("Корова", "Корова 1");
    check("Animal: начальные значения", cow.getHunger() == START_HUNGER && cow.getMood() == START_MOOD);
    check("Animal: корм по виду", cow.getFoodName() == "Сено");
    cow.feed();
    check("Animal: кормление снижает голод, но не ниже нуля", cow.getHunger() == 0);
    cow.care();
    check("Animal: забота повышает настроение, но не выше предела", cow.getMood() == MAX_STAT);
    cow.passDay(20);
    check("Animal: за день растут голод и падает настроение", cow.getHunger() == 20 && cow.getMood() == 90);
    check("Animal: доход у сытого животного с хорошим настроением", cow.getDailyIncome() == 12);
    cow.passDay(40);
    check("Animal: голодное животное не приносит дохода", cow.getDailyIncome() == 0);
    cow.passDay(100);
    check("Animal: голод не больше предела", cow.getHunger() == MAX_STAT);
    check("Animal: при полном голоде животное погибает", !cow.isAlive());

    Animal sad("Курица", "Курица 1");
    for (int i = 0; i < 3; i++)
        sad.passDay(0);
    check("Animal: плохое настроение лишает дохода", sad.getMood() == 40 && sad.getDailyIncome() == 0);

    check("Кицунэ: цена и доход заданы", priceForSpecies("Кицунэ") == 80 && incomeForSpecies("Кицунэ") == 15);

    Player noFox(0, 5);
    noFox.addAnimal("Курица");
    check("Кицунэ: без кицунэ курицу никто не ест", noFox.kitsuneEats().empty() && noFox.getHerd().size() == 1);

    Player lonelyFox(0, 5);
    lonelyFox.addAnimal("Кицунэ");
    lonelyFox.addAnimal("Корова");
    check("Кицунэ: если куриц нет, есть некого", lonelyFox.kitsuneEats().empty() && lonelyFox.getHerd().size() == 2);

    Player oneFox(0, 5);
    oneFox.addAnimal("Кицунэ");
    oneFox.addAnimal("Курица");
    oneFox.addAnimal("Курица");
    oneFox.addAnimal("Корова");
    vector<string> eaten = oneFox.kitsuneEats();
    check("Кицунэ: за день съедает одну курицу", eaten.size() == 1 && eaten[0] == "Курица 1");
    check("Кицунэ: съеденная курица пропадает из стада", oneFox.getHerd().size() == 3);
    check("Кицунэ: остальные животные остаются на месте",
          oneFox.getHerd()[0].getName() == "Кицунэ 1" && oneFox.getHerd()[1].getName() == "Курица 2"
          && oneFox.getHerd()[2].getName() == "Корова 1");
    eaten = oneFox.kitsuneEats();
    check("Кицунэ: на следующий день съедает следующую курицу", eaten.size() == 1 && eaten[0] == "Курица 2");
    eaten = oneFox.kitsuneEats();
    check("Кицунэ: когда куры кончились, есть некого", eaten.empty() && oneFox.getHerd().size() == 2);

    Player twoFoxes(0, 5);
    twoFoxes.addAnimal("Кицунэ");
    twoFoxes.addAnimal("Кицунэ");
    twoFoxes.addAnimal("Курица");
    twoFoxes.addAnimal("Курица");
    twoFoxes.addAnimal("Курица");
    eaten = twoFoxes.kitsuneEats();
    check("Кицунэ: каждая кицунэ съедает по одной курице",
          eaten.size() == 2 && eaten[0] == "Курица 1" && eaten[1] == "Курица 2" && twoFoxes.getHerd().size() == 3);

    Player hungryFoxes(0, 5);
    hungryFoxes.addAnimal("Кицунэ");
    hungryFoxes.addAnimal("Кицунэ");
    hungryFoxes.addAnimal("Курица");
    eaten = hungryFoxes.kitsuneEats();
    check("Кицунэ: если кур меньше, чем кицунэ, съедены только те, что есть", eaten.size() == 1 && hungryFoxes.getHerd().size() == 2);
    check("Кицунэ: сытой становится только та, что поела",
          hungryFoxes.getHerd()[0].getHunger() == 0 && hungryFoxes.getHerd()[1].getHunger() == START_HUNGER);

    Player fedFox(0, 5);
    fedFox.addAnimal("Кицунэ");
    fedFox.addAnimal("Курица");
    fedFox.kitsuneEats();
    DayReport eatReport = fedFox.endDay(0);
    check("Кицунэ: курица в еду кицунэ, голод падает", fedFox.getHerd()[0].getHunger() == 0);
    check("Кицунэ: съеденная курица дохода не приносит", eatReport.income == incomeForSpecies("Кицунэ"));


    check("Правила: у сложности разные параметры", getRules("Легко").actionsPerDay != getRules("Сложно").actionsPerDay
          && getRules("Легко").startMoney > getRules("Сложно").startMoney
          && getRules("Легко").hungerPerDay < getRules("Сложно").hungerPerDay);
    check("Правила: неизвестный уровень считается обычным", getRules("Нормально").startMoney == getRules("Что-то").startMoney);

    Player player(100, 3);
    player.addAnimal("Корова");
    player.addAnimal("Корова");
    player.addAnimal("Курица");
    check("Player: животные получают номера по видам",
          player.getHerd()[0].getName() == "Корова 1" && player.getHerd()[1].getName() == "Корова 2"
          && player.getHerd()[2].getName() == "Курица 1");

    check("Имена: можно переименовать стартовое животное", player.renameAnimal(0, "  Бурёнка  ") && player.getHerd()[0].getName() == "Бурёнка");
    check("Имена: нельзя задать пустое имя", !player.renameAnimal(0, "   ") && player.getHerd()[0].getName() == "Бурёнка");
    check("Имена: нельзя дать имя другого животного", !player.renameAnimal(0, "Корова 2"));
    check("Имена: неверный индекс отклоняется", !player.renameAnimal(-1, "Имя") && !player.renameAnimal(50, "Имя"));
    check("Имена: отклоняется слишком длинное имя", !player.renameAnimal(0, string(61, 'a')));
    check("Имена: переименование не расходует действия", player.getActionsLeft() == 3);
    check("Имена: вид животного после переименования не меняется", player.getHerd()[0].getSpecies() == "Корова");
    check("Имена: можно переименовать обратно", player.renameAnimal(0, "Корова 1"));

    check("Player: кормление без корма не проходит", !player.feedAnimal(0));
    check("Player: неудачное кормление не тратит действие", player.getActionsLeft() == 3);
    player.addFood("Сено", 1);
    check("Player: кормление с кормом проходит", player.feedAnimal(0));
    check("Player: корм расходуется", player.getFood("Сено") == 0);
    check("Player: действие тратится", player.getActionsLeft() == 2);
    check("Player: животное стало менее голодным", player.getHerd()[0].getHunger() == 0);
    check("Player: нельзя кормить несуществующее животное", !player.feedAnimal(10) && !player.feedAnimal(-1));

    check("Player: забота о животном", player.careAnimal(1) && player.getHerd()[1].getMood() == 100);
    check("Player: покупка корма", player.buyFood(hay) && player.getFood("Сено") == FOOD_PACK);
    check("Player: покупка корма списывает деньги", player.getMoney() == 100 - 5 * FOOD_PACK);
    check("Player: действия закончились", player.getActionsLeft() == 0);
    check("Player: без действий ничего нельзя", !player.careAnimal(0) && !player.buyFood(hay) && !player.buyAnimal("Курица"));

    Player poor(10, 5);
    check("Player: нехватка денег на корм", !poor.buyFood(hay) && poor.getActionsLeft() == 5);
    check("Player: нехватка денег на животное", !poor.buyAnimal("Корова") && poor.getHerd().empty());

    Player rich(100, 5);
    check("Player: покупка животного", rich.buyAnimal("Корова") && rich.getHerd().size() == 1);
    check("Player: покупка животного списывает цену", rich.getMoney() == 100 - priceForSpecies("Корова"));

    Player farm(0, 5);
    farm.addAnimal("Корова");
    farm.addAnimal("Курица");
    DayReport report = farm.endDay(20);
    check("Player: доход за день", report.income == incomeForSpecies("Корова") + incomeForSpecies("Курица"));
    check("Player: доход добавляется к деньгам", farm.getMoney() == report.income);
    check("Player: после дня действий нет", farm.getActionsLeft() == 0);
    check("Player: животные подросли в голоде", farm.getHerd()[0].getHunger() == START_HUNGER + 20);
    farm.startDay(5);
    check("Player: новый день даёт действия", farm.getActionsLeft() == 5);
    report = farm.endDay(100);
    check("Player: погибшие животные удаляются", farm.getHerd().empty() && report.diedNames.size() == 2);
    check("Player: имена погибших в отчёте", report.diedNames.size() == 2 && report.diedNames[0] == "Корова 1");
    Player scorer(50, 1);
    scorer.addAnimal("Корова");
    check("Player: итоговый счёт", scorer.getScore() == 50 + ANIMAL_SCORE);

    removeFile();
    out = run("5\n");
    check("Выход из программы", has(out, "Программа завершена"));
    check("Заставка: ферма показывается при запуске", has(out, farmArt()) && has(out, "ДОБРО ПОЖАЛОВАТЬ"));

    removeFile();
    out = run(wholeGame());
    check("Пункт 1: начало игры", has(out, "Игра началась"));
    check("Пункт 1: возврат в главное меню", countOf(out, "1. Начать игру") == 2);
    check("Пункт 1: показан первый день", has(out, "День 1 из 7"));
    check("Пункт 1: при старте игры показаны корова и курица", has(out, "(oo)") && has(out, "|| ||"));
    check("Пункт 1: показаны итоги", has(out, "ИТОГИ ИГРЫ") && has(out, "Итоговый счёт"));

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
    check("Пункт 4: номер лабораторной", has(out, "Лабораторная работа №3"));
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
    check("Первый запуск: значения по умолчанию в файле",
          has(readFile(), "difficulty=Нормально") && has(readFile(), "animal.Корова=3") && has(readFile(), "animal.Курица=5"));

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

    removeFile();
    out = run("3\n3\nСвинья\n7\n4\n5\n");
    check("Новый вид: добавлен и записан в файл", animals["Свинья"] == 7 && has(readFile(), "animal.Свинья=7"));

    removeFile();
    out = run("3\n3\nКорова\n4\n5\n");
    check("Новый вид: повтор названия не принимается", has(out, "такой вид уже есть") && animals["Корова"] == 3);

    removeFile();
    out = run("3\n3\nУтка\nabc\n4\n5\n");
    check("Новый вид: неверное количество не принимается", animals.count("Утка") == 0 && !has(readFile(), "Утка"));

    removeFile();
    out = run("3\n3\n   \n4\n5\n");
    check("Новый вид: пустое название не принимается", has(out, "неверное название"));

    removeFile();
    run("3\n1\n1\n3\nЛошадь\n4\n4\n5\n");
    out = run(wholeGame());
    check("Перезапуск: сложность загружена из файла", has(out, "Уровень сложности: Легко"));
    check("Перезапуск: новый вид загружен из файла", has(out, "Лошадь: 4"));
    check("Перезапуск: второй запуск не создаёт файл заново", !has(out, "Создан"));

    removeFile();
    writeFile("difficulty=Сложно\nanimal.Овца=8\n");
    out = run(wholeGame());
    check("Загрузка: сложность из файла", has(out, "Уровень сложности: Сложно"));
    check("Загрузка: животное из файла", has(out, "Овца: 8"));
    check("Загрузка: животных по умолчанию нет", !has(out, "Корова"));

    removeFile();
    writeFile("мусор\ndifficulty=Невозможно\nanimal.Корова=abc\nanimal.Курица=5\n");
    out = run("5\n");
    check("Ошибки в файле: есть предупреждение", has(out, "Предупреждение"));
    check("Ошибки в файле: сложность по умолчанию", difficulty == "Нормально");
    check("Ошибки в файле: верная строка прочитана, неверная пропущена",
          animals.count("Курица") == 1 && animals.count("Корова") == 0);
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
    writeFile("difficulty=Легко\nanimal.Овца=2\n");
    out = run(wholeGame());
    check("Игра: действий в день по сложности Легко", has(out, "Действий осталось: 8"));
    check("Игра: животные создаются из настроек", has(out, "Овца 1") && has(out, "Овца 2") && !has(out, "Овца 3"));
    check("Игра: стартовые деньги по сложности Легко", has(out, "Деньги: 250"));

    removeFile();
    writeFile("difficulty=Сложно\nanimal.Корова=1\n");
    out = run(wholeGame());
    check("Игра: действий в день по сложности Сложно", has(out, "Действий осталось: 5"));
    check("Игра: стартовые деньги по сложности Сложно", has(out, "Деньги: 120"));

    removeFile();
    writeFile("difficulty=Нормально\nanimal.Корова=3\n");
    out = run("1\n1\n1\n1\n2\n1\n3\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: кормление животного", has(out, "Корова 1 накормлено."));
    check("Игра: при кормлении коровы показан рисунок коровы", countOf(out, "(oo)") == 3 && countOf(out, "|| ||") == 1);
    check("Игра: корм расходуется, при нехватке сообщение", has(out, "Нет корма \"Сено\""));
    check("Игра: после действия их остаётся меньше", has(out, "Действий осталось: 5"));

    out = run("1\n3\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: покупка корма", has(out, "Куплено: Сено, 5 порций."));
    check("Игра: покупка корма списывает деньги", has(out, "Деньги: 155"));

    out = run("1\n4\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: покупка животного", has(out, "Куплено новое животное: Корова 4."));
    check("Игра: покупка животного списывает деньги", has(out, "Деньги: 120"));

    out = run("1\n2\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: забота о животном", has(out, "Вы позаботились о животном: Корова 1."));

    writeFile("difficulty=Сложно\nanimal.Корова=1\n");
    out = run("1\n4\n1\n4\n1\n4\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: нехватка денег на животное", has(out, "Недостаточно денег."));

    out = run("1\n9\n1\n7\n" + repeat("5\n", 7) + "\n5\n");
    check("Игра: неверное действие", has(out, "такого действия нет"));
    check("Игра: неверный номер животного", has(out, "такого животного нет"));

    writeFile("difficulty=Нормально\nanimal.Корова=1\n");
    out = run(wholeGame());
    check("Игра: без ухода животные погибают", has(out, "Животное погибло от голода: Корова 1"));
    check("Игра: когда все погибли, игра заканчивается", has(out, "Все животные погибли"));

    writeFile("difficulty=Легко\nanimal.Корова=1\n");
    string days = "1\n" + repeat("1\n1\n5\n", 2) + "3\n1\n1\n1\n5\n" + repeat("1\n1\n5\n", 4) + "\n5\n";
    out = run(days);
    check("Игра: с уходом животное доживает до конца", has(out, "День 7 из 7") && has(out, "Животных на ферме: 1"));
    check("Игра: итоговый счёт при полном прохождении", has(out, "Итоговый счёт"));
    check("Игра: никто не погиб", !has(out, "погибло"));

    removeFile();
    writeFile("difficulty=Нормально\nanimal.Корова=1\n");
    out = run("1\n6\n1\nБурёнка\n1\n1\n5\n" + repeat("5\n", 6) + "\n5\n");
    check("Имена: пункт переименования доступен в игре", has(out, "6. Дать имя"));
    check("Имена: новое имя отображается в статусе", has(out, "Бурёнка (голод:"));
    check("Имена: новое имя используется при кормлении", has(out, "Бурёнка накормлено."));
    check("Имена: переименование не тратит игровой ход", has(out, "Действий осталось: 6"));

    removeFile();
    writeFile("difficulty=Нормально\nanimal.Кицунэ=1\nanimal.Курица=1\n");
    out = run("1\n6\n2\nЦыпа\n5\n" + repeat("5\n", 6) + "\n5\n");
    check("Имена и кицунэ: выводится пользовательское имя съеденной курицы", has(out, "Кицунэ съела курицу: Цыпа."));

    removeFile();
    out = run(wholeGame());
    check("Кицунэ: без кицунэ в игре никого не съедают", !has(out, "съела"));

    removeFile();
    writeFile("difficulty=Нормально\nanimal.Кицунэ=1\nanimal.Курица=2\n");
    out = run(wholeGame());
    check("Кицунэ: в первый день съедает первую курицу", has(out, "Кицунэ съела курицу: Курица 1."));
    check("Кицунэ: на следующий день съедает вторую курицу", has(out, "Кицунэ съела курицу: Курица 2."));
    check("Кицунэ: показаны настройки с кицунэ", has(out, "Кицунэ: 1") && has(out, "Курица: 2"));

    out = run("1\n1\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Кицунэ: при кормлении показан её рисунок", countOf(out, "( o.o )----(") == 1);
    check("Кицунэ: кицунэ накормлена тофу", has(out, "Кицунэ 1 накормлено.") && has(out, "Тофу 1;"));

    out = run("1\n4\n1\n" + repeat("5\n", 7) + "\n5\n");
    check("Кицунэ: можно купить ещё одну", has(out, "Куплено новое животное: Кицунэ 2.") && has(out, "Деньги: 100"));

    removeFile();
    writeFile("difficulty=Нормально\nanimal.Кицунэ=1\nanimal.Курица=1\n");
    out = run(wholeGame());
    check("Кицунэ: единственную курицу съедают один раз", countOf(out, "съела курицу") == 1);

    removeFile();
    out = run("3\n3\nКицунэ\n1\n4\n5\n");
    check("Кицунэ: добавляется через настройки", animals["Кицунэ"] == 1 && has(readFile(), "animal.Кицунэ=1"));
    check("Кицунэ: в настройках есть подсказка про то, что она ест кур", has(out, "Кицунэ съедает"));

    removeFile();

    cout << "----------------------------------\n";
    cout << "Пройдено: " << passed << ", провалено: " << failed << "\n";

    finished = true;
    return failed == 0 ? 0 : 1;
}
