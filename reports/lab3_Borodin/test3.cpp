#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>

static std::string exeName = "./lab3";

static std::string runWithInput(const std::string& input) {
    std::string tmpIn = "test_input.tmp";
    std::string tmpOut = "test_output.tmp";
    std::ofstream fin(tmpIn);
    fin << input;
    fin.close();
    std::string cmd = exeName + " < " + tmpIn + " > " + tmpOut + " 2>&1";
    int ret = std::system(cmd.c_str());
    (void)ret;
    std::ifstream fout(tmpOut);
    std::stringstream ss;
    ss << fout.rdbuf();
    fout.close();
    std::remove(tmpIn.c_str());
    std::remove(tmpOut.c_str());
    return ss.str();
}

static bool contains(const std::string& hay, const std::string& needle) {
    return hay.find(needle) != std::string::npos;
}

static int passed = 0;
static int failed = 0;

static void check(bool cond, const std::string& name) {
    if (cond) {
        std::cout << "[PASS] " << name << "\n";
        passed++;
    } else {
        std::cout << "[FAIL] " << name << "\n";
        failed++;
    }
}

static void removeOptions() {
    std::remove("options.txt");
}

int main(int argc, char** argv) {
    if (argc > 1) exeName = argv[1];

    {
        removeOptions();
        std::string out = runWithInput("5\n");
        check(contains(out, "1. Начать игру") &&
              contains(out, "2. Загрузить игру") &&
              contains(out, "3. Настройки") &&
              contains(out, "4. О программе") &&
              contains(out, "5. Выход"),
              "test1_menu_display");
        std::ifstream f("options.txt");
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        f.close();
        check(contains(content, "10") && contains(content, "50") &&
              contains(content, "Легкая"),
              "test1_options_default_created");
    }

    {
        removeOptions();
        std::string out = runWithInput("4\n\n5\n");
        check(contains(out, "Версия игры 0.5") &&
              contains(out, "Бородин Кирилл"),
              "test2_about");
    }

    {
        removeOptions();
        std::string out = runWithInput("3\n3\n5\n");
        check(contains(out, "Здоровье: 10") &&
              contains(out, "Маленькое") &&
              contains(out, "Легкая"),
              "test3_settings_defaults");
    }

    {
        removeOptions();
        std::string out = runWithInput("1\n\n\n\n5\n");
        check(contains(out, "Начало игры") &&
              contains(out, "Вы спускаетесь в подземелье") &&
              contains(out, "Этаж 1") &&
              contains(out, "HP") &&
              contains(out, "Атаковать"),
              "test4_game_start_combat");
    }

    std::cout << "\nPassed: " << passed << ", Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}