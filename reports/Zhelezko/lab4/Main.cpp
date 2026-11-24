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
const string PROGRAM_VERSION = "4.0";
const string STUDENT_NAME = "Железко Андрей";

const string SETTINGS_FILE = "settings.txt";
const string DIFFICULTIES_FILE = "difficulties.txt";
const string SAVE_FILE = "savegame.txt";
const string AUTOSAVE_FILE = "autosave.txt";

struct GameSettings {
    int mazeWidth = 10;
    int mazeHeight = 10;
    string difficulty = "Легкий";
};

class Room {
public:
    bool walls[4];
    bool visited;

    Room() : visited(false) {
        for (int i = 0; i < 4; ++i) walls[i] = true;
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
    void setPosition(int newX, int newY) { x = newX; y = newY; }
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
    Labyrinth(int w, int h, bool doGenerate = true)
        : width(w), height(h),
        rooms(h, vector<Room>(w)),
        exitX(w - 1), exitY(h - 1) {
        random_device rd;
        rng.seed(rd());
        if (doGenerate) {
            uniform_int_distribution<int> cornerDist(0, 2);
            int corner = cornerDist(rng);
            if (corner == 0) { exitX = w - 1; exitY = 0; }
            else if (corner == 1) { exitX = 0;     exitY = h - 1; }
            else { exitX = w - 1; exitY = h - 1; }
            generate();
        }
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getExitX() const { return exitX; }
    int getExitY() const { return exitY; }

    bool isExit(int x, int y) const { return x == exitX && y == exitY; }

    void markVisited(int x, int y) { if (inBounds(x, y)) rooms[y][x].visited = true; }

    int getWallsMask(int x, int y) const {
        if (!inBounds(x, y)) return 0xF;
        int mask = 0;
        for (int i = 0; i < 4; ++i)
            if (rooms[y][x].walls[i]) mask |= (1 << i);
        return mask;
    }
    bool isVisited(int x, int y) const { return inBounds(x, y) && rooms[y][x].visited; }
    void setWallsMask(int x, int y, int mask) {
        if (!inBounds(x, y)) return;
        for (int i = 0; i < 4; ++i)
            rooms[y][x].walls[i] = ((mask >> i) & 1) != 0;
    }
    void setVisited(int x, int y, bool v) { if (inBounds(x, y)) rooms[y][x].visited = v; }
    void setExit(int x, int y) { if (inBounds(x, y)) { exitX = x; exitY = y; } }

    bool canMove(int x, int y, int direction) const {
        if (!inBounds(x, y)) return false;
        if (direction < 0 || direction > 3) return false;
        return !rooms[y][x].walls[direction];
    }

    void generate() {
        vector<vector<bool>> carved(height, vector<bool>(width, false));
        vector<pair<int, int>> stack;

        const int dx[4] = { 0, 1, 0, -1 };
        const int dy[4] = { -1, 0, 1, 0 };

        carved[0][0] = true;
        stack.push_back({ 0, 0 });

        while (!stack.empty()) {
            int x = stack.back().first;
            int y = stack.back().second;

            if (x == exitX && y == exitY) {
                stack.pop_back();
                continue;
            }

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

            if (availableDirections.empty()) { stack.pop_back(); continue; }

            uniform_int_distribution<int> dist(
                0, static_cast<int>(availableDirections.size()) - 1);
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
        for (int x = 0; x < width; ++x) cout << "+---";
        cout << "+\n";

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == 0) cout << "|";
                else {
                    bool known = rooms[y][x - 1].visited || rooms[y][x].visited;
                    bool wall = rooms[y][x - 1].walls[1];
                    cout << (known && wall ? "|" : " ");
                }

                if (heroX >= 0 && heroY >= 0 && x == heroX && y == heroY)
                    cout << " H ";
                else if (!rooms[y][x].visited)
                    cout << " ? ";
                else if (x == exitX && y == exitY)
                    cout << " E ";
                else
                    cout << "   ";
            }
            cout << "|\n";

            for (int x = 0; x < width; ++x) {
                cout << "+";
                if (y == height - 1) cout << "---";
                else {
                    bool known = rooms[y][x].visited || rooms[y + 1][x].visited;
                    bool wall = rooms[y][x].walls[2];
                    cout << (known && wall ? "---" : "   ");
                }
            }
            cout << "+\n";
        }

        cout << "\nH - герой, E - выход (тупик в углу), ? - неизвестная область, "
            "+---+ и | - стены\n";
    }
};

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool fileExists(const string& filename) {
    ifstream in(filename);
    return in.good();
}

struct SaveData {
    int mazeWidth = 0;
    int mazeHeight = 0;
    int heroX = 0;
    int heroY = 0;
    int stepsLeft = 0;
    int exitX = 0;
    int exitY = 0;
    bool won = false;
    vector<vector<pair<int, bool>>> rooms;
};

bool saveGameToFile(const string& filename, const Labyrinth& lab,
    int heroX, int heroY, int stepsLeft, bool won) {
    ofstream out(filename);
    if (!out) { cerr << "Не удалось открыть файл для записи: " << filename << "\n"; return false; }

    out << "mazeWidth=" << lab.getWidth() << "\n";
    out << "mazeHeight=" << lab.getHeight() << "\n";
    out << "heroX=" << heroX << "\n";
    out << "heroY=" << heroY << "\n";
    out << "stepsLeft=" << stepsLeft << "\n";
    out << "exitX=" << lab.getExitX() << "\n";
    out << "exitY=" << lab.getExitY() << "\n";
    out << "won=" << (won ? 1 : 0) << "\n";
    out << "[rooms]\n";

    for (int y = 0; y < lab.getHeight(); ++y) {
        for (int x = 0; x < lab.getWidth(); ++x) {
            if (x > 0) out << " ";
            out << lab.getWallsMask(x, y) << ","
                << (lab.isVisited(x, y) ? 1 : 0);
        }
        out << "\n";
    }
    return true;
}

bool loadGameFromFile(const string& filename, SaveData& data) {
    ifstream in(filename);
    if (!in) return false;

    map<string, string> kv;
    vector<vector<pair<int, bool>>> rooms;
    bool roomsMode = false;
    int width = 0, height = 0;
    int rowIdx = 0;

    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;

        if (!roomsMode && line == "[rooms]") {
            try {
                width = stoi(kv.at("mazeWidth"));
                height = stoi(kv.at("mazeHeight"));
            }
            catch (...) { return false; }
            if (width  < MIN_MAZE_SIZE || width  > MAX_MAZE_SIZE ||
                height < MIN_MAZE_SIZE || height > MAX_MAZE_SIZE) return false;
            rooms.assign(height, vector<pair<int, bool>>(width, { 0xF, false }));
            roomsMode = true;
            continue;
        }

        if (!roomsMode) {
            size_t pos = line.find('=');
            if (pos == string::npos) continue;
            kv[trim(line.substr(0, pos))] = trim(line.substr(pos + 1));
            continue;
        }

        if (rowIdx >= height) continue;
        size_t pos = 0;
        int colIdx = 0;
        while (pos < line.size() && colIdx < width) {
            size_t space = line.find(' ', pos);
            string token;
            if (space == string::npos) { token = line.substr(pos); pos = line.size(); }
            else { token = line.substr(pos, space - pos); pos = space + 1; }

            size_t comma = token.find(',');
            if (comma == string::npos) return false;
            try {
                int mask = stoi(token.substr(0, comma));
                int vis = stoi(token.substr(comma + 1));
                rooms[rowIdx][colIdx] = { mask, vis != 0 };
            }
            catch (...) { return false; }
            ++colIdx;
        }
        if (colIdx != width) return false;
        ++rowIdx;
    }

    if (!roomsMode || rowIdx != height) return false;

    try {
        data.mazeWidth = stoi(kv.at("mazeWidth"));
        data.mazeHeight = stoi(kv.at("mazeHeight"));
        data.heroX = stoi(kv.at("heroX"));
        data.heroY = stoi(kv.at("heroY"));
        data.stepsLeft = stoi(kv.at("stepsLeft"));
        data.exitX = stoi(kv.at("exitX"));
        data.exitY = stoi(kv.at("exitY"));
        data.won = stoi(kv.at("won")) != 0;
    }
    catch (...) { return false; }

    if (data.heroX < 0 || data.heroX >= data.mazeWidth ||
        data.heroY < 0 || data.heroY >= data.mazeHeight) return false;
    if (data.exitX < 0 || data.exitX >= data.mazeWidth ||
        data.exitY < 0 || data.exitY >= data.mazeHeight) return false;

    data.rooms = std::move(rooms);
    return true;
}

void createDefaultDifficultiesFile() {
    ofstream out(DIFFICULTIES_FILE);
    if (!out) { cerr << "Не удалось создать файл уровней сложности\n"; return; }
    out << "Легкий=0\n";
    out << "Средний=101\n";
    out << "Сложный=401\n";
}

void ensureDifficultiesFile() {
    ifstream in(DIFFICULTIES_FILE);
    if (in.good()) return;
    createDefaultDifficultiesFile();
}

bool loadDifficultyLevels(map<string, int>& levels) {
    levels.clear();
    ifstream in(DIFFICULTIES_FILE);
    if (!in) return false;

    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        size_t pos = line.find('=');
        if (pos == string::npos) continue;
        string name = trim(line.substr(0, pos));
        string valueStr = trim(line.substr(pos + 1));
        if (name.empty() || valueStr.empty()) continue;
        try { levels[name] = stoi(valueStr); }
        catch (...) {}
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
        if (area >= pair.second && pair.second > bestMinArea) {
            bestMinArea = pair.second;
            result = pair.first;
        }
    }

    if (result.empty() && !levels.empty()) {
        int minValue = numeric_limits<int>::max();
        for (const auto& pair : levels)
            if (pair.second < minValue) { minValue = pair.second; result = pair.first; }
    }
    if (result.empty()) result = "Неизвестно";
    return result;
}

void updateDifficulty(GameSettings& settings, const map<string, int>& levels) {
    settings.difficulty = calculateDifficulty(settings.mazeWidth, settings.mazeHeight, levels);
}

bool saveSettings(const GameSettings& settings) {
    ofstream out(SETTINGS_FILE);
    if (!out) { cerr << "Не удалось сохранить настройки\n"; return false; }
    out << "mazeWidth=" << settings.mazeWidth << "\n";
    out << "mazeHeight=" << settings.mazeHeight << "\n";
    out << "difficulty=" << settings.difficulty << "\n";
    return true;
}

void loadSettings(GameSettings& settings, const map<string, int>& levels) {
    settings = GameSettings{};
    ifstream in(SETTINGS_FILE);
    if (!in) { updateDifficulty(settings, levels); saveSettings(settings); return; }

    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        size_t pos = line.find('=');
        if (pos == string::npos) continue;
        string key = trim(line.substr(0, pos));
        string value = trim(line.substr(pos + 1));
        if (key == "mazeWidth") {
            try { settings.mazeWidth = stoi(value); }
            catch (...) { settings.mazeWidth = 10; }
        }
        else if (key == "mazeHeight") {
            try { settings.mazeHeight = stoi(value); }
            catch (...) { settings.mazeHeight = 10; }
        }
        else if (key == "difficulty") {
            settings.difficulty = value;
        }
    }
    if (settings.mazeWidth  < MIN_MAZE_SIZE || settings.mazeWidth  > MAX_MAZE_SIZE) settings.mazeWidth = 10;
    if (settings.mazeHeight < MIN_MAZE_SIZE || settings.mazeHeight > MAX_MAZE_SIZE) settings.mazeHeight = 10;
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
    int width = readInt("Введите ширину x (" + to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): ");
    if (width < MIN_MAZE_SIZE || width > MAX_MAZE_SIZE) {
        cout << "Недопустимая ширина.\n"; waitEnter(); return;
    }
    int height = readInt("Введите высоту y (" + to_string(MIN_MAZE_SIZE) + "-" +
        to_string(MAX_MAZE_SIZE) + "): ");
    if (height < MIN_MAZE_SIZE || height > MAX_MAZE_SIZE) {
        cout << "Недопустимая высота.\n"; waitEnter(); return;
    }
    settings.mazeWidth = width;
    settings.mazeHeight = height;
    updateDifficulty(settings, levels);
    saveSettings(settings);
    cout << "\nРазмер сохранён: " << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
    cout << "Уровень сложности автоматически изменён на: " << settings.difficulty << "\n";
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
            cout << "\nСложность: " << settings.difficulty << "\n";
            cout << "Размер лабиринта: "
                << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
            waitEnter(); break;
        case SETTINGS_CHANGE_SIZE:
            changeMazeSize(settings, levels); break;
        case SETTINGS_BACK:
            return;
        default:
            cout << "Неверный пункт меню.\n"; waitEnter();
        }
    }
}

int calculateInitialSteps(const GameSettings& settings) {
    int area = settings.mazeWidth * settings.mazeHeight;
    float multiplier = 1.0f;
    if (settings.difficulty == "Легкий") multiplier = 1.5f;
    else if (settings.difficulty == "Средний") multiplier = 1.3f;
    else if (settings.difficulty == "Сложный") multiplier = 1.1f;

    int steps = static_cast<int>(area * multiplier);
    if (steps < 20) steps = 20;
    return steps;
}

void gameLoop(Labyrinth& labyrinth, Hero& hero, int stepsLeft, bool alreadyWon) {
    if (alreadyWon) {
        labyrinth.print(-1, -1);
        cout << "\nЭта игра была уже завершена победой.\n";
        cout << "Остаток шагов при победе: " << stepsLeft << "\n";
        waitEnter();
        return;
    }

    bool gameOver = false;
    bool win = false;

    while (!gameOver) {
        labyrinth.print(hero.getX(), hero.getY());

        cout << "\nОсталось шагов: " << stepsLeft << "\n";
        cout << "1. Вверх\n";
        cout << "2. Вправо\n";
        cout << "3. Вниз\n";
        cout << "4. Влево\n";
        cout << "5. Сохранить игру (ручное)\n";
        cout << "6. Выйти в главное меню\n";

        int choice = readInt("Выберите действие: ");

        if (choice == 5) {
            if (saveGameToFile(SAVE_FILE, labyrinth,
                hero.getX(), hero.getY(), stepsLeft, false))
                cout << "Игра сохранена в " << SAVE_FILE << "\n";
            else
                cout << "Ошибка сохранения!\n";
            waitEnter();
            continue;
        }
        if (choice == 6) {
            cout << "Вы вышли из игры. Прогресс автосохранён.\n";
            waitEnter();
            return;
        }
        if (choice < 1 || choice > 4) {
            cout << "Неверный пункт.\n"; waitEnter(); continue;
        }

        int direction = choice - 1;
        int oldX = hero.getX(), oldY = hero.getY();
        int newX = oldX, newY = oldY;
        if (direction == 0) newY--;
        else if (direction == 1) newX++;
        else if (direction == 2) newY++;
        else if (direction == 3) newX--;

        --stepsLeft;

        if (labyrinth.canMove(oldX, oldY, direction)) {
            hero.setPosition(newX, newY);
            labyrinth.markVisited(newX, newY);
            cout << "Вы переместились.\n";
            if (labyrinth.isExit(newX, newY)) { win = true; gameOver = true; }
        }
        else {
            cout << "Там стена. Ход потрачен.\n";
        }

        if (!gameOver && stepsLeft <= 0) { gameOver = true; win = false; }

        if (win) {
            saveGameToFile(AUTOSAVE_FILE, labyrinth,
                hero.getX(), hero.getY(), stepsLeft, true);
        }
        else if (!gameOver) {
            saveGameToFile(AUTOSAVE_FILE, labyrinth,
                hero.getX(), hero.getY(), stepsLeft, false);
        }

        if (!win && !gameOver) waitEnter();
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

void startGame(const GameSettings& settings) {
    cout << "\n    Начать игру    \n";
    cout << "Генерация нового лабиринта размером "
        << settings.mazeWidth << " x " << settings.mazeHeight
        << " (сложность: " << settings.difficulty << ")...\n";

    Labyrinth labyrinth(settings.mazeWidth, settings.mazeHeight);
    Hero hero(0, 0);
    labyrinth.markVisited(hero.getX(), hero.getY());
    int stepsLeft = calculateInitialSteps(settings);

    saveGameToFile(AUTOSAVE_FILE, labyrinth,
        hero.getX(), hero.getY(), stepsLeft, false);

    gameLoop(labyrinth, hero, stepsLeft, false);
}

void printSaveInfoLine(int num, const string& file, const SaveData& d) {
    cout << num << ". " << file << ": "
        << d.mazeWidth << "x" << d.mazeHeight
        << ", шагов: " << d.stepsLeft
        << ", герой (" << d.heroX << "," << d.heroY << ")"
        << ", выход (" << d.exitX << "," << d.exitY << ")";
    if (d.won) cout << "  [ПОБЕДА]";
    cout << "\n";
}

void loadGame(const GameSettings& settings) {
    cout << "\n    Загрузить игру    \n";

    bool hasManual = fileExists(SAVE_FILE);
    bool hasAuto = fileExists(AUTOSAVE_FILE);

    if (!hasManual && !hasAuto) {
        cout << "Сохранения не найдены.\n";
        waitEnter();
        return;
    }

    SaveData manualData, autoData;
    bool manualOk = hasManual && loadGameFromFile(SAVE_FILE, manualData);
    bool autoOk = hasAuto && loadGameFromFile(AUTOSAVE_FILE, autoData);

    if (hasManual) {
        if (manualOk) printSaveInfoLine(1, SAVE_FILE, manualData);
        else cout << "1. " << SAVE_FILE << ": файл повреждён\n";
    }
    if (hasAuto) {
        if (autoOk) printSaveInfoLine(2, AUTOSAVE_FILE, autoData);
        else cout << "2. " << AUTOSAVE_FILE << ": файл повреждён\n";
    }
    cout << "0. Отмена\n";

    int choice = readInt("Выберите сохранение: ");
    if (choice == 0) return;

    SaveData data;
    if (choice == 1) {
        if (!manualOk) { cout << "Недоступно.\n"; waitEnter(); return; }
        data = manualData;
    }
    else if (choice == 2) {
        if (!autoOk) { cout << "Недоступно.\n"; waitEnter(); return; }
        data = autoData;
    }
    else {
        cout << "Неверный пункт.\n"; waitEnter(); return;
    }

    Labyrinth lab(data.mazeWidth, data.mazeHeight, false);
    for (int y = 0; y < data.mazeHeight; ++y)
        for (int x = 0; x < data.mazeWidth; ++x) {
            lab.setWallsMask(x, y, data.rooms[y][x].first);
            lab.setVisited(x, y, data.rooms[y][x].second);
        }
    lab.setExit(data.exitX, data.exitY);

    Hero hero(data.heroX, data.heroY);
    cout << "Игра загружена.\n";

    gameLoop(lab, hero, data.stepsLeft, data.won);
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
        case MENU_START:    startGame(settings); break;
        case MENU_LOAD:     loadGame(settings); break;
        case MENU_SETTINGS: settingsMenu(settings, difficultyLevels); break;
        case MENU_ABOUT:    about(); break;
        case MENU_EXIT:     cout << "\nВыход из программы.\n"; break;
        default:
            cout << "Неверный пункт меню. Попробуйте снова.\n";
            waitEnter();
        }
    } while (choice != MENU_EXIT);

    return 0;
}