#include <iostream>
#include <limits>
#include <string>
#include <clocale>
#include <map>
#include <fstream>
#include <vector>
#include <random>
#include <utility>

using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;

const int SETTINGS_SHOW = 1;
const int SETTINGS_CHANGE_SIZE = 2;
const int SETTINGS_BACK = 3;

const int MIN_MAZE_SIZE = 5;
const int MAX_MAZE_SIZE = 50;

const string GAME_TITLE = "Побег из лабиринта";
const string PROGRAM_VERSION = "3.0";
const string STUDENT_NAME = "Железко Андрей";

const string SETTINGS_FILE = "settings.txt";
const string DIFFICULTIES_FILE = "difficulties.txt";

struct GameSettings {
    int mazeWidth = 10;
    int mazeHeight = 10;
    string difficulty = "Легкий";
};

class Room {
public:
    // 0 - вверх, 1 - вправо, 2 - вниз, 3 - влево
    // true  - стена есть (прохода нет)
    // false - стены нет (есть проход)
    bool walls[4];
    bool visited;

    Room() : visited(false) {
        for (int i = 0; i < 4; ++i) {
            walls[i] = true;
        }
    }
};

class Hero {
private:
    int x;
    int y;

public:
    Hero(int startX = 0, int startY = 0) : x(startX), y(startY) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }
};

class Labyrinth {
private:
    int width;
    int height;
    vector<vector<Room>> rooms;
    mt19937 rng;
    int exitX;
    int exitY;

    bool inBounds(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

public:
    Labyrinth(int w, int h)
        : width(w), height(h),
        rooms(h, vector<Room>(w)),
        exitX(w - 1), exitY(h - 1) {
        random_device rd;
        rng.seed(rd());
        generate();
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getExitX() const { return exitX; }
    int getExitY() const { return exitY; }

    bool isExit(int x, int y) const {
        return x == exitX && y == exitY;
    }

    void markVisited(int x, int y) {
        if (inBounds(x, y)) {
            rooms[y][x].visited = true;
        }
    }

    bool canMove(int x, int y, int direction) const {
        if (!inBounds(x, y)) {
            return false;
        }

        if (direction < 0 || direction > 3) {
            return false;
        }

        return !rooms[y][x].walls[direction];
    }

    void generate() {
        vector<vector<bool>> carved(height, vector<bool>(width, false));
        vector<pair<int, int>> stack;

        const int dx[4] = { 0, 1, 0, -1 };
        const int dy[4] = { -1, 0, 1, 0 };

        int startX = 0;
        int startY = 0;

        carved[startY][startX] = true;
        stack.push_back({ startX, startY });

        while (!stack.empty()) {
            int x = stack.back().first;
            int y = stack.back().second;

            vector<int> availableDirections;

            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx >= 0 && nx < width &&
                    ny >= 0 && ny < height &&
                    !carved[ny][nx]) {
                    availableDirections.push_back(dir);
                }
            }

            if (availableDirections.empty()) {
                stack.pop_back();
                continue;
            }

            uniform_int_distribution<int> dist(
                0,
                static_cast<int>(availableDirections.size()) - 1
            );

            int dir = availableDirections[dist(rng)];
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            rooms[y][x].walls[dir] = false;
            rooms[ny][nx].walls[(dir + 2) % 4] = false;

            carved[ny][nx] = true;
            stack.push_back({ nx, ny });
        }
    }

    void print(int heroX, int heroY) const {
        for (int x = 0; x < width; ++x) {
            cout << "+---";
        }
        cout << "+\n";

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == 0) {
                    cout << "|";  
                }
                else {
                    bool known = rooms[y][x - 1].visited || rooms[y][x].visited;
                    bool wall = rooms[y][x - 1].walls[1];
                    cout << (known && wall ? "|" : " ");
                }

                if (heroX >= 0 && heroY >= 0 && x == heroX && y == heroY) {
                    cout << " H ";
                }
                else if (!rooms[y][x].visited) {
                    cout << " ? ";
                }
                else if (x == exitX && y == exitY) {
                    cout << " E ";
                }
                else {
                    cout << "   ";
                }
            }
            cout << "|\n";

            for (int x = 0; x < width; ++x) {
                cout << "+";
                if (y == height - 1) {
                    cout << "---";
                }
                else {
                    bool known = rooms[y][x].visited || rooms[y + 1][x].visited;
                    bool wall = rooms[y][x].walls[2];
                    cout << (known && wall ? "---" : "   ");
                }
            }
            cout << "+\n";
        }

        cout << "\nH - герой, E - выход, ? - неизвестная область, "
            "+---+ и | - стены\n";
    }
};

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }

    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

void createDefaultDifficultiesFile() {
    ofstream out(DIFFICULTIES_FILE);

    if (!out) {
        cerr << "Не удалось создать файл уровней сложности: " << DIFFICULTIES_FILE << "\n";
        return;
    }

    out << "Легкий=0\n";
    out << "Средний=101\n";
    out << "Сложный=401\n";
}

void ensureDifficultiesFile() {
    ifstream in(DIFFICULTIES_FILE);

    if (in.good()) {
        return;
    }

    createDefaultDifficultiesFile();
}

bool loadDifficultyLevels(map<string, int>& levels) {
    levels.clear();

    ifstream in(DIFFICULTIES_FILE);

    if (!in) {
        return false;
    }

    string line;

    while (getline(in, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t pos = line.find('=');

        if (pos == string::npos) {
            continue;
        }

        string name = trim(line.substr(0, pos));
        string valueStr = trim(line.substr(pos + 1));

        if (name.empty() || valueStr.empty()) {
            continue;
        }

        try {
            int minArea = stoi(valueStr);
            levels[name] = minArea;
        }
        catch (...) {
            continue;
        }
    }

    if (levels.empty()) {
        levels["Легкий"] = 0;
        levels["Средний"] = 101;
        levels["Сложный"] = 401;
        createDefaultDifficultiesFile();
        return false;
    }

    return true;
}

string calculateDifficulty(int width, int height, const map<string, int>& levels) {
    int area = width * height;
    string result;
    int bestMinArea = -1;

    for (const auto& pair : levels) {
        const string& name = pair.first;
        int minArea = pair.second;

        if (area >= minArea && minArea > bestMinArea) {
            bestMinArea = minArea;
            result = name;
        }
    }

    if (result.empty()) {
        if (!levels.empty()) {
            int minValue = numeric_limits<int>::max();

            for (const auto& pair : levels) {
                if (pair.second < minValue) {
                    minValue = pair.second;
                    result = pair.first;
                }
            }
        }
        else {
            result = "Неизвестно";
        }
    }

    return result;
}

void updateDifficulty(GameSettings& settings, const map<string, int>& levels) {
    settings.difficulty = calculateDifficulty(
        settings.mazeWidth,
        settings.mazeHeight,
        levels
    );
}

bool saveSettings(const GameSettings& settings) {
    ofstream out(SETTINGS_FILE);

    if (!out) {
        cerr << "Не удалось сохранить настройки в файл: " << SETTINGS_FILE << "\n";
        return false;
    }

    out << "mazeWidth=" << settings.mazeWidth << "\n";
    out << "mazeHeight=" << settings.mazeHeight << "\n";
    out << "difficulty=" << settings.difficulty << "\n";

    return true;
}

void loadSettings(GameSettings& settings, const map<string, int>& levels) {
    settings = GameSettings{};

    ifstream in(SETTINGS_FILE);

    if (!in) {
        updateDifficulty(settings, levels);
        saveSettings(settings);
        return;
    }

    string line;

    while (getline(in, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        size_t pos = line.find('=');

        if (pos == string::npos) {
            continue;
        }

        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));

        if (key == "mazeWidth") {
            try {
                settings.mazeWidth = stoi(value);
            }
            catch (...) {
                settings.mazeWidth = 10;
            }
        }
        else if (key == "mazeHeight") {
            try {
                settings.mazeHeight = stoi(value);
            }
            catch (...) {
                settings.mazeHeight = 10;
            }
        }
        else if (key == "difficulty") {
            settings.difficulty = value;
        }
    }

    if (settings.mazeWidth < MIN_MAZE_SIZE || settings.mazeWidth > MAX_MAZE_SIZE) {
        settings.mazeWidth = 10;
    }

    if (settings.mazeHeight < MIN_MAZE_SIZE || settings.mazeHeight > MAX_MAZE_SIZE) {
        settings.mazeHeight = 10;
    }

    updateDifficulty(settings, levels);
    saveSettings(settings);
}

int readInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        cout << "Ошибка: введите целое число.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void waitEnter() {
    cout << "\nНажмите Enter, чтобы продолжить...";
    cin.get();
}

void changeMazeSize(GameSettings& settings, const map<string, int>& levels) {
    cout << "\n    Изменение размера лабиринта    \n";

    int width = readInt(
        "Введите ширину x (" +
        to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): "
    );

    if (width < MIN_MAZE_SIZE || width > MAX_MAZE_SIZE) {
        cout << "Недопустимая ширина.\n";
        waitEnter();
        return;
    }

    int height = readInt(
        "Введите высоту y (" +
        to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): "
    );

    if (height < MIN_MAZE_SIZE || height > MAX_MAZE_SIZE) {
        cout << "Недопустимая высота.\n";
        waitEnter();
        return;
    }

    settings.mazeWidth = width;
    settings.mazeHeight = height;
    updateDifficulty(settings, levels);
    saveSettings(settings);

    cout << "\nРазмер лабиринта сохранён: "
        << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
    cout << "Уровень сложности автоматически изменён на: "
        << settings.difficulty << "\n";

    waitEnter();
}

void settingsMenu(GameSettings& settings, const map<string, int>& levels) {
    while (true) {
        updateDifficulty(settings, levels);

        cout << "\n    Настройки    \n";
        cout << "Текущий уровень сложности: " << settings.difficulty << "\n";
        cout << "Текущий размер лабиринта: "
            << settings.mazeWidth << " x " << settings.mazeHeight << "\n";

        cout << SETTINGS_SHOW << ". Показать параметры\n";
        cout << SETTINGS_CHANGE_SIZE << ". Изменить размер лабиринта\n";
        cout << SETTINGS_BACK << ". Назад в главное меню\n";

        int choice = readInt("Выберите пункт: ");

        switch (choice) {
        case SETTINGS_SHOW:
            cout << "\nТекущие настройки:\n";
            cout << "Сложность: " << settings.difficulty << "\n";
            cout << "Размер лабиринта: "
                << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
            waitEnter();
            break;

        case SETTINGS_CHANGE_SIZE:
            changeMazeSize(settings, levels);
            break;

        case SETTINGS_BACK:
            return;

        default:
            cout << "Неверный пункт меню.\n";
            waitEnter();
        }
    }
}

int calculateInitialSteps(const GameSettings& settings) {
    int area = settings.mazeWidth * settings.mazeHeight;
    float multiplier = 1;

    if (settings.difficulty == "Легкий") {
        multiplier = 1.5;
    }
    else if (settings.difficulty == "Средний") {
        multiplier = 1.3;
    }
    else if (settings.difficulty == "Сложный") {
        multiplier = 1.1;
    }

    int steps = area * multiplier;

    if (steps < 20) {
        steps = 20;
    }

    return steps;
}

void startGame(const GameSettings& settings) {
    cout << "\n    Начать игру    \n";
    cout << "Генерация нового лабиринта размером "
        << settings.mazeWidth << " x " << settings.mazeHeight
        << " (сложность: " << settings.difficulty << ")...\n";

    Labyrinth labyrinth(settings.mazeWidth, settings.mazeHeight);
    Hero hero(0, 0);

    labyrinth.markVisited(hero.getX(), hero.getY());

    int stepsLeft = calculateInitialSteps(settings);
    bool gameOver = false;
    bool win = false;

    while (!gameOver) {
        labyrinth.print(hero.getX(), hero.getY());

        cout << "\nОсталось шагов: " << stepsLeft << "\n";
        cout << "1. Вверх\n";
        cout << "2. Вправо\n";
        cout << "3. Вниз\n";
        cout << "4. Влево\n";
        cout << "5. Выйти в главное меню\n";

        int choice = readInt("Выберите направление: ");

        if (choice == 5) {
            cout << "Вы вышли из игры.\n";
            waitEnter();
            return;
        }

        if (choice < 1 || choice > 4) {
            cout << "Неверный пункт. Попробуйте снова.\n";
            waitEnter();
            continue;
        }

        int direction = choice - 1;
        int oldX = hero.getX();
        int oldY = hero.getY();

        int newX = oldX;
        int newY = oldY;

        if (direction == 0) {
            newY--;
        }
        else if (direction == 1) {
            newX++;
        }
        else if (direction == 2) {
            newY++;
        }
        else if (direction == 3) {
            newX--;
        }

        --stepsLeft;

        if (labyrinth.canMove(oldX, oldY, direction)) {
            hero.setPosition(newX, newY);
            labyrinth.markVisited(newX, newY);

            cout << "Вы переместились.\n";

            if (labyrinth.isExit(newX, newY)) {
                win = true;
                gameOver = true;
            }
        }
        else {
            cout << "Там стена. Ход потрачен.\n";
        }

        if (!gameOver && stepsLeft <= 0) {
            gameOver = true;
            win = false;
        }

        if (!win) {
            waitEnter();
        }
    }

    if (win) {
        labyrinth.print(-1, -1);

        cout << "\nПоздравляем! Вы выбрались из лабиринта!\n";
        cout << "Остаток шагов: " << stepsLeft << "\n";
    }
    else {
        cout << "\nШаги закончились. Вы не успели выбраться из лабиринта.\n";
    }

    waitEnter();
}

void loadGame() {
    cout << "\n    Загрузить игру    \n";
    cout << "Данная функция на данный момент недоступна и появится в ближайших обновлениях.\n";
    waitEnter();
}

void about() {
    cout << "\n    О программе    \n";
    cout << "Название: " << GAME_TITLE << "\n";
    cout << "Версия: " << PROGRAM_VERSION << "\n";
    cout << "Выполнил: " << STUDENT_NAME << "\n";
    waitEnter();
}

int main() {
    setlocale(LC_ALL, "Russian");

    ensureDifficultiesFile();

    map<string, int> difficultyLevels;
    loadDifficultyLevels(difficultyLevels);

    GameSettings settings;
    loadSettings(settings, difficultyLevels);

    int choice = 0;

    do {
        cout << "\n" << GAME_TITLE << "\n";
        cout << MENU_START << ". Начать игру\n";
        cout << MENU_LOAD << ". Загрузить игру\n";
        cout << MENU_SETTINGS << ". Настройки\n";
        cout << MENU_ABOUT << ". О программе\n";
        cout << MENU_EXIT << ". Выход\n";

        choice = readInt("Введите номер пункта меню: ");

        switch (choice) {
        case MENU_START:
            startGame(settings);
            break;

        case MENU_LOAD:
            loadGame();
            break;

        case MENU_SETTINGS:
            settingsMenu(settings, difficultyLevels);
            break;

        case MENU_ABOUT:
            about();
            break;

        case MENU_EXIT:
            cout << "\nВыход из программы.\n";
            break;

        default:
            cout << "Неверный пункт меню. Попробуйте снова.\n";
            waitEnter();
        }

    } while (choice != MENU_EXIT);

    return 0;
}