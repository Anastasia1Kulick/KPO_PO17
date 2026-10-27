#include <iostream>
#include <windows.h>
#include <string>

using namespace std;

const int START = 1;
const int LOAD = 2;
const int SETTINGS = 3;
const int INFO = 4;
const int EXIT = 5;

const string DEFAULT_DIFFICULTY = "Легкий";
const int DEFAULT_ALL_PLAYERS = 4;
const int DEFAULT_HUMAN_PLAYERS = 2;

string difficulty = DEFAULT_DIFFICULTY;
int allPlayers = DEFAULT_ALL_PLAYERS;
int humanPlayers = DEFAULT_HUMAN_PLAYERS;

void Start();
void Load();
void Settings();
void Info();
void Exit();

int main() {
	setlocale(LC_ALL, "Rus");
	int choice = 0;

	do {
		system("cls");
		cout << "\tСОРЕВНОВАНИЯ АРМИЙ\n\n\t" << START << ". Начать игру\n\t" << LOAD << ". Загрузить игру\n\t" << SETTINGS << ". Настройки\n\t" << INFO << ". О программе\n\t" << EXIT << ". Выход\n";
		cin >> choice;

		switch (choice) {
		case START: Start(); break;
		case LOAD: Load(); break;
		case SETTINGS: Settings(); break;
		case INFO: Info(); break;
		case EXIT: Exit(); break;
		default: cout << "\n\tНеверный пункт\n";
			Sleep(1000);
		}
	} while (choice != EXIT);
	return 0;
}

void Start() {
	system("cls");
	cout << "\tНачало игры\n";
	Sleep(1000);
	cout << "\n\t\t...Нажмите Enter чтобы вернуться в главное меню\n";
	cin.ignore();
	cin.get();
}
void Load() {
	system("cls");
	cout << "\tНа данный момент функция недоступна\n";
	Sleep(1000);
	cout << "\n\t\t...Нажмите Enter чтобы вернуться в главное меню\n";
	cin.ignore();
	cin.get();
}
void Settings() {
	int setChoice = 0;

	do {
		system("cls");

		int aiPlayers = allPlayers - humanPlayers;
		cout << "\t\t\nНАСТРОЙКИ\n" << "\n\t1. Уровень сложности: " << difficulty << "\n\t2. Всего игроков: " << allPlayers << "\n\t3. Живых игроков: " << humanPlayers << "\n\t4. Игроков ИИ: " << aiPlayers << "\n\t5. Сбросить настройки по умолчанию" << "\n\t6. Вернуться в главное меню" << "\nВыберите пункт: ";
		cin >> setChoice;

		switch (setChoice) {
		case 1: {
			int diffChoice = 0;
			cout << "\n\tВыберите сложность:\n" << "\t1. Легкий\n" << "\t2. Средний\n" << "\t3. Сложный\n" << "\tВаш выбор: ";
			cin >> diffChoice;
			if (diffChoice == 1) difficulty = "Легкий";
			else if (diffChoice == 2) difficulty = "Средний";
			else if (diffChoice == 3) difficulty = "Сложный";
			else cout << "\tНеверный выбор!\n";
			break;
		}
		case 2: {
			int tempAll = 0;
			cout << "\n\tВведите общее количество игроков (от 2 до 5): ";
			cin >> tempAll;
			if (tempAll >= 2 && tempAll <= 5) {
				allPlayers = tempAll;
				if (humanPlayers > allPlayers) {
					humanPlayers = allPlayers;
				}
			}
			else {
				cout << "\t[Ошибка]: Число игроков должно быть от 2 до 5!\n";
				Sleep(1500);
			}
			break;
		}
		case 3: {
			int tempHuman = 0;
			cout << "\n\tВведите количество живых игроков (от 1 до " << allPlayers << "): ";
			cin >> tempHuman;
			if (tempHuman >= 1 && tempHuman <= allPlayers) {
				humanPlayers = tempHuman;
			}
			else {
				cout << "\t[Ошибка]: Живых игроков не может быть больше, чем всего (" << allPlayers << ")!\n";
				Sleep(1500);
			}
			break;
		}
		case 4:
			cout << "\n\tЭтот параметр считается автоматически: (Всего - Живые)\n";
			Sleep(1500);
			break;

		case 5:
			difficulty = DEFAULT_DIFFICULTY;
			allPlayers = DEFAULT_ALL_PLAYERS;
			humanPlayers = DEFAULT_HUMAN_PLAYERS;
			cout << "\n\tНастройки сброшены к значениям по умолчанию!\n";
			Sleep(1200);
			break;

		case 6:
			break;

		default:
			cout << "\n\tНеверный пункт!\n";
			Sleep(1000);
		}
	} while (setChoice != 6);
}
void Info() {
	system("cls");
	cout << "\t\tО ПРОГРАММЕ\n\n\tВерсия программы: 1.0\n\tРазработчик: Яна Леонович\n\n\t...Нажмите Enter чтобы вернуться в главное меню\n";
	cin.ignore();
	cin.get();
}
void Exit() {
	system("cls");
	cout << "\tЗавершение работы программы...\n";
	Sleep(1000);
}
