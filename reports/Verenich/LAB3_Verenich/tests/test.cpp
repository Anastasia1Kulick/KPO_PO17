#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <clocale>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace std;

const string GREEN = "\033[0;32m";
const string RED = "\033[0;31m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[0;34m";
const string NC = "\033[0m";

// ==================== COUNTERS ====================
int passed = 0, failed = 0, total = 0;

const string BINARY = "./farm";
const string SETTINGS_FILE = "settings.txt";

// ==================== LOCALE SETUP ====================
void setupLocale()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");
#else
    setlocale(LC_ALL, "ru_RU.UTF-8");
#endif
}

// ==================== UTILITIES ====================

// Run the main program with given input (simulating user actions)
string runProgram(const string& input)
{
    string inputFile = "test_input.tmp";
    string outputFile = "test_output.tmp";

    ofstream fin(inputFile);
    fin << input;
    fin.close();

    string cmd = BINARY + " < " + inputFile + " > " + outputFile + " 2>&1";
    system(cmd.c_str());

    ifstream fout(outputFile);
    stringstream ss;
    ss << fout.rdbuf();
    fout.close();

    remove(inputFile.c_str());
    remove(outputFile.c_str());
    return ss.str();
}

// Check if text contains a substring
bool contains(const string& text, const string& sub)
{
    return text.find(sub) != string::npos;
}

// Print test result
void printResult(const string& name, bool ok)
{
    total++;
    if (ok)
    {
        cout << GREEN << "[PASS]" << NC << " " << name << "\n";
        passed++;
    }
    else
    {
        cout << RED << "[FAIL]" << NC << " " << name << "\n";
        failed++;
    }
}

// Remove settings file before test
void removeSettings()
{
    remove(SETTINGS_FILE.c_str());
}

// Check if game executable exists
bool checkBinaryExists()
{
    ifstream c(BINARY);
    bool ok = c.good();
    c.close();
    return ok;
}

// ==================== TESTS (LAB #3) ====================

// Test 1: Start game — day, money, actions are displayed
void testStartGameShowsState()
{
    removeSettings();
    runProgram("5\n"); // create default settings file
    string output = runProgram("1\n\n\n2\n5\n"); // enter game, end day, exit
    bool ok = contains(output, "Day:") &&
        contains(output, "Money:") &&
        contains(output, "Actions:");
    printResult("Test 1: Start game — player state", ok);
}

// Test 2: Starting animals from settings are present (Cows, Chickens)
void testStartGameHasAnimals()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    bool ok = contains(output, "Cows") &&
        contains(output, "Chickens");
    printResult("Test 2: Starting animals from settings", ok);
}

// Test 3: Actions per day depends on difficulty (Easy = 5)
void testActionsEasy()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Set Easy
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Test 3: Easy — 5 actions per day",
        contains(output, "Actions: 5"));
}

// Test 4: Actions per day depends on difficulty (Hard = 3)
void testActionsHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n"); // Set Hard
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Test 4: Hard — 3 actions per day",
        contains(output, "Actions: 3"));
}

// Test 5: Money depends on difficulty (Easy = 2000)
void testMoneyEasy()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Test 5: Easy — 2000 money",
        contains(output, "Money: 2000"));
}

// Test 6: Money depends on difficulty (Hard = 500)
void testMoneyHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    printResult("Test 6: Hard — 500 money",
        contains(output, "Money: 500"));
}

// Test 7: Day menu is displayed (4 actions)
void testDayMenu()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n\n\n2\n5\n");
    bool ok = contains(output, "Feed") &&
        contains(output, "Care") &&
        contains(output, "Buy food") &&
        contains(output, "Buy an animal");
    printResult("Test 7: Day menu with all actions", ok);
}

// Test 8: Feeding an animal (food available in stock)
void testFeedAnimal()
{
    removeSettings();
    runProgram("5\n");
    // Game -> feed -> 1st animal -> end day -> exit
    string output = runProgram("1\n1\n1\n2\n2\n5\n");
    printResult("Test 8: Feeding an animal",
        contains(output, "fed"));
}

// Test 9: Caring for an animal
void testCareAnimal()
{
    removeSettings();
    runProgram("5\n");
    // Game -> care -> 1st animal -> end day -> exit
    string output = runProgram("1\n2\n1\n2\n2\n5\n");
    printResult("Test 9: Caring for an animal",
        contains(output, "cared for"));
}

// Test 10: Buying food (Easy — enough money)
void testBuyFood()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy
    // Game -> buy food -> 1 (Hay) -> 10 pcs -> end day -> exit
    string output = runProgram("1\n3\n1\n10\n2\n2\n5\n");
    printResult("Test 10: Buying food",
        contains(output, "Bought 10"));
}

// Test 11: Buying an animal
void testBuyAnimal()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy
    // Game -> buy animal -> 1 (Cows) -> end day -> exit
    string output = runProgram("1\n4\n1\n2\n2\n5\n");
    printResult("Test 11: Buying an animal",
        contains(output, "Bought"));
}

// Test 12: Ending day — increases day number
void testEndDay()
{
    removeSettings();
    runProgram("5\n");
    // Game -> end day -> exit
    string output = runProgram("1\n2\n2\n5\n");
    printResult("Test 12: Ending day increases day",
        contains(output, "Day: 2"));
}

// Test 13: Income from animals is credited (money grows)
void testIncome()
{
    removeSettings();
    runProgram("3\n1\n1\n4\n4\n5\n"); // Easy, money 2000
    string output = runProgram("1\n5\n");

    // After first day income should be added to 2000
    // Cows: 2 * (15*50/100=7) = 14
    // Chickens: 3 * (5*50/100=2) = 6
    // Total: 2020
    printResult("Test 13: Income is credited (money grows)",
        contains(output, "Money: 2020"));
}

// Test 14: No actions left — program requires ending day
void testNoActionsLeft()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n"); // Hard: 3 actions
    // Spend 3 actions on care -> warning should appear
    string output = runProgram("1\n"
        "2\n1\n2\n1\n2\n1\n" // 3 cares
        "1\n"                // end day
        "2\n5\n");           // exit
    printResult("Test 14: When actions run out — end day required",
        contains(output, "No actions left for today"));
}

// Test 15: Choosing 5 (exit game) — return to main menu
void testExitFromGame()
{
    removeSettings();
    runProgram("5\n");
    string output = runProgram("1\n5\n5\n");
    printResult("Test 15: Exit game to main menu",
        contains(output, "HAPPY FARM"));
}

// Test 16: Game works on Hard (3 actions)
void testGameHard()
{
    removeSettings();
    runProgram("3\n1\n3\n4\n4\n5\n");
    string output = runProgram("1\n"
        "1\n1\n1\n1\n1\n1\n" // 3 feeding attempts
        "2\n2\n5\n");        // end and exit
    bool ok = contains(output, "Day:") &&
        contains(output, "Actions: 3");
    printResult("Test 16: Game works on Hard", ok);
}

// ==================== MAIN ====================

int main()
{
    setupLocale();

    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  Tests: Happy Farm v3.0\n" << NC;
    cout << BLUE << "  (Lab #3: game logic)\n" << NC;
    cout << BLUE << "============================================\n\n" << NC;

    if (!checkBinaryExists())
    {
        cout << RED << "Binary " << BINARY << " not found!\n" << NC;
        cout << "First build the program:\n";
        cout << "  g++ -std=c++17 main.cpp -o farm\n";
        return 1;
    }

    cout << YELLOW << ">>> Running tests...\n\n" << NC;

    // Run all tests
    testStartGameShowsState();
    testStartGameHasAnimals();
    testActionsEasy();
    testActionsHard();
    testMoneyEasy();
    testMoneyHard();
    testDayMenu();
    testFeedAnimal();
    testCareAnimal();
    testBuyFood();
    testBuyAnimal();
    testEndDay();
    testIncome();
    testNoActionsLeft();
    testExitFromGame();
    testGameHard();

    // Final report
    cout << "\n";
    cout << BLUE << "============================================\n" << NC;
    cout << BLUE << "  FINAL REPORT\n" << NC;
    cout << BLUE << "============================================\n" << NC;
    cout << "Total:   " << total << "\n";
    cout << GREEN << "Passed:  " << passed << NC << "\n";
    cout << RED << "Failed:  " << failed << NC << "\n";

    if (failed == 0)
    {
        cout << GREEN << ">>> ALL TESTS PASSED! <<<\n" << NC;
        return 0;
    }

    cout << RED << ">>> SOME TESTS FAILED <<<\n" << NC;
    return 1;
}