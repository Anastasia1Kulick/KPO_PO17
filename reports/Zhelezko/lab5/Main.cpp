#include <iostream>
#include <limits>
#include <string>
#include <clocale>
#include <map>
#include <fstream>
#include <vector>
#include <random>
#include <utility>
#include <algorithm>
#include <sstream>

using namespace std;

const int MENU_START = 1;
const int MENU_LOAD = 2;
const int MENU_SETTINGS = 3;
const int MENU_ABOUT = 4;
const int MENU_EXIT = 5;
const int MENU_LEADERBOARD = 6;

const int SETTINGS_SHOW = 1;
const int SETTINGS_CHANGE_SIZE = 2;
const int SETTINGS_BACK = 3;

const int GAME_UP = 1;
const int GAME_RIGHT = 2;
const int GAME_DOWN = 3;
const int GAME_LEFT = 4;
const int GAME_SAVE = 5;
const int GAME_EXIT_TO_MENU = 6;
const int GAME_ABILITY = 7;

const int HERO_SCOUT = 0;
const int HERO_BREAKER = 1;
const int HERO_RUNNER = 2;
const int HERO_CLASS_COUNT = 3;

const int MIN_MAZE_SIZE = 5;
const int MAX_MAZE_SIZE = 50;
const int SCOUT_RADIUS = 1;
const int RUNNER_DISTANCE = 2;
const int LEADERBOARD_LIMIT = 10;
const int INPUT_EOF = -9999;

const string GAME_TITLE = "Побег из лабиринта";
const string PROGRAM_VERSION = "5.0";
const string STUDENT_NAME = "Железко Андрей";
const string SETTINGS_FILE = "settings.txt";
const string DIFFICULTIES_FILE = "difficulties.txt";
const string SAVE_FILE = "savegame.txt";
const string AUTOSAVE_FILE = "autosave.txt";
const string LEADERBOARD_FILE = "leaderboard.txt";

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

struct GameSettings {
    int mazeWidth = 10;
    int mazeHeight = 10;
    string difficulty = "Легкий";
};

bool isValidHeroClass(int cls) { return cls >= 0 && cls < HERO_CLASS_COUNT; }

string heroClassName(int cls) {
    switch (cls) {
    case HERO_SCOUT:   return "Разведчик";
    case HERO_BREAKER: return "Разрушитель";
    case HERO_RUNNER:  return "Бегун";
    default:           return "Неизвестный";
    }
}

string heroAbilityName(int cls) {
    switch (cls) {
    case HERO_SCOUT:   return "Подсветка области 3x3 (бесплатно)";
    case HERO_BREAKER: return "Пробить стену и пройти (1 шаг)";
    case HERO_RUNNER:  return "Рывок на 2 клетки (1 шаг)";
    default:           return "Нет способности";
    }
}

int heroAbilityMaxUses(int cls) {
    switch (cls) {
    case HERO_SCOUT:   return 3;
    case HERO_BREAKER: return 2;
    case HERO_RUNNER:  return 3;
    default:           return 0;
    }
}

bool heroAbilityNeedsDirection(int cls) {
    return cls == HERO_BREAKER || cls == HERO_RUNNER;
}

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
    int classId;
    int abilityUses;
public:
    Hero(int startX = 0, int startY = 0, int heroClass = HERO_SCOUT)
        : x(startX), y(startY), classId(HERO_SCOUT), abilityUses(0) {
        setClass(heroClass);
    }
    int getX() const { return x; }
    int getY() const { return y; }
    void setPosition(int newX, int newY) { x = newX; y = newY; }

    int getClassId() const { return classId; }
    int getAbilityUses() const { return abilityUses; }
    bool canUseAbility() const { return abilityUses > 0; }

    void setClass(int cls) {
        classId = isValidHeroClass(cls) ? cls : HERO_SCOUT;
        abilityUses = heroAbilityMaxUses(classId);
    }
    void setAbilityUses(int uses) {
        int mx = heroAbilityMaxUses(classId);
        if (uses < 0) uses = 0;
        if (uses > mx) uses = mx;
        abilityUses = uses;
    }
    void spendAbility() { if (abilityUses > 0) --abilityUses; }
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

    int getWidth()  const { return width; }
    int getHeight() const { return height; }
    int getExitX()  const { return exitX; }
    int getExitY()  const { return exitY; }
    bool isExit(int x, int y) const { return x == exitX && y == exitY; }

    void markVisited(int x, int y) { if (inBounds(x, y)) rooms[y][x].visited = true; }
    bool isVisited(int x, int y) const { return inBounds(x, y) && rooms[y][x].visited; }
    void setVisited(int x, int y, bool v) { if (inBounds(x, y)) rooms[y][x].visited = v; }
    void setExit(int x, int y) { if (inBounds(x, y)) { exitX = x; exitY = y; } }

    int getWallsMask(int x, int y) const {
        if (!inBounds(x, y)) return 0xF;
        int mask = 0;
        for (int i = 0; i < 4; ++i)
            if (rooms[y][x].walls[i]) mask |= (1 << i);
        return mask;
    }
    void setWallsMask(int x, int y, int mask) {
        if (!inBounds(x, y)) return;
        for (int i = 0; i < 4; ++i)
            rooms[y][x].walls[i] = ((mask >> i) & 1) != 0;
    }

    bool canMove(int x, int y, int direction) const {
        if (!inBounds(x, y)) return false;
        if (direction < 0 || direction > 3) return false;
        return !rooms[y][x].walls[direction];
    }

    bool breakWall(int x, int y, int direction) {
        if (!inBounds(x, y) || direction < 0 || direction > 3) return false;
        const int dx[4] = { 0, 1, 0, -1 };
        const int dy[4] = { -1, 0, 1, 0 };
        int nx = x + dx[direction];
        int ny = y + dy[direction];
        if (!inBounds(nx, ny)) return false;
        rooms[y][x].walls[direction] = false;
        rooms[ny][nx].walls[(direction + 2) % 4] = false;
        return true;
    }

    int revealArea(int cx, int cy, int radius) {
        int count = 0;
        for (int y = cy - radius; y <= cy + radius; ++y)
            for (int x = cx - radius; x <= cx + radius; ++x)
                if (inBounds(x, y) && !rooms[y][x].visited) {
                    rooms[y][x].visited = true;
                    ++count;
                }
        return count;
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
            if (x == exitX && y == exitY) { stack.pop_back(); continue; }
            vector<int> availableDirections;
            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (nx >= 0 && nx < width && ny >= 0 && ny < height && !carved[ny][nx])
                    availableDirections.push_back(dir);
            }
            if (availableDirections.empty()) { stack.pop_back(); continue; }
            uniform_int_distribution<int> dist(0, static_cast<int>(availableDirections.size()) - 1);
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

struct SaveData {
    int mazeWidth = 0;
    int mazeHeight = 0;
    int heroX = 0;
    int heroY = 0;
    int stepsLeft = 0;
    int exitX = 0;
    int exitY = 0;
    bool won = false;
    int heroClass = HERO_SCOUT;
    int abilityUses = 0;
    vector<vector<pair<int, bool>>> rooms;
};

bool saveGameToFile(const string& filename, const Labyrinth& lab,
    int heroX, int heroY, int stepsLeft, bool won,
    int heroClass = HERO_SCOUT, int abilityUses = 0) {
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
    out << "heroClass=" << heroClass << "\n";
    out << "abilityUses=" << abilityUses << "\n";
    out << "[rooms]\n";
    for (int y = 0; y < lab.getHeight(); ++y) {
        for (int x = 0; x < lab.getWidth(); ++x) {
            if (x > 0) out << " ";
            out << lab.getWallsMask(x, y) << "," << (lab.isVisited(x, y) ? 1 : 0);
        }
        out << "\n";
    }
    return true;
}

bool saveGameToFile(const string& filename, const Labyrinth& lab, const Hero& hero,
    int stepsLeft, bool won) {
    return saveGameToFile(filename, lab, hero.getX(), hero.getY(), stepsLeft, won,
        hero.getClassId(), hero.getAbilityUses());
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

    data.heroClass = HERO_SCOUT;
    data.abilityUses = heroAbilityMaxUses(HERO_SCOUT);
    if (kv.count("heroClass")) {
        int cls = HERO_SCOUT;
        try { cls = stoi(kv.at("heroClass")); }
        catch (...) { cls = HERO_SCOUT; }
        data.heroClass = isValidHeroClass(cls) ? cls : HERO_SCOUT;
        data.abilityUses = heroAbilityMaxUses(data.heroClass);
    }
    if (kv.count("abilityUses")) {
        int uses = data.abilityUses;
        try { uses = stoi(kv.at("abilityUses")); }
        catch (...) {}
        int mx = heroAbilityMaxUses(data.heroClass);
        if (uses < 0) uses = 0;
        if (uses > mx) uses = mx;
        data.abilityUses = uses;
    }

    data.rooms = std::move(rooms);
    return true;
}

map<string, int> defaultDifficultyLevels() {
    map<string, int> levels;
    levels["Легкий"] = 0;
    levels["Средний"] = 101;
    levels["Сложный"] = 401;
    return levels;
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
        levels = defaultDifficultyLevels();
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

string difficultyForSize(int width, int height) {
    map<string, int> levels;
    if (!loadDifficultyLevels(levels) || levels.empty()) levels = defaultDifficultyLevels();
    return calculateDifficulty(width, height, levels);
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
        if (cin.eof()) {
            cout << "\nВвод больше недоступен.\n";
            return INPUT_EOF;
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
    cout << "\nИзменение размера лабиринта\n";
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
        cout << "\nНастройки\n";
        cout << "Текущий уровень сложности: " << settings.difficulty << "\n";
        cout << "Текущий размер лабиринта: "
            << settings.mazeWidth << " x " << settings.mazeHeight << "\n";
        cout << SETTINGS_SHOW << ". Показать параметры\n";
        cout << SETTINGS_CHANGE_SIZE << ". Изменить размер лабиринта\n";
        cout << SETTINGS_BACK << ". Назад в главное меню\n";
        int choice = readInt("Выберите пункт: ");
        if (choice == INPUT_EOF) return;
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

int calculateInitialSteps(int width, int height, const string& difficulty) {
    int area = width * height;
    float multiplier = 1.0f;
    if (difficulty == "Легкий")       multiplier = 1.5f;
    else if (difficulty == "Средний") multiplier = 1.3f;
    else if (difficulty == "Сложный") multiplier = 1.1f;
    int steps = static_cast<int>(area * multiplier);
    if (steps < 20) steps = 20;
    return steps;
}

int calculateInitialSteps(const GameSettings& settings) {
    return calculateInitialSteps(settings.mazeWidth, settings.mazeHeight, settings.difficulty);
}

struct LeaderboardEntry {
    string name = "Игрок";
    int heroClass = HERO_SCOUT;
    int mazeWidth = 0;
    int mazeHeight = 0;
    string difficulty = "Легкий";
    int stepsLeft = 0;
    int stepsUsed = 0;
    int score = 0;
};

int difficultyBonus(const string& difficulty) {
    if (difficulty == "Средний") return 2;
    if (difficulty == "Сложный") return 3;
    return 1;
}

int calculateScore(int mazeWidth, int mazeHeight, int stepsLeft, const string& difficulty) {
    if (stepsLeft < 0) stepsLeft = 0;
    int area = mazeWidth * mazeHeight;
    return (area + stepsLeft * 2) * difficultyBonus(difficulty);
}

string sanitizeField(const string& s) {
    string r = trim(s);
    for (char& c : r) if (c == '|' || c == '\n' || c == '\r') c = ' ';
    if (r.empty()) r = "Игрок";
    return r;
}

string leaderboardEntryToLine(const LeaderboardEntry& e) {
    ostringstream out;
    out << sanitizeField(e.name) << "|" << e.heroClass << "|"
        << e.mazeWidth << "|" << e.mazeHeight << "|" << e.difficulty << "|"
        << e.stepsLeft << "|" << e.stepsUsed << "|" << e.score;
    return out.str();
}

bool parseLeaderboardLine(const string& line, LeaderboardEntry& e) {
    vector<string> f;
    size_t pos = 0;
    while (true) {
        size_t p = line.find('|', pos);
        if (p == string::npos) { f.push_back(line.substr(pos)); break; }
        f.push_back(line.substr(pos, p - pos));
        pos = p + 1;
    }
    if (f.size() < 8) return false;
    try {
        e.name = trim(f[0]);
        e.heroClass = stoi(f[1]);
        e.mazeWidth = stoi(f[2]);
        e.mazeHeight = stoi(f[3]);
        e.difficulty = trim(f[4]);
        e.stepsLeft = stoi(f[5]);
        e.stepsUsed = stoi(f[6]);
        e.score = stoi(f[7]);
    }
    catch (...) { return false; }
    if (e.name.empty()) e.name = "Игрок";
    if (!isValidHeroClass(e.heroClass)) e.heroClass = HERO_SCOUT;
    if (e.difficulty.empty()) e.difficulty = "Неизвестно";
    if (e.mazeWidth  < MIN_MAZE_SIZE || e.mazeWidth  > MAX_MAZE_SIZE) return false;
    if (e.mazeHeight < MIN_MAZE_SIZE || e.mazeHeight > MAX_MAZE_SIZE) return false;
    if (e.stepsLeft < 0 || e.stepsUsed < 0) return false;
    return true;
}

void sortLeaderboard(vector<LeaderboardEntry>& entries) {
    stable_sort(entries.begin(), entries.end(),
        [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
            if (a.score != b.score) return a.score > b.score;
            if (a.stepsUsed != b.stepsUsed) return a.stepsUsed < b.stepsUsed;
            return a.name < b.name;
        });
}

void trimLeaderboard(vector<LeaderboardEntry>& entries, size_t limit = LEADERBOARD_LIMIT) {
    if (limit > 0 && entries.size() > limit) entries.resize(limit);
}

bool loadLeaderboard(const string& filename, vector<LeaderboardEntry>& entries) {
    entries.clear();
    ifstream in(filename);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        LeaderboardEntry e;
        if (parseLeaderboardLine(line, e)) entries.push_back(e);
    }
    sortLeaderboard(entries);
    return true;
}

bool saveLeaderboard(const string& filename, const vector<LeaderboardEntry>& entries) {
    ofstream out(filename);
    if (!out) { cerr << "Не удалось сохранить таблицу лидеров\n"; return false; }
    for (const auto& e : entries) out << leaderboardEntryToLine(e) << "\n";
    return true;
}

bool addLeaderboardEntry(const string& filename, const LeaderboardEntry& entry,
    size_t limit = LEADERBOARD_LIMIT) {
    vector<LeaderboardEntry> entries;
    loadLeaderboard(filename, entries);
    entries.push_back(entry);
    sortLeaderboard(entries);
    trimLeaderboard(entries, limit);
    return saveLeaderboard(filename, entries);
}

bool isUtf8(const string& s) {
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { ++i; continue; }
        size_t need = 0;
        if (c >= 0xC2 && c <= 0xDF) need = 1;
        else if (c >= 0xE0 && c <= 0xEF) need = 2;
        else if (c >= 0xF0 && c <= 0xF4) need = 3;
        else return false;
        if (i + need >= s.size()) return false;
        for (size_t k = 1; k <= need; ++k) {
            unsigned char cc = static_cast<unsigned char>(s[i + k]);
            if (cc < 0x80 || cc > 0xBF) return false;
        }
        i += need + 1;
    }
    return true;
}

size_t displayWidth(const string& s) {
    if (!isUtf8(s)) return s.size();
    size_t w = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80 || c >= 0xC0) ++w;
    }
    return w;
}

string fitToWidth(const string& s, size_t width) {
    if (width == 0) return "";
    if (displayWidth(s) <= width) return s;
    bool utf8 = isUtf8(s);
    string r;
    size_t w = 0, i = 0;
    while (i < s.size() && w + 1 < width) {
        size_t step = 1;
        if (utf8) {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if (c >= 0xC2 && c <= 0xDF) step = 2;
            else if (c >= 0xE0 && c <= 0xEF) step = 3;
            else if (c >= 0xF0 && c <= 0xF4) step = 4;
        }
        r += s.substr(i, step);
        ++w;
        i += step;
    }
    return r + "~";
}

string padRight(const string& s, size_t width) {
    string r = fitToWidth(s, width);
    size_t w = displayWidth(r);
    if (w < width) r.append(width - w, ' ');
    return r;
}

void printLeaderboard(const vector<LeaderboardEntry>& entries) {
    cout << "\nТаблица лидеров (лучшие " << LEADERBOARD_LIMIT << ")\n";
    if (entries.empty()) {
        cout << "Пока нет ни одного прохождения.\n";
        return;
    }

    const size_t W_NUM = 2, W_NAME = 12, W_CLASS = 11, W_SIZE = 7;
    const size_t W_DIFF = 9, W_LEFT = 8, W_USED = 9, W_SCORE = 6;
    const string GAP = "  ";
    size_t total = W_NUM + W_NAME + W_CLASS + W_SIZE + W_DIFF +
        W_LEFT + W_USED + W_SCORE + GAP.size() * 7;
    string sep(total, '-');

    cout << sep << "\n";
    cout << padRight("№", W_NUM) << GAP
        << padRight("Имя", W_NAME) << GAP
        << padRight("Класс", W_CLASS) << GAP
        << padRight("Размер", W_SIZE) << GAP
        << padRight("Сложность", W_DIFF) << GAP
        << padRight("Осталось", W_LEFT) << GAP
        << padRight("Потрачено", W_USED) << GAP
        << padRight("Очки", W_SCORE) << "\n";
    cout << sep << "\n";

    int num = 1;
    for (const auto& e : entries) {
        string size = to_string(e.mazeWidth) + "x" + to_string(e.mazeHeight);
        cout << padRight(to_string(num), W_NUM) << GAP
            << padRight(e.name, W_NAME) << GAP
            << padRight(heroClassName(e.heroClass), W_CLASS) << GAP
            << padRight(size, W_SIZE) << GAP
            << padRight(e.difficulty, W_DIFF) << GAP
            << padRight(to_string(e.stepsLeft), W_LEFT) << GAP
            << padRight(to_string(e.stepsUsed), W_USED) << GAP
            << padRight(to_string(e.score), W_SCORE) << "\n";
        ++num;
    }
    cout << sep << "\n";
    cout << "Осталось - неиспользованные шаги, Потрачено - сделанные шаги.\n";
}

void showLeaderboard(const string& filename = LEADERBOARD_FILE) {
    vector<LeaderboardEntry> entries;
    loadLeaderboard(filename, entries);
    printLeaderboard(entries);
    waitEnter();
}

LeaderboardEntry makeWinEntry(const string& name, const Labyrinth& lab, const Hero& hero,
    int stepsLeft, int stepsUsed, const string& difficulty) {
    LeaderboardEntry e;
    e.name = sanitizeField(name);
    e.heroClass = hero.getClassId();
    e.mazeWidth = lab.getWidth();
    e.mazeHeight = lab.getHeight();
    e.difficulty = difficulty.empty() ? difficultyForSize(e.mazeWidth, e.mazeHeight) : difficulty;
    e.stepsLeft = stepsLeft < 0 ? 0 : stepsLeft;
    e.stepsUsed = stepsUsed < 0 ? 0 : stepsUsed;
    e.score = calculateScore(e.mazeWidth, e.mazeHeight, e.stepsLeft, e.difficulty);
    return e;
}

struct GameContext {
    string difficulty = "Легкий";
    bool recordResult = true;
    string playerName;
};

void registerWin(const Labyrinth& lab, const Hero& hero, int stepsLeft, const GameContext& ctx) {
    string name = trim(ctx.playerName);
    if (name.empty()) {
        cout << "Введите имя для таблицы лидеров: ";
        getline(cin, name);
        name = trim(name);
    }
    if (name.empty()) name = "Игрок";

    string difficulty = ctx.difficulty.empty()
        ? difficultyForSize(lab.getWidth(), lab.getHeight()) : ctx.difficulty;
    int total = calculateInitialSteps(lab.getWidth(), lab.getHeight(), difficulty);
    int used = total - stepsLeft;
    if (used < 0) used = 0;

    LeaderboardEntry e = makeWinEntry(name, lab, hero, stepsLeft, used, difficulty);
    if (addLeaderboardEntry(LEADERBOARD_FILE, e))
        cout << "Результат добавлен в таблицу лидеров. Очки: " << e.score << "\n";
    else
        cout << "Не удалось сохранить результат в таблицу лидеров.\n";

    vector<LeaderboardEntry> entries;
    loadLeaderboard(LEADERBOARD_FILE, entries);
    printLeaderboard(entries);
}

bool useHeroAbility(Labyrinth& lab, Hero& hero, int& stepsLeft, int direction) {
    if (!hero.canUseAbility()) {
        cout << "Заряды способности закончились.\n";
        return false;
    }
    int cls = hero.getClassId();
    int x = hero.getX(), y = hero.getY();
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    if (cls == HERO_SCOUT) {
        int revealed = lab.revealArea(x, y, SCOUT_RADIUS);
        hero.spendAbility();
        cout << "Разведчик подсветил область: новых клеток - " << revealed
            << ". Шаг не потрачен.\n";
        return true;
    }

    if (direction < 0 || direction > 3) {
        cout << "Неверное направление способности.\n";
        return false;
    }

    if (cls == HERO_BREAKER) {
        if (!lab.breakWall(x, y, direction)) {
            cout << "Внешнюю стену пробить нельзя. Заряд сохранён.\n";
            return false;
        }
        int nx = x + dx[direction], ny = y + dy[direction];
        hero.setPosition(nx, ny);
        lab.markVisited(nx, ny);
        hero.spendAbility();
        --stepsLeft;
        cout << "Разрушитель пробил стену и прошёл вперёд. Потрачен 1 шаг.\n";
        return true;
    }

    if (cls == HERO_RUNNER) {
        int cx = x, cy = y, moved = 0;
        for (int i = 0; i < RUNNER_DISTANCE; ++i) {
            if (!lab.canMove(cx, cy, direction)) break;
            cx += dx[direction];
            cy += dy[direction];
            ++moved;
            lab.markVisited(cx, cy);
            if (lab.isExit(cx, cy)) break;
        }
        if (moved == 0) {
            cout << "Рывок невозможен: там стена. Заряд сохранён.\n";
            return false;
        }
        hero.setPosition(cx, cy);
        hero.spendAbility();
        --stepsLeft;
        cout << "Бегун совершил рывок на " << moved << " клетк(и) за 1 шаг.\n";
        return true;
    }

    cout << "У этого класса нет способности.\n";
    return false;
}

void gameLoop(Labyrinth& labyrinth, Hero& hero, int stepsLeft, bool alreadyWon,
    const GameContext& ctx = GameContext{}) {
    if (alreadyWon) {
        labyrinth.print(-1, -1);
        cout << "\nЭта игра была уже завершена победой.\n";
        cout << "Остаток шагов при победе: " << stepsLeft << "\n";
        waitEnter();
        return;
    }
    if (stepsLeft <= 0) {
        labyrinth.print(-1, -1);
        cout << "\nШаги уже закончились. Эта игра проиграна, начните новую.\n";
        waitEnter();
        return;
    }

    bool gameOver = false;
    bool win = false;
    while (!gameOver) {
        labyrinth.print(hero.getX(), hero.getY());
        cout << "\nОсталось шагов: " << stepsLeft << "\n";
        cout << "Класс героя: " << heroClassName(hero.getClassId())
            << " | " << heroAbilityName(hero.getClassId())
            << " | зарядов: " << hero.getAbilityUses() << "\n";
        cout << GAME_UP << ". Вверх\n";
        cout << GAME_RIGHT << ". Вправо\n";
        cout << GAME_DOWN << ". Вниз\n";
        cout << GAME_LEFT << ". Влево\n";
        cout << GAME_SAVE << ". Сохранить игру (ручное)\n";
        cout << GAME_EXIT_TO_MENU << ". Выйти в главное меню\n";
        cout << GAME_ABILITY << ". Использовать способность класса\n";
        int choice = readInt("Выберите действие: ");

        if (choice == INPUT_EOF) {
            cout << "Ввод недоступен. Выход в главное меню.\n";
            return;
        }
        if (choice == GAME_SAVE) {
            if (saveGameToFile(SAVE_FILE, labyrinth, hero, stepsLeft, false))
                cout << "Игра сохранена в " << SAVE_FILE << "\n";
            else
                cout << "Ошибка сохранения!\n";
            waitEnter();
            continue;
        }
        if (choice == GAME_EXIT_TO_MENU) {
            cout << "Вы вышли из игры. Прогресс автосохранён.\n";
            waitEnter();
            return;
        }

        if (choice == GAME_ABILITY) {
            int direction = -1;
            if (heroAbilityNeedsDirection(hero.getClassId())) {
                int d = readInt("Направление (1-вверх, 2-вправо, 3-вниз, 4-влево): ");
                if (d == INPUT_EOF) return;
                direction = d - 1;
                if (direction < 0 || direction > 3) {
                    cout << "Неверное направление.\n";
                    waitEnter();
                    continue;
                }
            }
            if (!useHeroAbility(labyrinth, hero, stepsLeft, direction)) {
                waitEnter();
                continue;
            }
        }
        else if (choice >= GAME_UP && choice <= GAME_LEFT) {
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
            }
            else {
                cout << "Там стена. Ход потрачен.\n";
            }
        }
        else {
            cout << "Неверный пункт.\n";
            waitEnter();
            continue;
        }

        if (labyrinth.isExit(hero.getX(), hero.getY())) { win = true; gameOver = true; }
        if (!gameOver && stepsLeft <= 0) { gameOver = true; win = false; }
        saveGameToFile(AUTOSAVE_FILE, labyrinth, hero, stepsLeft, win);
        if (!gameOver) waitEnter();
    }

    if (win) {
        labyrinth.print(-1, -1);
        cout << "\nПоздравляем! Вы выбрались из лабиринта!\n";
        cout << "Остаток шагов: " << stepsLeft << "\n";
        if (ctx.recordResult) registerWin(labyrinth, hero, stepsLeft, ctx);
    }
    else {
        cout << "\nШаги закончились. Вы не успели выбраться из лабиринта.\n";
    }
    waitEnter();
}

int chooseHeroClass() {
    cout << "\nВыберите класс героя\n";
    for (int i = 0; i < HERO_CLASS_COUNT; ++i) {
        cout << (i + 1) << ". " << heroClassName(i) << " - " << heroAbilityName(i)
            << " (зарядов: " << heroAbilityMaxUses(i) << ")\n";
    }
    while (true) {
        int choice = readInt("Ваш выбор: ");
        if (choice == INPUT_EOF) return HERO_SCOUT;
        if (choice >= 1 && choice <= HERO_CLASS_COUNT) return choice - 1;
        cout << "Неверный пункт меню.\n";
    }
}

void startGame(const GameSettings& settings, int heroClass = HERO_SCOUT,
    const string& playerName = "") {
    cout << "\nНачать игру\n";
    cout << "Генерация нового лабиринта размером "
        << settings.mazeWidth << " x " << settings.mazeHeight
        << " (сложность: " << settings.difficulty << ")...\n";
    Labyrinth labyrinth(settings.mazeWidth, settings.mazeHeight);
    Hero hero(0, 0, heroClass);
    labyrinth.markVisited(hero.getX(), hero.getY());
    cout << "Класс героя: " << heroClassName(hero.getClassId())
        << " - " << heroAbilityName(hero.getClassId()) << "\n";
    int stepsLeft = calculateInitialSteps(settings);
    saveGameToFile(AUTOSAVE_FILE, labyrinth, hero, stepsLeft, false);

    GameContext ctx;
    ctx.difficulty = settings.difficulty;
    ctx.playerName = playerName;
    gameLoop(labyrinth, hero, stepsLeft, false, ctx);
}

void printSaveInfoLine(int num, const string& file, const SaveData& d) {
    cout << num << ". " << file << ": "
        << d.mazeWidth << "x" << d.mazeHeight
        << ", шагов: " << d.stepsLeft
        << ", герой (" << d.heroX << "," << d.heroY << ")"
        << ", класс: " << heroClassName(d.heroClass)
        << " (зарядов: " << d.abilityUses << ")"
        << ", выход (" << d.exitX << "," << d.exitY << ")";
    if (d.won) cout << "  [ПОБЕДА]";
    cout << "\n";
}

void loadGame(const GameSettings& settings) {
    (void)settings;
    cout << "\nЗагрузить игру\n";
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
    if (choice == 0 || choice == INPUT_EOF) return;

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
    Hero hero(data.heroX, data.heroY, data.heroClass);
    hero.setAbilityUses(data.abilityUses);

    GameContext ctx;
    ctx.difficulty = difficultyForSize(data.mazeWidth, data.mazeHeight);
    cout << "Игра загружена.\n";
    gameLoop(lab, hero, data.stepsLeft, data.won, ctx);
}

void about() {
    cout << "\nО программе\n";
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
    if (difficultyLevels.empty()) difficultyLevels = defaultDifficultyLevels();
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
        cout << MENU_LEADERBOARD << ". Таблица лидеров\n";
        choice = readInt("Введите номер пункта меню: ");
        switch (choice) {
        case MENU_START: {
            int heroClass = chooseHeroClass();
            startGame(settings, heroClass);
            break;
        }
        case MENU_LOAD:        loadGame(settings); break;
        case MENU_SETTINGS:    settingsMenu(settings, difficultyLevels); break;
        case MENU_ABOUT:       about(); break;
        case MENU_EXIT:        cout << "\nВыход из программы.\n"; break;
        case MENU_LEADERBOARD: showLeaderboard(); break;
        default:
            if (choice == INPUT_EOF) {
                cout << "\nВвод недоступен. Завершение программы.\n";
                return 0;
            }
            cout << "Неверный пункт меню. Попробуйте снова.\n";
            waitEnter();
        }
    } while (choice != MENU_EXIT);
    return 0;
}