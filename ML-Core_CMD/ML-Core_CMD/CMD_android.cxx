#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

// 🔑 СВЯЗЬ ВЕКА: Выходим из папки в корень и подключаем твой НАСТОЯЩИЙ синтаксис!
#include "../ML-core.cxx" 

using namespace std;

void show_loading_animation() {
    cout << "\033[33m\n[Система]: Запуск скачивания пакетов ядра...\033[0m" << endl;
    string bar = "░░░░░░░░░░░░░░░░░░░░";
    for (int i = 0; i <= 100; i += 10) {
        system("clear");
        cout << "\033[36m============================================\033[0m" << endl;
        cout << "\033[32m             ANDROID CMD SYSTEM             \033[0m" << endl;
        cout << "\033[36m============================================\033[0m\n" << endl;
        cout << "Скачивание компонентов ML-core Engine: [ " << i << "% ]" << endl;
        if (i / 5 > 0 && i / 5 <= bar.length()) {
            for (int j = 0; j < i / 5; j++) bar[j] = 'X';
        }
        cout << "\033[32m[ " << bar << " ]\033[0m\n" << endl;
        cout << "\033[33mПожалуйста, подождите, идёт развёртывание пакетов...\033[0m" << endl;
        this_thread::sleep_for(chrono::milliseconds(1200)); 
    }
    cout << "\033[32m\n[УСПЕХ]: Все компоненты успешно развернуты!\033[0m" << endl;
    this_thread::sleep_for(chrono::seconds(2));
}

int main() {
    system("clear");
    cout << "\033[36m============================================\033[0m" << endl;
    cout << "\033[32m             ANDROID CMD SYSTEM             \033[0m" << endl;
    cout << "\033[36m============================================\033[0m" << endl;
    cout << "Введите команду скачивания для развёртывания языка.\n" << endl;

    string command;
    while (true) {
        cout << "\033[31mandroid@cmd:~# \033[0m";
        getline(cin, command);
        if (command == "download ML-core;") {
            show_loading_animation();
            break;
        } else {
            cout << "\033[31m[Ошибка]: Введите строго: download ML-core;\033[0m" << endl;
        }
    }

    // 🔥 Запуск интерактивного CMD-режима, который зашит в твоём главном ML-core.cxx
    // (Когда мы добавим туда цикл run_cmd_loop)
    cout << "\033[32m[Система]: Перенаправление потока в главное ядро...\033[0m" << endl;

    return 0;
}
