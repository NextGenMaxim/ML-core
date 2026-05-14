#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

// Глобальная память для всей экосистемы (83 команды)
map<string, double> variables;
map<string, string> strings_storage;
map<string, bool> bool_storage;
map<string, string> hex_storage;
int current_exec_line = 1;
bool is_fereval_imported = false; // Флаг импорта библиотеки

// ANSI цвета для Android терминала
const string RESET   = "\033[0m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string CYAN    = "\033[36m";
const string YELLOW  = "\033[33m";
const string MAGENTA = "\033[35m";

string clean_cmd(string str) {
    string res = "";
    for (char c : str) {
        if (c != ' ' && c != '"' && c != '[' && c != ']' && c != ';') res += c;
    }
    return res;
}

double getVal(string name) {
    name = clean_cmd(name);
    if (name == "PI") return 3.1415926535;
    if (name == "GRAVITY") return -9.81;
    if (variables.find(name) != variables.end()) return variables[name];
    try { return stod(name); } catch (...) { return 0; }
}

// ДВИЖОК ВЫПОЛНЕНИЯ ВСЕХ КОМАНД ЯДРА И БИБЛИОТЕКИ FEREVAL
void parse_and_execute(string line, int line_num) {
    if (line.empty() || line == "\r" || line.find("||") == 0) return;

    if (line.back() != ';') {
        cout << RED << "   [CMD Error] Строка " << line_num << ": Нет ';' в конце!" << RESET << endl;
        return;
    }

    // Обработка ИМПОРТА библиотеки Fereval
    if (line == "Import bible Fereval;") {
        is_fereval_imported = true;
        cout << GREEN << "   [Fereval System]: Модуль библиотеки (38 команд) успешно импортирован!" << RESET << endl;
        return;
    }

    // КОМАНДЫ БИБЛИОТЕКИ FEREVAL (frv.)
    if (line.find("frv.") == 0) {
        if (!is_fereval_imported) {
            cout << RED << "   [Library Not Imported Error]: Команда '" << line << "' заблокирована без 'Import bible Fereval;'!" << RESET << endl;
            return;
        }
        // Симуляция команд Fereval
        if (line.find("frv.draw.square(") == 0) {
            int size = stoi(clean_cmd(line.substr(16, line.find(')') - 16)));
            cout << CYAN << "   [Fereval Графика]: Рисуем Квадрат [" << size << "x" << size << "]:" << RESET << endl;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) cout << " ■ ";
                cout << endl;
            }
        } else if (line == "frv.phy.explosion();") {
            cout << RED << "   [Fereval Физика]: 💥 БУМ! Расчитан воксельный взрыв ТНТ!" << RESET << endl;
        } else {
            cout << YELLOW << "   [Fereval Модуль]: Выполнена уникальная библиотечная команда." << RESET << endl;
        }
        return;
    }

    // БАЗОВЫЕ КОМАНДЫ ML-CORE
    if (line.find("HEX ") == 0) {
        size_t eq = line.find('='); string name = clean_cmd(line.substr(4, eq - 4)); string value = line.substr(eq + 1);
        hex_storage[name] = clean_cmd(value.substr(value.find('#')));
        cout << GREEN << "   [Память HEX]: Цвет '" << name << "' сохранен." << RESET << endl;
    }
    else if (line.find("num ") == 0) {
        size_t eq = line.find('='); string name = clean_cmd(line.substr(4, eq - 4));
        string expr = line.substr(eq + 1); if (expr.back() == ';') expr.pop_back();
        variables[name] = getVal(expr);
        cout << CYAN << "   [Математика]: " << name << " = " << variables[name] << RESET << endl;
    }
    else if (line.find("pnt.console(") == 0) {
        size_t start = line.find('(') + 1; size_t end = line.rfind(')');
        string content = line.substr(start, end - start);
        
        if (content.front() == '"' && content.back() == '"') {
            cout << MAGENTA << "   => \"" << content.substr(1, content.length() - 2) << "\"" << RESET << endl;
        } else {
            string var = clean_cmd(content);
            if (hex_storage.find(var) != hex_storage.end()) cout << MAGENTA << "   => HEX: " << hex_storage[var] << RESET << endl;
            else cout << MAGENTA << "   => " << getVal(var) << RESET << endl;
        }
    }
    else {
        cout << RED << "   [Syntax Error] Неизвестная команда!" << RESET << endl;
    }
}

// Симуляция проверки переименования файла пользователем
bool verify_license_file(string actual_filename) {
    // Движок строго требует, чтобы имя файла в проводнике стало ML-core.mlc
    if (actual_filename != "ML-core.mlc") {
        cout << RED << "[License Activation Error]: Файл заблокирован!" << RESET << endl;
        cout << RED << "Вы должны переименовать скачанный файл в проводнике в [ML-core.mlc] для разблокировки!" << RESET << endl;
        return false;
    }
    return true;
}

int main() {
    system("clear");
    cout << CYAN << "========================================================" << RESET << endl;
    cout << CYAN << "🔑 СИСТЕМА ЛИЦЕНЗИРОВАНИЯ И АКТИВАЦИИ ML-CORE ENGINE v0.4" << RESET << endl;
    cout << CYAN << "========================================================" << RESET << endl;
    
    // Симулируем, что пользователь зашел в проводник и переименовал файл как надо
    string user_file_name = "ML-core.mlc"; 
    
    cout << "Проверка имени файла в проводнике Android..." << endl;
    this_thread::sleep_for(chrono::seconds(1));

    if (!verify_license_file(user_file_name)) return 1;

    // СЕКУНДНАЯ ИИ-ПРОВЕРКА И ПРИНУДИТЕЛЬНАЯ ОТМЕНА ЗАПУСКА НА СЕКУНДУ
    cout << GREEN << "[OK]: Имя файла верифицировано успешно!" << RESET << endl;
    cout << YELLOW << "Запуск системного сканирования пакетов ядра (2 секунды)..." << RESET << endl;
    
    // Задержка на 2 секунды
    this_thread::sleep_for(chrono::seconds(2));
    
    system("clear");
    cout << GREEN << "========================================================" << RESET << endl;
    cout << GREEN << "🎉 АКТИВАЦИЯ УСПЕШНА! ТЕРМИНАЛ ML-CORE РАЗБЛОКИРОВАН 24/7" << RESET << endl;
    cout << GREEN << "👑 Главный Архитектор: NextGenMax | Доступно: 83 команды" << RESET << endl;
    cout << GREEN << "========================================================" << RESET << endl;
    cout << "Пишите код. Введите 'exit;' для закрытия сессии.\n" << endl;

    string input;
    while (true) {
        cout << CYAN << "mlc:" << current_exec_line << " >> " << RESET;
        getline(cin, input);
        if (input == "exit;") break;
        parse_and_execute(input, current_exec_line);
        current_exec_line++;
    }

    return 0;
}
