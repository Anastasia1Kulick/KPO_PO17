// test.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

static std::string exeName = "./1lab";

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
    }
    else {
        std::cout << "[FAIL] " << name << "\n";
        failed++;
    }
}

int main(int argc, char** argv) {
    if (argc > 1) exeName = argv[1];

    {
        std::string out = runWithInput("5\n");
        check(contains(out, "1. Начать игру") &&
            contains(out, "2. Загрузить игру") &&
            contains(out, "3. Настройки") &&
            contains(out, "4. О программе") &&
            contains(out, "5. Выход"),
            "test1_menu_display");
    }

    {
        std::string out = runWithInput("4\n\n5\n");
        check(contains(out, "Версия игры 0.1") &&
            contains(out, "Бородин Кирилл"),
            "test2_about");
    }

    {
        std::string out = runWithInput("3\n2\n5\n");
        check(contains(out, "Здоровье: 10") &&
            contains(out, "Маленькое") &&
            contains(out, "Легко"),
            "test3_settings_defaults");
    }

    {
        std::string out = runWithInput("3\n1\n7\n2\n3\n2\n5\n");
        check(contains(out, "Здоровье: 7") &&
            contains(out, "Среднее") &&
            contains(out, "Тяжело"),
            "test4_settings_change");
    }

    std::cout << "\nPassed: " << passed << ", Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}