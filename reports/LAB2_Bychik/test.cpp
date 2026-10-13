#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "settings.h"

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    out << content;
}

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    std::string s, line;
    while (std::getline(in, line)) s += line + "\n";
    return s;
}

void removeFile(const std::string& path) {
    std::remove(path.c_str());
}

TEST(EnsureFile, CreatesFileIfMissing) {
    std::string path = "test_ensure_tmp.txt";
    removeFile(path);

    ensureFile(path, "hello");

    std::ifstream f(path);
    EXPECT_TRUE(f.good());          // файл должен появиться
    EXPECT_EQ(readFile(path), "hello\n");
    removeFile(path);
}

TEST(EnsureFile, DoesNotOverwriteExisting) {
    std::string path = "test_ensure_tmp2.txt";
    writeFile(path, "old content\n");

    ensureFile(path, "new content");

    EXPECT_EQ(readFile(path), "old content\n");  // старое не должно затереться
    removeFile(path);
}


TEST(Settings, SaveThenLoadRoundTrip) {
    std::string path = "test_settings_tmp.txt";

    Settings s1;
    s1.speciality    = "Кибербезопасность";
    s1.studentName   = "Иван";
    s1.subjectsCount = 7;
    saveSettings(s1, path);

    Settings s2;                     // значения по умолчанию
    loadSettings(s2, path);

    EXPECT_EQ(s2.speciality,    "Кибербезопасность");
    EXPECT_EQ(s2.studentName,   "Иван");
    EXPECT_EQ(s2.subjectsCount, 7);

    removeFile(path);
}

TEST(Settings, FileFormatIsKeyValue) {
    std::string path = "test_settings_fmt.txt";
    Settings s;
    s.speciality = "ИТ";
    s.studentName = "Анна";
    s.subjectsCount = 3;
    saveSettings(s, path);

    std::string content = readFile(path);
    EXPECT_NE(content.find("speciality=ИТ"),        std::string::npos);
    EXPECT_NE(content.find("studentName=Анна"),     std::string::npos);
    EXPECT_NE(content.find("subjectsCount=3"),      std::string::npos);
    removeFile(path);
}

TEST(Settings, LoadFromMissingFileKeepsDefaults) {
    std::string path = "no_such_file_12345.txt";
    removeFile(path);

    Settings s;                       // дефолтные значения
    loadSettings(s, path);

    EXPECT_EQ(s.speciality,    "Программная инженерия");
    EXPECT_EQ(s.studentName,   "Кристина");
    EXPECT_EQ(s.subjectsCount, 5);
}

TEST(Settings, LoadHandlesGarbageNumber) {
    std::string path = "test_garbage.txt";
    writeFile(path,
        "speciality=ИТ\n"
        "studentName=Пётр\n"
        "subjectsCount=abc\n");      // мусор вместо числа

    Settings s;
    loadSettings(s, path);

    EXPECT_EQ(s.speciality,  "ИТ");
    EXPECT_EQ(s.studentName, "Пётр");
    EXPECT_EQ(s.subjectsCount, 5);   // должно остаться дефолтное 5
    removeFile(path);
}

TEST(Settings, LoadIgnoresLinesWithoutEquals) {
    std::string path = "test_noeq.txt";
    writeFile(path,
        "speciality=ИТ\n"
        "это строка без знака равно\n"
        "subjectsCount=10\n");

    Settings s;
    loadSettings(s, path);

    EXPECT_EQ(s.speciality, "ИТ");
    EXPECT_EQ(s.subjectsCount, 10);
    removeFile(path);
}

TEST(LoadLines, IntoList) {
    std::string path = "test_lines_list.txt";
    writeFile(path, "Кристина\nИван\n\nАнна\n");

    std::list<std::string> lst;
    loadLines(path, lst);

    ASSERT_EQ(lst.size(), 3u);          // пустая строка пропущена
    auto it = lst.begin();
    EXPECT_EQ(*it++, "Кристина");
    EXPECT_EQ(*it++, "Иван");
    EXPECT_EQ(*it++, "Анна");
    removeFile(path);
}

TEST(LoadLines, IntoSetSortsAndDeduplicates) {
    std::string path = "test_lines_set.txt";
    writeFile(path, "Веб-разработка\nКибербезопасность\nВеб-разработка\n");

    std::set<std::string> st;
    loadLines(path, st);

    ASSERT_EQ(st.size(), 2u);           
    auto it = st.begin();
    EXPECT_EQ(*it++, "Веб-разработка");
    EXPECT_EQ(*it++, "Кибербезопасность"); 
    removeFile(path);
}

TEST(LoadLines, ClearsContainerBeforeLoading) {
    std::string path = "test_lines_clear.txt";
    writeFile(path, "A\nB\n");

    std::list<std::string> lst = {"старое", "мусор"};
    loadLines(path, lst);

    EXPECT_EQ(lst.size(), 2u);
    auto it = lst.begin();
    EXPECT_EQ(*it++, "A");
    EXPECT_EQ(*it++, "B");
    removeFile(path);
}

TEST(InitFiles, CreatesAllThreeFiles) {
    removeFile(SETTINGS_FILE);
    removeFile(STUDENTS_FILE);
    removeFile(SPECIALITY_FILE);

    Settings d;
    initFiles(d);

    std::ifstream f1(SETTINGS_FILE), f2(STUDENTS_FILE), f3(SPECIALITY_FILE);
    EXPECT_TRUE(f1.good());
    EXPECT_TRUE(f2.good());
    EXPECT_TRUE(f3.good());

    removeFile(SETTINGS_FILE);
    removeFile(STUDENTS_FILE);
    removeFile(SPECIALITY_FILE);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
