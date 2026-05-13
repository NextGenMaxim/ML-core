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

// Структура для 2D физических векторов
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

// Структура для пронумерованных строк функций
struct FunctionLine {
    int lineNumber;
    string code;
};

// Глобальная память ML-core v0.1 Beta
map<string, double> variables;
map<string, string> strings_storage;
map<string, bool> bool_storage;
map<string, vector<double>> ml_matrices;
map<string, Vector2D> ml_vectors;
map<string, vector<FunctionLine>> ml_functions;
map<string, vector<string>> ml_func_params;

double global_gravity = -9.81;
bool inMultiLineComment = false;
bool isRecordingFunction = false;
string currentFunctionName = "";

// ANSI цвета для консоли
const string RESET = "\033[0m";
const string RED   = "\033[31m";
const string GREEN = "\033[32m";
const string CYAN  = "\033[36m";
const string YELLOW = "\033[33m";

// Очистка строк от мусора
string clean(string str) {
    string res = "";
    for (char c : str) {
        if (c != ' ' && c != '"' && c != '[' && c != ']' && c != ';') res += c;
    }
    return res;
}

// Удаление уникальных комментариев || и |[ ]|
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
    if (variables.find(name) != variables.end()) return variables[name];
    try { return stod(name); } catch (...) { return 0; }
}

double lerp(double start, double end, double t) { return start + t * (end - start); }

// Математический парсер (Обычный + Твой Error-режим)
double parseMathExpression(string expr, bool errorMode) {
    vector<double> values; vector<char> ops; string current = "";
    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];
        if (c == '+' || c == '-' || c == '*' || c == 'x' || c == '/') {
            values.push_back(getVal(current)); ops.push_back(c); current = "";
        } else current += c;
    }
    values.push_back(getVal(current));
    
    if (errorMode) {
        // ИСПРАВЛЕНО: Стартуем с первого числа массива, а не со всего вектора!
        double r = values[0]; 
        for (size_t i = 0; i < ops.size(); i++) {
            if (ops[i] == '+') r += values[i+1];
            if (ops[i] == '-') r -= values[i+1];
            if (ops[i] == '*' || ops[i] == 'x') r *= values[i+1];
            if (ops[i] == '/') r /= values[i+1];
        }
        return r;
    } else {
        for (size_t i = 0; i < ops.size(); ) {
            if (ops[i] == '*' || ops[i] == 'x' || ops[i] == '/') {
                double res = (ops[i] == '/') ? values[i]/values[i+1] : values[i]*values[i+1];
                values[i] = res; values.erase(values.begin()+i+1); ops.erase(ops.begin()+i);
            } else i++;
        }
        // ИСПРАВЛЕНО: Стартуем с первого числа массива!
        double r = values[0]; 
        for (size_t i = 0; i < ops.size(); i++) {
            if (ops[i] == '+') r += values[i+1];
            if (ops[i] == '-') r -= values[i+1];
        }
        return r;
    }
}

bool checkCondition(string condStr) {
    if (condStr.front() == '[') condStr.erase(0, 1);
    if (condStr.back() == ']') condStr.pop_back();
    string ops[] = {"==", "!=", ">=", "<=", ">", "<"};
    for (string op : ops) {
        size_t pos = condStr.find(op);
        if (pos != string::npos) {
            double v1 = getVal(condStr.substr(0, pos));
            double v2 = getVal(condStr.substr(pos + op.length()));
            if (op == "==") return v1 == v2;
            if (op == "!=") return v1 != v2;
            if (op == ">")  return v1 > v2;
            if (op == "<")  return v1 < v2;
            if (op == ">=") return v1 >= v2;
            if (op == "<=") return v1 <= v2;
        }
    }
    return false;
}

// Главный движок выполнения команд ML-core
double execute_ml_core(string line) {
    line = remove_comments(line);
    if (line.empty() || line == "\r") return 0;

    // ЗАПИСЬ ФУНКЦИИ В ПАМЯТЬ ДВИЖКА
    if (isRecordingFunction) {
        if (line.find("};") != string::npos) {
            isRecordingFunction = false;
            return 0;
        }
        size_t openBracket = line.find('['); size_t closeBracket = line.rfind(']');
        if (openBracket != string::npos && closeBracket != string::npos) {
            int lineNum = stoi(clean(line.substr(0, openBracket)));
            string innerCode = line.substr(openBracket + 1, closeBracket - openBracket - 1);
            ml_functions[currentFunctionName].push_back({lineNum, innerCode});
        }
        return 0;
    }

    // ОБЪЯВЛЕНИЕ ФУНКЦИИ: Func имя(параметры):
    if (line.find("Func ") == 0) {
        size_t openParen = line.find('('); size_t closeParen = line.find(')');
        currentFunctionName = clean(line.substr(5, openParen - 5));
        string paramsStr = line.substr(openParen + 1, closeParen - openParen - 1);
        vector<string> params; size_t pos = 0;
        while ((pos = paramsStr.find(',')) != string::npos) {
            params.push_back(clean(paramsStr.substr(0, pos))); paramsStr.erase(0, pos + 1);
        }
        if (!paramsStr.empty()) params.push_back(clean(paramsStr));
        ml_func_params[currentFunctionName] = params;
        ml_functions[currentFunctionName].clear();
        isRecordingFunction = true;
        return 0;
    }

    // УСЛОВИЯ IF/ELSE*
    if (line.find("if ") == 0) {
        size_t start = line.find("["); size_t end = line.rfind("]");
        string condition = line.substr(start, end - start + 1);
        if (checkCondition(condition)) cout << GREEN << "[If True]" << RESET << endl;
        else cout << YELLOW << "[If False]" << RESET << endl;
        return 0;
    }

    // УНИКАЛЬНЫЙ ЦИКЛ FOR С ПЕРЕМЕННОЙ-СЧЕТЧИКОМ
    if (line.find("for ") == 0) {
        size_t openParen = line.find('('); size_t closeParen = line.find(')');
        string loopDef = line.substr(openParen + 1, closeParen - openParen - 1);
        size_t eqPos = loopDef.find('=');
        string varName = clean(loopDef.substr(0, eqPos));
        string rangeStr = clean(loopDef.substr(eqPos + 1));
        size_t comma = rangeStr.find(',');
        
        if (comma == string::npos || rangeStr.empty()) {
            cout << RED << "[ML-core Error]: Ошибка цикла for! Укажите диапазон вида" << RESET << endl;
            return 0;
        }
        
        try {
            int startVal = stoi(rangeStr.substr(0, comma)); 
            int endVal = stoi(rangeStr.substr(comma + 1));
            size_t bodyStart = line.find('{') + 1; size_t bodyEnd = line.rfind('}');
            string body = line.substr(bodyStart, bodyEnd - bodyStart);
            for (int i = startVal; i <= endVal; i++) {
                variables[varName] = i; execute_ml_core(body);
            }
        } catch(...) {
            cout << RED << "[ML-core Error]: Неверные числа в цикле for!" << RESET << endl;
        }
        return 0;
    }

    // ТВОЯ КОМАНДА ДИНАМИЧЕСКИХ МАТРИЦ: имя.push -> [значение];
    if (line.find(".push->") != string::npos || line.find(".push ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".push")));
        double newValue = getVal(line.substr(line.find("->") + 2));
        if (ml_matrices.find(name) != ml_matrices.end()) ml_matrices[name].push_back(newValue);
        return 0;
    }

    // ВЫЗОВ ФУНКЦИИ С ПЕРЕДАЧЕЙ ДАННЫХ И RETURN
    if (line.find(");") != string::npos && line.find("Func ") == string::npos && line.find("for ") == string::npos && line.find("pnt.console") == string::npos) {
        size_t openParen = line.find('('); size_t closeParen = line.find(')');
        string funcName = clean(line.substr(0, openParen));
        if (ml_functions.find(funcName) != ml_functions.end()) {
            string argsStr = line.substr(openParen + 1, closeParen - openParen - 1);
            vector<double> passedArgs; size_t pos = 0;
            while ((pos = argsStr.find(',')) != string::npos) {
                passedArgs.push_back(getVal(argsStr.substr(0, pos))); argsStr.erase(0, pos + 1);
            }
            if (!argsStr.empty()) passedArgs.push_back(getVal(argsStr));
            map<string, double> backup = variables;
            for (size_t i = 0; i < ml_func_params[funcName].size() && i < passedArgs.size(); i++) {
                variables[ml_func_params[funcName][i]] = passedArgs[i];
            }
            double returnValue = 0;
            for (auto& fLine : ml_functions[funcName]) {
                if (fLine.code.find("return ") == 0) {
                    returnValue = getVal(fLine.code.substr(7)); break;
                }
                execute_ml_core(fLine.code);
            }
            variables = backup; return returnValue;
        }
    }

    // ЦВЕТНОЙ ВЫВОД
    if (line.find("color.pnt.console(") != string::npos) {
        size_t semi = line.find(';');
        string col = clean(line.substr(line.find("color:") + 6, semi - (line.find("color:") + 6)));
        string txt = line.substr(semi + 1); size_t fq = txt.find('"') + 1; size_t lq = txt.rfind('"');
        string to_print = txt.substr(fq, lq - fq);
        if (col == "red") cout << RED << to_print << RESET << endl;
        else if (col == "green") cout << GREEN << to_print << RESET << endl;
        else if (col == "cyan") cout << CYAN << to_print << RESET << endl;
        else cout << to_print << endl;
    }
    // ОБЫЧНЫЙ ВЫВОД И СВОЙСТВО .SIZE МАТРИЦЫ
    else if (line.find("pnt.console(") == 0) {
        if (line.find(".size") != string::npos) {
            string name = clean(line.substr(12, line.find(".size") - 12));
            if (ml_matrices.find(name) != ml_matrices.end()) cout << "   -> [Matrix Size]: " << ml_matrices[name].size() << endl;
        } else {
            size_t fq = line.find("\"");
            if (fq != string::npos) {
                size_t lq = line.rfind("\""); cout << line.substr(fq + 1, lq - fq - 1) << endl;
            } else {
                string varName = clean(line.substr(12, line.find(')') - 12));
                cout << "   -> [Значение]: " << getVal(varName) << endl;
            }
        }
    }
    // ТАЙМИНГИ КАДРА
    else if (line.find("physics.delay()") != string::npos) {
        this_thread::sleep_for(chrono::milliseconds(16));
    }
    // ТИПЫ ДАННЫХ И ХРАНИЛИЩА
    else if (line.find("bool ") == 0 || line.find("Xbool ") == 0) {
        bool isX = (line.find("Xbool ") == 0); size_t eq = line.find('=');
        string name = clean(line.substr(isX ? 6 : 5, eq - (isX ? 6 : 5)));
        bool_storage[name] = isX ? !(clean(line.substr(eq + 1)) == "True") : (clean(line.substr(eq + 1)) == "True");
    }
    else if (line.find("STXRT ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(6, eq - 6));
        size_t fq = line.find('"') + 1; size_t lq = line.rfind('"');
        strings_storage[name] = line.substr(fq, lq - fq);
    }
    else if (line.find("matrix ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(7, eq - 7));
        vector<double> vals; ml_matrices[name] = vals;
    }
    else if (line.find("vector ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(7, eq - 7));
        string vals = clean(line.substr(eq + 1)); size_t comma = vals.find(',');
        Vector2D vec; vec.x = stod(vals.substr(0, comma)); vec.y = stod(vals.substr(comma + 1));
        ml_vectors[name] = vec;
    }
    else if (line.find("physics.gravity") == 0) {
        global_gravity = stod(clean(line.substr(line.find('=') + 1)));
    }
    else if (line.find("pnt.console.math(") != string::npos) {
        string expr = line.substr(17, line.find(')') - 17);
        if (expr.find("cos:") != string::npos) cout << "Math (Cos): " << cos(getVal(expr.substr(4))) << endl;
        else if (expr.find("sin:") != string::npos) cout << "Math (Sin): " << sin(getVal(expr.substr(4))) << endl;
        else if (expr.find("abs:") != string::npos) cout << "Math (Abs): " << abs(getVal(expr.substr(4))) << endl;
        else if (expr.find("floor:") != string::npos) cout << "Math (Floor): " << floor(getVal(expr.substr(6))) << endl;
        else if (expr.find("ceil:") != string::npos) cout << "Math (Ceil): " << ceil(getVal(expr.substr(5))) << endl;
    }
    else if (line.find("num ") == 0) {
        size_t eq = line.find('='); string name = clean(line.substr(4, eq - 4));
        string expr = line.substr(eq + 1); if(expr.back() == ';') expr.pop_back();
        bool errMode = false;
        if (expr.find("error") != string::npos) { errMode = true; expr = expr.substr(expr.find("error") + 5); }
        variables[name] = parseMathExpression(expr, errMode);
    }
    // ФИЗИКА ТОЧЕЧНОГО СИНТАКСИСИСА
    else if (line.find(".mass->") != string::npos || line.find(".mass ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".mass")));
        ml_vectors[name].mass = stod(clean(line.substr(line.find("->") + 2)));
    }
    else if (line.find(".bounce->") != string::npos || line.find(".bounce ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".bounce")));
        ml_vectors[name].bounce = stod(clean(line.substr(line.find("->") + 2)));
    }
    else if (line.find(".collision_floor->") != string::npos || line.find(".collision_floor ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".collision_floor")));
        ml_vectors[name].floor_limit = stod(clean(line.substr(line.find("->") + 2)));
    }
    else if (line.find(".collision_roof->") != string::npos || line.find(".collision_roof ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".collision_roof")));
        ml_vectors[name].roof_limit = stod(clean(line.substr(line.find("->") + 2)));
    }
    else if (line.find(".collision_wall->") != string::npos || line.find(".collision_wall ->") != string::npos) {
        string name = clean(line.substr(0, line.find(".collision_wall")));
        string vals = clean(line.substr(line.find("->") + 2)); size_t c = vals.find(',');
        ml_vectors[name].wall_left = stod(vals.substr(0, c)); ml_vectors[name].wall_right = stod(vals.substr(c + 1));
    }
    else if (line.find(".apply_impulse(") != string::npos) {
        string name = clean(line.substr(0, line.find(".apply_impulse")));
        string vals = clean(line.substr(line.find("([") + 2, line.find("])"))); size_t c = vals.find(',');
        ml_vectors[name].vx += stod(vals.substr(0, c)) / ml_vectors[name].mass;
        ml_vectors[name].vy += stod(vals.substr(c + 1)) / ml_vectors[name].mass;
    }
    else if (line.find("physics.update(") == 0) {
        string name = clean(line.substr(15, line.find(")") - 15));
        if (ml_vectors.find(name) != ml_vectors.end()) {
            Vector2D& v = ml_vectors[name]; v.vy += global_gravity * 0.1;
            v.x += v.vx * 0.1; v.y += v.vy * 0.1;
            if (v.y < v.floor_limit) { v.y = v.floor_limit; v.vy = -v.vy * v.bounce; cout << RED << "[Hit Floor!] " << RESET; }
            if (v.y > v.roof_limit) { v.y = v.roof_limit; v.vy = -v.vy * v.bounce; cout << RED << "[Hit Roof!] " << RESET; }
            if (v.x < v.wall_left) { v.x = v.wall_left; v.vx = -v.vx * v.bounce; cout << RED << "[Hit Left Wall!] " << RESET; }
            if (v.x > v.wall_right) { v.x = v.wall_right; v.vx = -v.vx * v.bounce; cout << RED << "[Hit Right Wall!] " << RESET; }
            cout << "'" << name << "' pos: [" << v.x << ", " << v.y << "]" << endl;
        }
    }
    return 0;
}

void run_internal_test() {
    cout << "\033[33m--- [ЗАПУСК ОФИЦИАЛЬНОГО ВНУТРЕННЕГО ТЕСТА ВСЕХ КОМАНД БЕТЫ] ---\033[0m\n" << endl;
    execute_ml_core("STXRT block_name = [\"Obsidian\"];");
    execute_ml_core("Xbool disable_ui = [\"False\"];");
    execute_ml_core("color.pnt.console(color: green; \"[OK] Комментарии и цветной вывод активны!\");");
    variables["k"] = 5; variables["kk"] = 10;
    execute_ml_core("num standard_calc = k + kk * 5;");
    execute_ml_core("num error_calc = error k + kk * 5;");
    execute_ml_core("Func calc_speed(impulse, mass):");
    execute_ml_core("{");
    execute_ml_core("   1[num speed = impulse / mass;]");
    execute_ml_core("   2[return speed;]");
    execute_ml_core("};");
    cout << "\nВызов Func calc_speed(60, 2):" << endl;
    double speed_res = execute_ml_core("calc_speed(60, 2);");
    variables["test_speed"] = speed_res;
    execute_ml_core("pnt.console(test_speed);");
    
    // ИСПРАВЛЕННЫЙ ТЕСТ ЦИКЛА: ДОБАВИЛ ДИАПАЗОН [1,4], ЧТОБЫ STOI РАБОТАЛ ШТАТНО
    cout << "\nЗапуск цикла for (block_x = [1,4]):" << endl;
    execute_ml_core("for (block_x = [1,4]) { pnt.console(\"Постройка 2D блока по оси X\"); }");
    
    cout << "\nТест динамического расширения матрицы:" << endl;
    execute_ml_core("matrix world_blocks =;");
    execute_ml_core("world_blocks.push -> [1.0];");
    execute_ml_core("world_blocks.push -> [2.0];");
    execute_ml_core("pnt.console(world_blocks.size);");
    cout << "\nЗапуск физической симуляции слайма:" << endl;
    execute_ml_core("physics.gravity = -10;");
    execute_ml_core("vector slime = [0.0, 4.0];");
    execute_ml_core("slime.bounce -> [0.7];");
    execute_ml_core("slime.collision_floor -> [0.0];");
    execute_ml_core("slime.apply_impulse([0.0, 30.0]);");
    for(int i=0; i<4; i++) {
        execute_ml_core("physics.update(slime);");
        execute_ml_core("physics.delay();");
    }
}

int main() {
    srand(time(0));
    cout << "\033[32m=== Интерпретатор ML-core v0.1 Beta: Архитектор NextGenMax ===\033[0m\n" << endl;
    string filename = "main.mlc";
    ifstream file(filename);
    if (!file.is_open()) {
        run_internal_test();
        return 0;
    }
    string line;
    while (getline(file, line)) {
        execute_ml_core(line);
    }
    file.close();
    cout << "\n\033[32m=== Выполнение скрипта " << filename << " успешно завершено! ===\033[0m" << endl;
    return 0;
}
