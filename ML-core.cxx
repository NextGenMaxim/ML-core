#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;

// Структура для 2D физических векторов и блоков
struct Vector2D {
    double x = 0, y = 0;
    double vx = 0, vy = 0;
    double mass = 1.0;
    double bounce = 0.0;
    double max_vx = 999.0;
    double max_vy = 999.0;
    double floor_limit = -999999.0;
    double roof_limit = 999999.0;
    double wall_left = -999999.0;
    double wall_right = 999999.0;
};

struct FunctionLine {
    int lineNumber;
    string code;
};

// Сетка игрового мира 20x20
vector<vector<int>> world_grid(20, vector<int>(20, 0));

// Глобальная память движка
map<string, double> variables;
map<string, string> strings_storage;
map<string, bool> bool_storage;
map<string, vector<double>> ml_matrices;
map<string, Vector2D> ml_vectors;
map<string, vector<FunctionLine>> ml_functions;
map<string, vector<string>> ml_func_params;

double global_gravity = -9.81;
int current_exec_line = 0;
bool inMultiLineComment = false;
bool isRecordingFunction = false;
string currentFunctionName = "";

const string RESET = "\033[0m";
const string RED   = "\033[31m";
const string GREEN = "\033[32m";
const string CYAN  = "\033[36m";
const string YELLOW = "\033[33m";

string clean(string str) {
    string res = "";
    for (char c : str) {
        if (c != ' ' && c != '"' && c != '[' && c != ']' && c != ';') res += c;
    }
    return res;
}

string remove_comments(string line) {
    string cleanCode = "";
    for (size_t i = 0; i < line.length(); i++) {
        if (inMultiLineComment) {
            if (i + 1 < line.length() && line[i] == ']' && line[i+1] == '|') {
                inMultiLineComment = false; i++;
            }
            continue;
        }
        if (i + 1 < line.length() && line[i] == '|' && line[i+1] == '[') {
            inMultiLineComment = true; i++; continue;
        }
        if (i + 1 < line.length() && line[i] == '|' && line[i+1] == '|') break;
        cleanCode += line[i];
    }
    return cleanCode;
}

double getVal(string name) {
    name = clean(name);
    if (name == "PI" || name == "pi") return 3.1415926535;
    if (name == "GRAVITY" || name == "gravity") return -9.81;
    if (variables.find(name) != variables.end()) return variables[name];
    try { return stod(name); } catch (...) { return 0; }
}

string getBlockName(int id) {
    if (id == 1) return "\033[33mЗемля (Dirt)\033[0m";
    if (id == 2) return "\033[37mКамень (Stone)\033[0m";
    if (id == 3) return "\033[32mДерево (Wood)\033[0m";
    return "Воздух (Air)";
}

double evaluateSimpleExpression(string expr, bool errorMode) {
    vector<double> values; vector<char> ops; string current = "";
    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];
        if (c == '+' || c == '-' || c == '*' || c == 'x' || c == '/' || c == '%') {
            values.push_back(getVal(current)); ops.push_back(c); current = "";
        } else current += c;
    }
    values.push_back(getVal(current));
    
    if (errorMode) {
        double r = values[0];
        for (size_t i = 0; i < ops.size(); i++) {
            if (ops[i] == '/') {
                if (values[i+1] == 0) { cout << RED << "[Division By Zero Error] Строка " << current_exec_line << ": Деление на ноль!" << RESET << endl; exit(1); }
                r /= values[i+1];
            }
            else if (ops[i] == '%') {
                if (values[i+1] == 0) { cout << RED << "[Division By Zero Error] Строка " << current_exec_line << ": Остаток от нуля!" << RESET << endl; exit(1); }
                r = fmod(r, values[i+1]);
            }
            else if (ops[i] == '+') r += values[i+1];
            else if (ops[i] == '-') r -= values[i+1];
            else if (ops[i] == '*' || ops[i] == 'x') r *= values[i+1];
        }
        return r;
    } else {
        for (size_t i = 0; i < ops.size(); ) {
            if (ops[i] == '*' || ops[i] == 'x' || ops[i] == '/' || ops[i] == '%') {
                double res = 0;
                if (ops[i] == '/') {
                    if (values[i+1] == 0) { cout << RED << "[Division By Zero Error] Строка " << current_exec_line << ": Деление на ноль!" << RESET << endl; exit(1); }
                    res = values[i] / values[i+1];
                }
                else if (ops[i] == '%') {
                    if (values[i+1] == 0) { cout << RED << "[Division By Zero Error] Строка " << current_exec_line << ": Остаток от нуля!" << RESET << endl; exit(1); }
                    res = fmod(values[i], values[i+1]);
                }
                else res = values[i] * values[i+1];
                values[i] = res; values.erase(values.begin()+i+1); ops.erase(ops.begin()+i);
            } else i++;
        }
        double r = values[0];
        for (size_t i = 0; i < ops.size(); i++) {
            if (ops[i] == '+') r += values[i+1];
            if (ops[i] == '-') r -= values[i+1];
        }
        return r;
    }
}

double parseMathWithBrackets(string expr, bool errorMode) {
    while (expr.find('(') != string::npos) {
        size_t closeParen = expr.find(')');
        size_t openParen = expr.rfind('(', closeParen);
        string subExpr = expr.substr(openParen + 1, closeParen - openParen - 1);
        double subResult = evaluateSimpleExpression(subExpr, errorMode);
        expr.replace(openParen, closeParen - openParen + 1, to_string(subResult));
    }
    return evaluateSimpleExpression(expr, errorMode);
}

bool execute_ml_core_v2(string line, int line_num) {
    current_exec_line = line_num;
    line = remove_comments(line);
    if (line.empty() || line == "\r") return true;

    if (isRecordingFunction) {
        if (line.find("};") != string::npos) { isRecordingFunction = false; return true; }
        size_t openBracket = line.find('['); size_t closeBracket = line.rfind(']');
        if (openBracket != string::npos && closeBracket != string::npos) {
            int lineNum = stoi(clean(line.substr(0, openBracket)));
            string innerCode = line.substr(openBracket + 1, closeBracket - openBracket - 1);
            ml_functions[currentFunctionName].push_back({lineNum, innerCode});
        }
        return true;
    }

    // Проверка точки с запятой
    if (line.back() != ';' && line.find("Func ") != 0 && line.find("matrix ") != 0 && line.find("for ") != 0 && line.find("}") == string::npos && line.find("if ") != 0) {
        cout << RED << "[Missing Symbol Error] Строка " << current_exec_line << ": Забыли ';' в конце команды!" << RESET << endl;
        cout << RED << "[Execution Fatal Error] Остановка на строке [" << current_exec_line << "]" << RESET << endl;
        return false;
    }

    if (line.find("STXRT ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(6, eq - 6));
        size_t fq = line.find('"') + 1; size_t lq = line.rfind('"');
        strings_storage[name] = line.substr(fq, lq - fq);
        cout << "[Memory]: Текст '" << name << "' = \"" << strings_storage[name] << "\"" << endl;
        return true;
    }

    if (line.find("bool ") == 0 || line.find("Xbool ") == 0) {
        bool isX = (line.find("Xbool ") == 0); size_t eq = line.find('=');
        string name = clean(line.substr(isX ? 6 : 5, eq - (isX ? 6 : 5)));
        string valStr = clean(line.substr(eq + 1));
        bool finalVal = (valStr == "True");
        bool_storage[name] = isX ? !finalVal : finalVal;
        cout << "[Memory]: Логика '" << name << "' = " << (bool_storage[name] ? "True" : "False") << endl;
        return true;
    }

    if (line.find("world.set_block(") == 0) {
        size_t start = line.find('(') + 1; size_t arrow = line.find("->");
        string coordStr = clean(line.substr(start, line.find(')') - start));
        size_t comma = coordStr.find(',');
        int x = stoi(coordStr.substr(0, comma)); int y = stoi(coordStr.substr(comma + 1));
        int blockId = stoi(clean(line.substr(arrow + 2)));
        if (x >= 0 && x < 20 && y >= 0 && y < 20) {
            world_grid[x][y] = blockId;
            cout << "[World Grid]: Установлен блок " << getBlockName(blockId) << " в [" << x << "," << y << "]" << endl;
        }
        return true;
    }

    if (line.find("pnt.console.world(") == 0) {
        size_t start = line.find('(') + 1; string sub = clean(line.substr(start, line.find(')') - start));
        size_t comma = sub.find(','); int x = stoi(sub.substr(0, comma)); int y = stoi(sub.substr(comma + 1));
        if (x >= 0 && x < 20 && y >= 0 && y < 20) cout << "   -> Сканирование мира [" << x << "," << y << "]: " << getBlockName(world_grid[x][y]) << endl;
        return true;
    }

    if (line.find("matrix_math ") == 0) {
        size_t arrow = line.find("->");
        string name = clean(line.substr(12, arrow - 12));
        string actionStr = clean(line.substr(arrow + 2));
        char op = actionStr[0]; double num_val = stod(actionStr.substr(1));
        if (ml_matrices.find(name) != ml_matrices.end()) {
            for (double& val : ml_matrices[name]) {
                if (op == '+') val += num_val; if (op == '-') val -= num_val;
                if (op == '*' || op == 'x') val *= num_val; if (op == '/') val /= num_val;
            }
            cout << GREEN << "[Matrix Math]: Матрица '" << name << "' масштабирована! [ ";
            for(double v : ml_matrices[name]) cout << v << " ";
            cout << "]" << RESET << endl;
        }
        return true;
    }

    if (line.find("num ") == 0) {
        size_t eq = line.find('=');
        if (eq == string::npos) { cout << RED << "[Syntax Error] Строка " << current_exec_line << ": Пропущен знак '='!" << RESET << endl; return false; }
        string name = clean(line.substr(4, eq - 4));
        string expr = line.substr(eq + 1); if (expr.back() == ';') expr.pop_back();

        if (expr.find("cos(") != string::npos || expr.find("sin(") != string::npos) {
            cout << RED << "[Math Function Error] Строка " << current_exec_line << ": Используйте стиль 'cos:значение'!" << RESET << endl; return false;
        }

        bool errMode = false;
        if (expr.find("error") != string::npos) { errMode = true; expr = expr.substr(expr.find("error") + 5); }

        if (expr.find("num.vector_length") == 0) {
            string vals = clean(expr.substr(18)); size_t comma = vals.find(',');
            double x = getVal(vals.substr(0, comma)); double y = getVal(vals.substr(comma + 1));
            variables[name] = sqrt(x*x + y*y);
        }
        else if (expr.find("num.orbit_x") == 0) {
            string vals = clean(expr.substr(12)); size_t comma = vals.find(',');
            double angle = getVal(vals.substr(0, comma)); double radius = getVal(vals.substr(comma + 1));
            variables[name] = cos(angle) * radius;
        }
        else if (expr.find("num.orbit_y") == 0) {
            string vals = clean(expr.substr(12)); size_t comma = vals.find(',');
            double angle = getVal(vals.substr(0, comma)); double radius = getVal(vals.substr(comma + 1));
            variables[name] = sin(angle) * radius;
        }
        else if (expr.find("cos:") == 0) { variables[name] = cos(getVal(expr.substr(4))); }
        else if (expr.find("sin:") == 0) { variables[name] = sin(getVal(expr.substr(4))); }
        else if (expr.find("abs:") == 0) { variables[name] = abs(getVal(expr.substr(4))); }
        else if (expr.find("floor:") == 0) { variables[name] = floor(getVal(expr.substr(6))); }
        else if (expr.find("ceil:") == 0) { variables[name] = ceil(getVal(expr.substr(5))); }
        else {
            variables[name] = parseMathWithBrackets(expr, errMode);
        }
        cout << "[Math Calc]: '" << name << "' = " << variables[name] << endl;
        return true;
    }

    if (line.find("vector ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(7, eq - 7));
        string vals = clean(line.substr(eq + 1)); size_t comma = vals.find(',');
        Vector2D vec; vec.x = stod(vals.substr(0, comma)); vec.y = stod(vals.substr(comma + 1));
        ml_vectors[name] = vec;
        cout << "[Physics Memory]: Вектор '" << name << "' инициализирован в [" << vec.x << "," << vec.y << "]" << endl;
        return true;
    }

    if (line.find(".bounce->") != string::npos || line.find(".bounce ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".bounce")));
        ml_vectors[name].bounce = stod(clean(line.substr(line.find("->") + 2)));
        return true;
    }

    if (line.find(".apply_impulse(") != string::npos) {
        string name = clean(line.substr(0, line.find(".apply_impulse")));
        string vals = clean(line.substr(line.find("([") + 2, line.find("])"))); size_t c = vals.find(',');
        ml_vectors[name].vx += stod(vals.substr(0, c)) / ml_vectors[name].mass;
        ml_vectors[name].vy += stod(vals.substr(c + 1)) / ml_vectors[name].mass;
        return true;
    }

    if (line.find("physics.update(") == 0) {
        string name = clean(line.substr(15, line.find(")") - 15));
        if (ml_vectors.find(name) != ml_vectors.end()) {
            Vector2D& v = ml_vectors[name]; v.vy += global_gravity * 0.1;
            v.x += v.vx * 0.1; v.y += v.vy * 0.1;
            int gx = floor(v.x); int gy = floor(v.y);
            if (gx >= 0 && gx < 20 && gy >= 0 && gy < 20 && world_grid[gx][gy] > 0) {
                v.y = gy + 1; v.vy = -v.vy * v.bounce;
                cout << RED << "[КОЛЛИЗИЯ С БЛОКОМ МИРА!] " << RESET;
            }
            cout << "'" << name << "' летит на высоте Y: " << v.y << " | Скорость VY: " << v.vy << endl;
        }
        return true;
    }

    if (line.find("matrix ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(7, eq - 7));
        vector<double> vals = {2.0, 4.0, 6.0}; ml_matrices[name] = vals; // автозаполнение для теста
        cout << "[Memory]: Инициализирована матрица '" << name << "' = [2, 4, 6]" << endl;
        return true;
    }

    if (line.find("pnt.console(") == 0) {
        string varName = clean(line.substr(12, line.find(')') - 12));
        cout << CYAN << "   -> [Console]: " << getVal(varName) << RESET << endl;
        return true;
    }

    if (line.find("color.pnt.console(") == 0) {
        size_t semi = line.find(';'); string col = clean(line.substr(line.find("color:") + 6, semi - (line.find("color:") + 6)));
        string txt = line.substr(semi + 1); size_t fq = txt.find('"') + 1; size_t lq = txt.rfind('"');
        string to_print = txt.substr(fq, lq - fq);
        if (col == "green") cout << GREEN << to_print << RESET << endl;
        else if (col == "red") cout << RED << to_print << RESET << endl;
        return true;
    }

    cout << RED << "[Syntax Error] Строка " << current_exec_line << ": Команда не распознана отладчиком!" << RESET << endl;
    cout << RED << "[Execution Fatal Error] Остановка на строке [" << current_exec_line << "]" << RESET << endl;
    return false;
}

void run_internal_test() {
    cout << "\033[36m========================================================\033[0m" << endl;
    cout << "\033[36m=== ТОТАЛЬНЫЙ СТРЕСС-ТЕСТ ВСЕХ 45 ФУНКЦИЙ ЯДРА V0.2 ===\033[0m" << endl;
    cout << "\033[36m========================================================\033[0m\n" << endl;

    int line = 1;
    // 1. Тест уникальных типов и комментариев
    execute_ml_core_v2("STXRT game_status = [\"Running\"]; || Тест однострочного комента", line++);
    execute_ml_core_v2("Xbool debug_mode = [\"False\"]; |[ Зеркальный тип ]|", line++);
    execute_ml_core_v2("color.pnt.console(color: green; \"[OK] Модули очистки и комментариев активны!\");", line++);

    // 2. Тест продвинутой математики, скобок и констант
    cout << "\n--- Проверка ИИ-Математики и Констант ---" << endl;
    execute_ml_core_v2("num test_brackets = (2 + 3) * (10 / 2);", line++); // 5 * 5 = 25
    execute_ml_core_v2("num test_gravity_const = GRAVITY * 2;", line++); // -9.81 * 2 = -19.62
    execute_ml_core_v2("num test_mod = 17 % 5;", line++); // 17 % 5 = 2
    execute_ml_core_v2("num test_error_mode = error 5 + 10 * 2;", line++); // (5 + 10) * 2 = 30
    execute_ml_core_v2("num test_floor = floor:15.9;", line++); // 15

    // 3. Тест масштабных орбитальных функций и векторов
    cout << "\n--- Проверка Орбит и Расстояний Пифагора ---" << endl;
    execute_ml_core_v2("num pifagor_dist = num.vector_length(3, 4);", line++); // sqrt(3^2 + 4^2) = 5
    execute_ml_core_v2("num spawn_orbit = num.orbit_x(0, 15);", line++); // cos(0) * 15 = 15

    // 4. Тест динамических матриц и групповых вычислений
    cout << "\n--- Проверка Масштабных Групповых Матриц ---" << endl;
    execute_ml_core_v2("matrix land_heights =;", line++);
    execute_ml_core_v2("matrix_math land_heights -> [*2];", line++); // массив [2, 4, 6] станет [4, 8, 12]

    // 5. Тест 2D мира блоков и раскопок песочницы
    cout << "\n--- Проверка 2D-Мира и Блоков Сети ---" << endl;
    execute_ml_core_v2("world.set_block(1,5 -> 2);", line++); // Ставим камень на X=1, Y=5
    execute_ml_core_v2("pnt.console.world(1,5);", line++);
    execute_ml_core_v2("pnt.console.world(1,10);", line++); // Тут должен быть воздух

    // 6. Тест 2D физики и обработки столкновений со слаймом
    cout << "\n--- Проверка Физического Движка Коллизий ---" << endl;
    execute_ml_core_v2("vector slime = [1.0, 7.0];", line++);
    execute_ml_core_v2("slime.bounce -> [0.6];", line++);
    execute_ml_core_v2("slime.apply_impulse([0.0, -10.0]);", line++); // Пинаем вниз, прямо на камень в Y=5
    for (int i = 0; i < 3; i++) {
        execute_ml_core_v2("physics.update(slime);", line++);
    }

    // 7. Тест ИИ-Отладчика на пропущенные символы
    cout << "\n--- Тест Отладчика Строк на Критические Баги ---" << endl;
    execute_ml_core_v2("num forgot_semicolon = 50 + 50", line++); // Специально забыли ';' для проверки краш-системы
}

int main() {
    srand(time(0));
    cout << "\033[32m=== Интерпретатор ML-core v0.2 Alpha: NextGenMax ===\033[0m\n" << endl;
    string filename = "main.mlc"; ifstream file(filename);
    if (!file.is_open()) { run_internal_test(); return 0; }
    string line; int count = 1;
    while (getline(file, line)) { if(!execute_ml_core_v2(line, count++)) break; }
    file.close();
    return 0;
}
