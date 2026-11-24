#define NOMINMAX
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <random>
#include <limits>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <windows.h>

using namespace std;

const int Start = 1;
const int Load = 2;
const int Setting = 3;
const int Info = 4;
const int Exit = 5;

const int Specialty = 1;
const int Name = 2;
const int Subjects = 3;
const int Back = 4;

const int GAME_DAYS = 5; 
const int SUBJECTS_PER_DAY = 3;
const int HOME_ACTIONS_PER_DAY = 2;
const int MAX_STAT = 100;
const int PASS_GRADE = 4;
const int QUESTIONS_PER_SUBJECT = 2;

const string SAVE_FILE = "saves.txt";
const int SLOT_COUNT = 3;
const string EMPTY_SLOT = "EMPTY";
const char SEPARATOR = ';';
const int FIELDS_BEFORE_SUBJECTS = 8;
const int FIELDS_PER_SUBJECT = 6;

const int CAT_MATH = 0;
const int CAT_PROG = 1;
const int CAT_ELEC = 2;
const int CAT_HUM = 3;
const int CAT_IS = 4;

struct SETTING {
    string specialty = "Программная инженерия";
    string studentName = "Аня";
    int subjectCount = 11;
};

struct Question {
    string text;
    vector<string> options;
    int correct;
};

struct SubjectInfo {
    string name;
    int category;
    int difficulty; // 1..5
};

class Subject {
private:
    string name;
    int category;
    int difficulty;
    int knowledge;
    int attended;
    int skipped;

public:
    Subject(const SubjectInfo& info)
        : name(info.name), category(info.category), difficulty(info.difficulty),
        knowledge(0), attended(0), skipped(0) {
    }

    Subject(const string& subjectName, int subjectCategory, int subjectDifficulty,
        int subjectKnowledge, int subjectAttended, int subjectSkipped)
        : name(subjectName), category(subjectCategory), difficulty(subjectDifficulty),
        knowledge(subjectKnowledge), attended(subjectAttended), skipped(subjectSkipped) {
    }

    string getName() const { return name; }
    int getCategory() const { return category; }
    int getDifficulty() const { return difficulty; }
    int getKnowledge() const { return knowledge; }
    int getAttended() const { return attended; }
    int getSkipped() const { return skipped; }

    void attend(int gain) {
        attended++;
        knowledge += gain;
    }
    void skip() { skipped++; }
    void addKnowledge(int gain) { knowledge += gain; }
};

class Student {
private:
    string name;
    string specialty;
    int energy;
    int mood;
    int skills;

public:
    Student() : name(""), specialty(""), energy(MAX_STAT), mood(50), skills(0) {}

    Student(const string& studentName, const string& studentSpecialty)
        : name(studentName), specialty(studentSpecialty), energy(MAX_STAT), mood(50), skills(0) {
    }

    Student(const string& studentName, const string& studentSpecialty,
        int studentEnergy, int studentMood, int studentSkills)
        : name(studentName), specialty(studentSpecialty),
        energy(studentEnergy), mood(studentMood), skills(studentSkills) {
    }

    string getName() const { return name; }
    string getSpecialty() const { return specialty; }
    int getEnergy() const { return energy; }
    int getMood() const { return mood; }
    int getSkills() const { return skills; }

    void changeEnergy(int delta) {
        energy = max(0, min(MAX_STAT, energy + delta));
    }
    void changeMood(int delta) {
        mood = max(0, min(MAX_STAT, mood + delta));
    }
    void addSkills(int delta) {
        skills = max(0, skills + delta);
    }

    void showStatus() const {
        cout << " Студент: " << name << " (" << specialty << ")" << endl;
        cout << " Энергия: " << energy << "/" << MAX_STAT
            << "   Настроение: " << mood << "/" << MAX_STAT
            << "   Навыки: " << skills << endl;
    }
};

class Action {
private:
    string name;
    string description;
    int energyCost; 
    int moodChange;
    int knowledgeGain;
    int skillsGain;
    bool needsSubject;

public:
    Action(const string& actionName, const string& actionDescription,
        int cost, int mood, int knowledge, int skillsAdd, bool subjectNeeded)
        : name(actionName), description(actionDescription), energyCost(cost),
        moodChange(mood), knowledgeGain(knowledge), skillsGain(skillsAdd),
        needsSubject(subjectNeeded) {
    }

    string getName() const { return name; }
    string getDescription() const { return description; }
    bool requiresSubject() const { return needsSubject; }

    bool canDo(const Student& st) const {
        return energyCost <= 0 || st.getEnergy() >= energyCost;
    }

    void apply(Student& st, Subject* subject) const {
        st.changeEnergy(-energyCost);
        st.changeMood(moodChange);
        st.addSkills(skillsGain);
        if (subject != nullptr) {
            subject->addKnowledge(knowledgeGain);
        }
    }
};

mt19937 rng{ random_device{}() };

map<string, vector<SubjectInfo>> createCurriculum() {
    map<string, vector<SubjectInfo>> c;

    c["Программная инженерия"] = {
        {"Математический анализ", CAT_MATH, 4},
        {"Дискретная математика", CAT_MATH, 4},
        {"Алгоритмы и структуры данных", CAT_PROG, 4},
        {"Объектно-ориентированное программирование", CAT_PROG, 3},
        {"Базы данных", CAT_IS, 3},
        {"Конструирование ПО", CAT_PROG, 3},
        {"Операционные системы", CAT_PROG, 4},
        {"Компьютерные сети", CAT_ELEC, 3},
        {"Английский язык", CAT_HUM, 2},
        {"История России", CAT_HUM, 2},
        {"Философия", CAT_HUM, 2},
        {"Тестирование ПО", CAT_PROG, 3}
    };

    c["Искусственный интеллект"] = {
        {"Линейная алгебра", CAT_MATH, 4},
        {"Теория вероятностей", CAT_MATH, 4},
        {"Машинное обучение", CAT_PROG, 5},
        {"Python для анализа данных", CAT_PROG, 3},
        {"Нейронные сети", CAT_PROG, 5},
        {"Математическая статистика", CAT_MATH, 4},
        {"Алгоритмы и структуры данных", CAT_PROG, 4},
        {"Базы данных", CAT_IS, 3},
        {"Обработка естественного языка", CAT_PROG, 4},
        {"Английский язык", CAT_HUM, 2},
        {"Философия", CAT_HUM, 2},
        {"Этика искусственного интеллекта", CAT_HUM, 1}
    };

    c["Системы управления информацией"] = {
        {"Информационные системы", CAT_IS, 3},
        {"Базы данных", CAT_IS, 3},
        {"Управление проектами", CAT_HUM, 2},
        {"Бизнес-аналитика", CAT_IS, 3},
        {"Проектирование информационных систем", CAT_IS, 4},
        {"Математическая статистика", CAT_MATH, 3},
        {"Программирование на Java", CAT_PROG, 3},
        {"Информационная безопасность", CAT_IS, 4},
        {"Экономика", CAT_HUM, 2},
        {"Английский язык", CAT_HUM, 2},
        {"Документооборот", CAT_IS, 2},
        {"История России", CAT_HUM, 2}
    };

    c["Электронные системы и технологии"] = {
        {"Физика", CAT_ELEC, 4},
        {"Электротехника", CAT_ELEC, 4},
        {"Схемотехника", CAT_ELEC, 5},
        {"Микропроцессорные системы", CAT_ELEC, 4},
        {"Математический анализ", CAT_MATH, 4},
        {"Материаловедение", CAT_ELEC, 3},
        {"Метрология", CAT_ELEC, 3},
        {"Цифровая обработка сигналов", CAT_ELEC, 5},
        {"Программирование на C", CAT_PROG, 3},
        {"Английский язык", CAT_HUM, 2},
        {"Безопасность жизнедеятельности", CAT_HUM, 1},
        {"Философия", CAT_HUM, 2}
    };

    c["Компьютерная инженерия"] = {
        {"Архитектура ЭВМ", CAT_ELEC, 4},
        {"Цифровая электроника", CAT_ELEC, 4},
        {"Программирование на C++", CAT_PROG, 3},
        {"Дискретная математика", CAT_MATH, 4},
        {"Операционные системы", CAT_PROG, 4},
        {"Компьютерные сети", CAT_ELEC, 3},
        {"Встраиваемые системы", CAT_ELEC, 5},
        {"Математический анализ", CAT_MATH, 4},
        {"Алгоритмы и структуры данных", CAT_PROG, 4},
        {"Английский язык", CAT_HUM, 2},
        {"Физика", CAT_ELEC, 3},
        {"История России", CAT_HUM, 2}
    };

    return c;
}

const vector<Question>& getQuestionPool(int category) {
    static const map<int, vector<Question>> pools = {
        { CAT_MATH, {
            {"Чему равна производная функции x^2?", {"x", "2x", "x^2", "2"}, 2},
            {"Чему равен определитель единичной матрицы 2x2?", {"0", "2", "1", "-1"}, 3},
            {"Сколько существует перестановок из 3 элементов?", {"3", "6", "9", "12"}, 2},
            {"Какова вероятность выпадения орла при броске честной монеты?", {"1/2", "1/3", "1/4", "1"}, 1}
        } },
        { CAT_PROG, {
            {"Какой принцип ООП скрывает внутреннее устройство объекта?", {"Наследование", "Полиморфизм", "Инкапсуляция", "Компиляция"}, 3},
            {"Какова сложность бинарного поиска?", {"O(n)", "O(log n)", "O(n^2)", "O(1)"}, 2},
            {"Какая структура данных работает по принципу LIFO?", {"Очередь", "Стек", "Множество", "Список"}, 2},
            {"Что вызывается при создании объекта класса?", {"Деструктор", "Конструктор", "Оператор", "Макрос"}, 2}
        } },
        { CAT_ELEC, {
            {"Как записывается закон Ома для участка цепи?", {"I = U / R", "I = U * R", "I = R / U", "U = I / R"}, 1},
            {"В чём измеряется электрическое сопротивление?", {"Вольт", "Ампер", "Ом", "Ватт"}, 3},
            {"Сколько бит в одном байте?", {"4", "8", "16", "10"}, 2},
            {"Какой элемент пропускает ток только в одном направлении?", {"Резистор", "Конденсатор", "Диод", "Катушка"}, 3}
        } },
        { CAT_HUM, {
            {"В каком году началась Великая Отечественная война?", {"1939", "1941", "1945", "1914"}, 2},
            {"Как переводится слово knowledge?", {"Сила", "Знание", "Время", "Работа"}, 2},
            {"Кто автор «Критики чистого разума»?", {"Кант", "Платон", "Гегель", "Декарт"}, 1},
            {"Кто основал Санкт-Петербург?", {"Пётр I", "Иван III", "Екатерина II", "Николай I"}, 1}
        } },
        { CAT_IS, {
            {"Какой оператор SQL выбирает данные из таблицы?", {"INSERT", "SELECT", "UPDATE", "DELETE"}, 2},
            {"Что такое первичный ключ?", {"Уникальный идентификатор записи", "Пароль пользователя", "Тип данных", "Вид индекса сортировки"}, 1},
            {"Какое свойство защиты информации означает её неизменность?", {"Конфиденциальность", "Целостность", "Доступность", "Скорость"}, 2},
            {"Что такое UML?", {"Язык моделирования", "Язык запросов", "Операционная система", "Сетевой протокол"}, 1}
        } }
    };
    return pools.at(category);
}

vector<Action> createActions() {
    vector<Action> actions;
    actions.push_back(Action("Учиться", "знания по предмету +3, энергия -15, настроение -3", 15, -3, 3, 0, true));
    actions.push_back(Action("Решать задачи", "навыки +2, энергия -15, настроение -2", 15, -2, 0, 2, false));
    actions.push_back(Action("Отдых", "энергия +30, настроение +5", -30, 5, 0, 0, false));
    actions.push_back(Action("Прогулка с друзьями", "настроение +15, энергия -5", 5, 15, 0, 0, false));
    return actions;
}

void startGame(const SETTING& userSet);
void loadGame();
void settingGame(SETTING& userSet, set<string>& specialtiesSet, list<string>& namesList);
void infoGame();
void exitGame();
void saveSettings(const SETTING& userSet);
void loadSettings(SETTING& userSet);
void loadSpecialties(set<string>& specialtiesSet);
void loadNames(list<string>& namesList);

int readInt(int lo, int hi);
void pauseScreen();
void playMorning(Student& st, vector<Subject>& subjects, int day);
void playEvening(Student& st, vector<Subject>& subjects, const vector<Action>& actions);
void runSession(Student& st, vector<Subject>& subjects);
void runGame(int startDay, Student& student, vector<Subject>& subjects);
bool dayMenu(int day, const Student& st, const vector<Subject>& subjects);

string currentTimeString();
vector<string> splitString(const string& text, char separator);
string serializeGame(int day, const Student& st, const vector<Subject>& subjects);
bool deserializeGame(const string& line, int& day, Student& st, vector<Subject>& subjects);
vector<string> readAllSlots();
void writeAllSlots(const vector<string>& slots);
bool isSlotEmpty(const string& line);
void showSlots(const vector<string>& slots);
bool saveGameToSlot(int slot, int day, const Student& st, const vector<Subject>& subjects);
bool loadGameFromSlot(int slot, int& day, Student& st, vector<Subject>& subjects);
void saveMenu(int day, const Student& st, const vector<Subject>& subjects);

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SETTING userSetting;
    loadSettings(userSetting);

    set<string> specialtiesSet;
    list<string> namesList;
    loadSpecialties(specialtiesSet);
    loadNames(namesList);

    int choice;

    while (true) {
        system("cls");
        cout << " ------------------------------" << endl;
        cout << "    МОЯ ЖИЗНЬ В УНИВЕРСИТЕТЕ   " << endl;
        cout << " ------------------------------" << endl;
        cout << "   " << Start << ". Начать игру              " << endl;
        cout << "   " << Load << ". Загрузить игру           " << endl;
        cout << "   " << Setting << ". Настройки                " << endl;
        cout << "   " << Info << ". О программе              " << endl;
        cout << "   " << Exit << ". Выход                    " << endl;
        cout << " ------------------------------" << endl;
        cout << "   Ваш выбор (1-5): ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            continue;
        }

        switch (choice) {
        case Start:
            startGame(userSetting);
            break;
        case Load:
            loadGame();
            break;
        case Setting:
            settingGame(userSetting, specialtiesSet, namesList);
            break;
        case Info:
            infoGame();
            break;
        case Exit:
            exitGame();
            return 0;
            break;
        }
    }
    return 0;
}

string currentTimeString() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", &localTime);
    return string(buffer);
}

vector<string> splitString(const string& text, char separator) {
    vector<string> parts;
    string current;
    for (char ch : text) {
        if (ch == separator) {
            parts.push_back(current);
            current.clear();
        }
        else {
            current += ch;
        }
    }
    parts.push_back(current);
    return parts;
}

string serializeGame(int day, const Student& st, const vector<Subject>& subjects) {
    ostringstream out;
    out << currentTimeString() << SEPARATOR
        << day << SEPARATOR
        << st.getName() << SEPARATOR
        << st.getSpecialty() << SEPARATOR
        << st.getEnergy() << SEPARATOR
        << st.getMood() << SEPARATOR
        << st.getSkills() << SEPARATOR
        << subjects.size();
    for (const auto& subj : subjects) {
        out << SEPARATOR << subj.getName()
            << SEPARATOR << subj.getCategory()
            << SEPARATOR << subj.getDifficulty()
            << SEPARATOR << subj.getKnowledge()
            << SEPARATOR << subj.getAttended()
            << SEPARATOR << subj.getSkipped();
    }
    return out.str();
}

bool deserializeGame(const string& line, int& day, Student& st, vector<Subject>& subjects) {
    vector<string> f = splitString(line, SEPARATOR);
    if (f.size() < FIELDS_BEFORE_SUBJECTS) {
        return false;
    }
    try {
        int loadedDay = stoi(f[1]);
        int energy = stoi(f[4]);
        int mood = stoi(f[5]);
        int skills = stoi(f[6]);
        int count = stoi(f[7]);

        if (loadedDay < 0 || loadedDay >= GAME_DAYS || count < 1) {
            return false;
        }
        if (f.size() != static_cast<size_t>(FIELDS_BEFORE_SUBJECTS + FIELDS_PER_SUBJECT * count)) {
            return false;
        }

        vector<Subject> loadedSubjects;
        for (int i = 0; i < count; i++) {
            size_t p = FIELDS_BEFORE_SUBJECTS + static_cast<size_t>(i) * FIELDS_PER_SUBJECT;
            int category = stoi(f[p + 1]);
            if (category < CAT_MATH || category > CAT_IS) {
                return false;
            }
            loadedSubjects.push_back(Subject(f[p], category, stoi(f[p + 2]),
                stoi(f[p + 3]), stoi(f[p + 4]), stoi(f[p + 5])));
        }

        day = loadedDay;
        st = Student(f[2], f[3], energy, mood, skills);
        subjects = loadedSubjects;
        return true;
    }
    catch (...) {
        return false; // нечисловые данные в файле
    }
}

vector<string> readAllSlots() {
    vector<string> slots(SLOT_COUNT, EMPTY_SLOT);
    ifstream inFile(SAVE_FILE);
    if (inFile.is_open()) {
        string line;
        for (int i = 0; i < SLOT_COUNT && getline(inFile, line); i++) {
            if (!line.empty()) {
                slots[i] = line;
            }
        }
        inFile.close();
    }
    return slots;
}

void writeAllSlots(const vector<string>& slots) {
    ofstream outFile(SAVE_FILE);
    if (outFile.is_open()) {
        for (const auto& line : slots) {
            outFile << line << endl;
        }
        outFile.close();
    }
}

bool isSlotEmpty(const string& line) {
    return line.empty() || line == EMPTY_SLOT;
}

void showSlots(const vector<string>& slots) {
    for (int i = 0; i < SLOT_COUNT; i++) {
        cout << "   Слот " << i + 1 << ": ";
        if (isSlotEmpty(slots[i])) {
            cout << "пусто" << endl;
            continue;
        }
        vector<string> f = splitString(slots[i], SEPARATOR);
        if (f.size() < FIELDS_BEFORE_SUBJECTS) {
            cout << "повреждено" << endl;
            continue;
        }
        int dayNumber = 0;
        try {
            dayNumber = stoi(f[1]) + 1;
        }
        catch (...) {
            cout << "повреждено" << endl;
            continue;
        }
        cout << f[0] << " | " << f[2] << " | день " << dayNumber << " из " << GAME_DAYS << endl;
    }
}

bool saveGameToSlot(int slot, int day, const Student& st, const vector<Subject>& subjects) {
    vector<string> slots = readAllSlots();
    slots[slot] = serializeGame(day, st, subjects);
    writeAllSlots(slots);

    return readAllSlots()[slot] == slots[slot];
}

bool loadGameFromSlot(int slot, int& day, Student& st, vector<Subject>& subjects) {
    vector<string> slots = readAllSlots();
    if (isSlotEmpty(slots[slot])) {
        return false;
    }
    return deserializeGame(slots[slot], day, st, subjects);
}

void saveMenu(int day, const Student& st, const vector<Subject>& subjects) {
    system("cls");
    cout << " ==============================================" << endl;
    cout << "               СОХРАНЕНИЕ ИГРЫ                 " << endl;
    cout << " ==============================================" << endl;
    showSlots(readAllSlots());
    cout << "   " << SLOT_COUNT + 1 << ". Назад" << endl;
    cout << " ----------------------------------------------" << endl;
    cout << "   Выберите слот: ";
    int choice = readInt(1, SLOT_COUNT + 1);
    if (choice == SLOT_COUNT + 1) {
        return;
    }
    int slot = choice - 1;

    if (!isSlotEmpty(readAllSlots()[slot])) {
        cout << "\n   Слот занят. Перезаписать?" << endl;
        cout << "   1. Да" << endl;
        cout << "   2. Нет" << endl;
        cout << "   Ваш выбор: ";
        if (readInt(1, 2) == 2) {
            return;
        }
    }

    if (saveGameToSlot(slot, day, st, subjects)) {
        cout << "\n   Игра сохранена в слот " << choice << "!" << endl;
    }
    else {
        cout << "\n   Не удалось сохранить игру." << endl;
    }
    pauseScreen();
}

int readInt(int lo, int hi) {
    int value;
    while (true) {
        if (cin >> value && value >= lo && value <= hi) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << " Введите число от " << lo << " до " << hi << ": ";
    }
}

void pauseScreen() {
    cout << "\n Нажмите Enter для продолжения...";
    cin.get();
}

bool dayMenu(int day, const Student& st, const vector<Subject>& subjects) {
    while (true) {
        system("cls");
        cout << " ==============================================" << endl;
        cout << "   ДЕНЬ " << day + 1 << " из " << GAME_DAYS << endl;
        cout << " ==============================================" << endl;
        st.showStatus();
        cout << " ----------------------------------------------" << endl;
        cout << "   1. Продолжить" << endl;
        cout << "   2. Сохранить игру" << endl;
        cout << "   3. Выйти в главное меню" << endl;
        cout << " ----------------------------------------------" << endl;
        cout << "   Ваш выбор: ";
        int choice = readInt(1, 3);

        if (choice == 1) {
            return true;
        }
        if (choice == 2) {
            saveMenu(day, st, subjects);
        }
        else {
            return false;
        }
    }
}

void playMorning(Student& st, vector<Subject>& subjects, int day) {
    int n = static_cast<int>(subjects.size());
    int perDay = min(SUBJECTS_PER_DAY, n);

    system("cls");
    cout << " ==============================================" << endl;
    cout << "   ДЕНЬ " << day + 1 << " из " << GAME_DAYS << " - УТРО" << endl;
    cout << " ==============================================" << endl;
    st.showStatus();
    cout << " ----------------------------------------------" << endl;
    cout << " Сегодня в расписании " << perDay << " пары.\n" << endl;

    for (int k = 0; k < perDay; k++) {
        Subject& subj = subjects[(day * perDay + k) % n];

        cout << " Пара " << k + 1 << ": " << subj.getName()
            << " (сложность " << subj.getDifficulty()
            << ", ваши знания: " << subj.getKnowledge() << ")" << endl;
        cout << "   1. Пойти на пару (энергия -10, настроение -2)" << endl;
        cout << "   2. Прогулять (настроение +3)" << endl;
        cout << "   Ваш выбор: ";
        int choice = readInt(1, 2);

        if (choice == 1 && st.getEnergy() < 10) {
            cout << "   Не хватает сил, чтобы идти на пару. Пара пропущена." << endl;
            subj.skip();
        }
        else if (choice == 1) {
            int gain = 2 + st.getEnergy() / 50; // чем бодрее студент, тем больше усвоил
            subj.attend(gain);
            st.changeEnergy(-10);
            st.changeMood(-2);
            cout << "   Вы сходили на пару. Знания по предмету +" << gain << endl;
        }
        else {
            subj.skip();
            st.changeMood(3);
            cout << "   Вы прогуляли пару. Знания не изменились." << endl;
        }
        cout << endl;
    }
    pauseScreen();
}

void playEvening(Student& st, vector<Subject>& subjects, const vector<Action>& actions) {
    int slot = 0;
    while (slot < HOME_ACTIONS_PER_DAY) {
        system("cls");
        cout << " ==============================================" << endl;
        cout << "   ВЕЧЕР: действие " << slot + 1 << " из " << HOME_ACTIONS_PER_DAY << endl;
        cout << " ==============================================" << endl;
        st.showStatus();
        cout << " ----------------------------------------------" << endl;
        for (size_t i = 0; i < actions.size(); i++) {
            cout << "   " << i + 1 << ". " << actions[i].getName()
                << " (" << actions[i].getDescription() << ")" << endl;
        }
        cout << "   Ваш выбор: ";
        int choice = readInt(1, static_cast<int>(actions.size()));
        const Action& act = actions[choice - 1];

        if (!act.canDo(st)) {
            cout << "\n Вы слишком устали для этого действия. Выберите другое." << endl;
            pauseScreen();
            continue;
        }

        Subject* chosen = nullptr;
        if (act.requiresSubject()) {
            cout << "\n Какой предмет учить?" << endl;
            for (size_t i = 0; i < subjects.size(); i++) {
                cout << "   " << i + 1 << ". " << subjects[i].getName()
                    << " (знания: " << subjects[i].getKnowledge() << ")" << endl;
            }
            cout << "   Ваш выбор: ";
            int s = readInt(1, static_cast<int>(subjects.size()));
            chosen = &subjects[s - 1];
        }

        act.apply(st, chosen);
        cout << "\n Вы выбрали: " << act.getName();
        if (chosen != nullptr) {
            cout << " (" << chosen->getName() << ")";
        }
        cout << endl;
        slot++;
        pauseScreen();
    }
}

void runSession(Student& st, vector<Subject>& subjects) {
    system("cls");
    cout << " ==============================================" << endl;
    cout << "                    СЕССИЯ                    " << endl;
    cout << " ==============================================" << endl;
    st.showStatus();
    cout << " ----------------------------------------------" << endl;
    cout << " За каждый правильный ответ на вопросы по предмету" << endl;
    cout << " вы получаете +1 балл к оценке." << endl;
    pauseScreen();

    vector<int> grades;

    for (auto& subj : subjects) {
        system("cls");
        cout << " ==============================================" << endl;
        cout << "  Экзамен: " << subj.getName() << endl;
        cout << " ==============================================" << endl;

        const vector<Question>& pool = getQuestionPool(subj.getCategory());
        vector<int> order(pool.size());
        iota(order.begin(), order.end(), 0);
        shuffle(order.begin(), order.end(), rng);
        int count = min(QUESTIONS_PER_SUBJECT, static_cast<int>(pool.size()));

        int correct = 0;
        for (int q = 0; q < count; q++) {
            const Question& question = pool[order[q]];
            cout << "\n Вопрос " << q + 1 << ": " << question.text << endl;
            for (size_t i = 0; i < question.options.size(); i++) {
                cout << "   " << i + 1 << ". " << question.options[i] << endl;
            }
            cout << "   Ваш ответ: ";
            int answer = readInt(1, static_cast<int>(question.options.size()));
            if (answer == question.correct) {
                cout << "   Верно! +1 балл" << endl;
                correct++;
            }
            else {
                cout << "   Неверно. Правильный ответ: " << question.options[question.correct - 1] << endl;
            }
        }

        double score = 1.0
            + subj.getKnowledge() * 0.5
            + st.getSkills() * 0.25
            + st.getMood() / 25.0
            - subj.getDifficulty() * 0.5
            + correct;
        int grade = static_cast<int>(lround(score));
        grade = max(0, min(10, grade));

        grades.push_back(grade);

        cout << "\n Ваша оценка: " << grade << "/10"
            << (grade >= PASS_GRADE ? " (зачтено)" : " (не зачтено)") << endl;
        pauseScreen();
    }

    system("cls");
    cout << " ==============================================" << endl;
    cout << "              ИТОГИ СЕССИИ                    " << endl;
    cout << " ==============================================" << endl;
    cout << " " << left << setw(44) << "Предмет" << setw(8) << "Знания" << "Оценка" << endl;
    cout << " ----------------------------------------------------------" << endl;

    int sum = 0;
    int failed = 0;
    for (size_t i = 0; i < subjects.size(); i++) {
        cout << " " << left << setw(44) << subjects[i].getName()
            << setw(8) << subjects[i].getKnowledge()
            << grades[i] << (grades[i] >= PASS_GRADE ? "" : "  (долг)") << endl;
        sum += grades[i];
        if (grades[i] < PASS_GRADE) failed++;
    }
    cout << " ----------------------------------------------------------" << endl;
    cout << fixed << setprecision(2);
    cout << " Средний балл: " << static_cast<double>(sum) / subjects.size() << " / 10" << endl;

    if (failed == 0) {
        cout << "\n " << st.getName() << ", поздравляем! Сессия сдана без задолженностей!" << endl;
    }
    else {
        cout << "\n " << st.getName() << ", сессия не сдана. Задолженностей: " << failed << endl;
    }
    cout.unsetf(ios::floatfield);
    pauseScreen();
}

void runGame(int startDay, Student& student, vector<Subject>& subjects) {
    vector<Action> actions = createActions();

    for (int day = startDay; day < GAME_DAYS; day++) {
        if (!dayMenu(day, student, subjects)) {
            return; // выход в главное меню
        }

        playMorning(student, subjects, day);
        playEvening(student, subjects, actions);

        student.changeEnergy(20);
        system("cls");
        cout << " ==============================================" << endl;
        cout << "   ИТОГ ДНЯ " << day + 1 << endl;
        cout << " ==============================================" << endl;
        cout << " Ночью вы поспали (энергия +20)." << endl;
        student.showStatus();
        pauseScreen();
    }

    runSession(student, subjects);
}

void startGame(const SETTING& userSet) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    map<string, vector<SubjectInfo>> curriculum = createCurriculum();
    auto it = curriculum.find(userSet.specialty);
    if (it == curriculum.end()) {
        it = curriculum.find("Программная инженерия");
    }
    const vector<SubjectInfo>& plan = it->second;

    int count = max(1, userSet.subjectCount);
    bool reduced = false;
    if (count > static_cast<int>(plan.size())) {
        count = static_cast<int>(plan.size());
        reduced = true;
    }

    vector<Subject> subjects;
    for (int i = 0; i < count; i++) {
        subjects.push_back(Subject(plan[i]));
    }

    Student student(userSet.studentName, userSet.specialty);

    system("cls");
    cout << " ==============================================" << endl;
    cout << "          МОЯ ЖИЗНЬ В УНИВЕРСИТЕТЕ             " << endl;
    cout << " ==============================================" << endl;
    cout << " Студент: " << student.getName() << endl;
    cout << " Специальность: " << student.getSpecialty() << endl;
    cout << " Предметов в семестре: " << count << endl;
    if (reduced) {
        cout << " (по этой специальности доступно только " << plan.size()
            << " предметов, число уменьшено)" << endl;
    }
    cout << " ----------------------------------------------" << endl;
    cout << " До сессии осталось " << GAME_DAYS << " дней." << endl;
    cout << " Утром выбирайте: идти на пары или прогуливать." << endl;
    cout << " Вечером занимайтесь делами дома и следите за энергией." << endl;
    cout << " На сессии оценка зависит от знаний, навыков," << endl;
    cout << " настроения и ответов на вопросы. Зачёт - от " << PASS_GRADE << " баллов." << endl;
    cout << " Сохраниться можно в начале любого дня." << endl;
    pauseScreen();

    runGame(0, student, subjects);
}

void loadGame() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    system("cls");
    cout << " ==============================================" << endl;
    cout << "                ЗАГРУЗКА ИГРЫ                  " << endl;
    cout << " ==============================================" << endl;
    showSlots(readAllSlots());
    cout << "   " << SLOT_COUNT + 1 << ". Назад" << endl;
    cout << " ----------------------------------------------" << endl;
    cout << "   Выберите слот: ";
    int choice = readInt(1, SLOT_COUNT + 1);
    if (choice == SLOT_COUNT + 1) {
        return;
    }

    int day = 0;
    Student student;
    vector<Subject> subjects;

    if (isSlotEmpty(readAllSlots()[choice - 1])) {
        cout << "\n   Этот слот пуст." << endl;
        pauseScreen();
        return;
    }
    if (!loadGameFromSlot(choice - 1, day, student, subjects)) {
        cout << "\n   Не удалось загрузить сохранение: данные повреждены." << endl;
        pauseScreen();
        return;
    }

    runGame(day, student, subjects);
}

void settingGame(SETTING& userSet, set<string>& specialtiesSet, list<string>& namesList) {
    int choice;
    while (true) {
        system("cls");
        cout << " ---------------------------------------------" << endl;
        cout << "                  НАСТРОЙКИ                   " << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  1. Специальность: " << userSet.specialty << endl;
        cout << "  2. Имя студента: " << userSet.studentName << endl;
        cout << "  3. Количество предметов: " << userSet.subjectCount << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  4. Назад в главное меню       " << endl;
        cout << " ---------------------------------------------" << endl;
        cout << "  Выберите пункт для изменения: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore();
            continue;
        }
        cin.ignore();

        switch (choice) {
        case Specialty: {
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Доступные специальности:" << endl;
            int index = 1;
            for (const auto& sp : specialtiesSet) {
                cout << "  " << index++ << ". " << sp << endl;
            }
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите номер специальности: ";
            int pos;
            if (cin >> pos) {
                auto it = specialtiesSet.begin();
                for (int i = 1; i < pos && it != specialtiesSet.end(); ++i) {
                    ++it;
                }
                if (it != specialtiesSet.end()) {
                    userSet.specialty = *it;
                    saveSettings(userSet);
                }
            }
            cin.ignore();
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
            cin.get();
            break;
        }
        case Name: {
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Доступные имена студентов:" << endl;
            int index = 1;
            for (const auto& name : namesList) {
                cout << "  " << index++ << ". " << name << endl;
            }
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите номер имени: ";
            int pos;
            if (cin >> pos) {
                auto it = namesList.begin();
                for (int i = 1; i < pos && it != namesList.end(); ++i) {
                    ++it;
                }
                if (it != namesList.end()) {
                    userSet.studentName = *it;
                    saveSettings(userSet);
                }
            }
            cin.ignore();
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
            cin.get();
            break;
        }
        case Subjects:
            system("cls");
            cout << " ---------------------------------------------" << endl;
            cout << "  Введите новое количество предметов: ";
            cin >> userSet.subjectCount;
            cin.ignore();
            saveSettings(userSet);
            cout << " ---------------------------------------------" << endl;
            cout << "  Настройки сохранены! Нажмите enter ...";
            cin.get();
            break;
        case Back:
            return;
        }
    }
}

void infoGame() {
    system("cls");
    cout << "\n ------------------------------" << endl;
    cout << "          О ПРОГРАММЕ          " << endl;
    cout << " ------------------------------" << endl;
    cout << "   версия: 4.0.0               " << endl;
    cout << "   выполнила: Киричун Анна     " << endl;
    cout << " ------------------------------\n\n\n" << endl;
    cout << " Нажмите Enter для продолжения...";
    cin.ignore();
    cin.get();
}

void exitGame() {
    cout << " ------------------------------" << endl;
    cout << "   Выход из программы ...      " << endl;
}

void saveSettings(const SETTING& userSet) {
    ofstream outFile("settings.txt");
    if (outFile.is_open()) {
        outFile << userSet.specialty << endl;
        outFile << userSet.studentName << endl;
        outFile << userSet.subjectCount << endl;
        outFile.close();
    }
}

void loadSettings(SETTING& userSet) {
    ifstream inFile("settings.txt");
    if (inFile.is_open()) {
        getline(inFile, userSet.specialty);
        getline(inFile, userSet.studentName);
        inFile >> userSet.subjectCount;
        inFile.close();
    }
    else {
        saveSettings(userSet);
    }
}

void loadSpecialties(set<string>& specialtiesSet) {
    ifstream inFile("specialties.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                specialtiesSet.insert(item);
            }
        }
        inFile.close();
    }
}

void loadNames(list<string>& namesList) {
    ifstream inFile("names.txt");
    string item;
    if (inFile.is_open()) {
        while (getline(inFile, item)) {
            if (!item.empty()) {
                namesList.push_back(item);
            }
        }
        inFile.close();
    }
}
