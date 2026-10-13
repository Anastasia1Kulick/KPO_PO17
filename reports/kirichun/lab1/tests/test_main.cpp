#define main original_main
#include "../src/main.cpp"
#undef main

#include <iostream>
#include <string>
#include <cassert>

#ifndef _WIN32
inline void SetConsoleCP(unsigned int) {}
inline void SetConsoleOutputCP(unsigned int) {}
#define system(cmd)
#endif

void test_default_settings() {
    SETTING set;
    assert(set.specialty == "Программная инженерия");
    assert(set.studentName == "Аня");
    assert(set.subjectCount == 11);
    std::cout << "test_default_settings passed\n";
}

void test_setting_modification() {
    SETTING set;
    set.studentName = "Анна";
    set.subjectCount = 10;
    
    assert(set.studentName == "Анна");
    assert(set.subjectCount == 10);
    std::cout << "test_setting_modification passed\n";
}

int main() {
    test_default_settings();
    test_setting_modification();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
