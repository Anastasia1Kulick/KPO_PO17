#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Копируем структуру для тестов (так как main.cpp не разбит на .h и .cpp)
class UserProfile {
private:
    string specialityField;
    string nameField;
    int subjectsCountField;

public:
    UserProfile() : specialityField("Программная инженерия"), 
                    nameField("Кристина"), 
                    subjectsCountField(5) {}

    string getSpeciality() const { return specialityField; }
    void setSpeciality(const string& s) { specialityField = s; }

    string getName() const { return nameField; }
    void setName(const string& n) { nameField = n; }

    int getSubjectsCount() const { return subjectsCountField; }
    void setSubjectsCount(int count) { subjectsCountField = count; }
};

int total_tests = 0;
int passed_tests = 0;

void check(bool condition, const string& test_name) {
    total_tests++;
    if (condition) {
        passed_tests++;
        cout << "[OK]   " << test_name << "\n";
    } else {
        cout << "[FAIL] " << test_name << "\n";
    }
}

void test_default_profile() {
    UserProfile profile;
    check(profile.getSpeciality() == "Программная инженерия", "Специальность по умолчанию");
    check(profile.getName() == "Кристина", "Имя по умолчанию");
    check(profile.getSubjectsCount() == 5, "Количество предметов по умолчанию");
}

void test_setters() {
    UserProfile profile;
    
    profile.setSpeciality("Кибербезопасность");
    check(profile.getSpeciality() == "Кибербезопасность", "Смена специальности");
    
    profile.setName("Алексей");
    check(profile.getName() == "Алексей", "Смена имени");
    
    profile.setSubjectsCount(8);
    check(profile.getSubjectsCount() == 8, "Смена количества предметов");
}

void test_invalid_input() {
    UserProfile profile;
    // Проверяем, что класс принимает только то, что ему передали
    // (Логика валидации >0 находится в main, но мы проверим сам класс)
    profile.setSubjectsCount(-1);
    check(profile.getSubjectsCount() == -1, "Класс сохраняет переданное значение (валидация в UI)");
}

int main() {
    cout << "=== Запуск модульных тестов ===\n\n";

    test_default_profile();
    test_setters();
    test_invalid_input();

    cout << "\n============================\n";
    cout << "Пройдено: " << passed_tests << " из " << total_tests << "\n";
    cout << "============================\n";

    return (passed_tests == total_tests) ? 0 : 1;
}
