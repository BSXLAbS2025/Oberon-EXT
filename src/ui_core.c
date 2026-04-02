#include "ui.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
    #include <pthread.h>
    pthread_mutex_t ui_lock = PTHREAD_MUTEX_INITIALIZER;
#else
    CRITICAL_SECTION ui_lock;
#endif

ui_t gui;
#define LOCK_UI #ifdef _WIN32 EnterCriticalSection(&ui_lock) #else pthread_mutex_lock(&ui_lock) #endif
#define UNLOCK_UI #ifdef _WIN32 LeaveCriticalSection(&ui_lock) #else pthread_mutex_unlock(&ui_lock) #endif

const char* banners[] = {
    "  ___  ____  _____ ____   ___  _   _ \n / _ \\| __ )| ____|  _ \\ / _ \\| \\ | |\n| | | |  _ \\|  _| | |_) | | | |  \\| |\n| |_| | |_) | |___|  _ <| |_| | |\\  |\n \\___/|____/|_____|_| \\_\\\\___/|_| \\_|\n      EXTENDED EDITION v2.33",
    "      .---.        .-----------.\n     /     \\  __  /    ------    \\\n    / /     \\(  )/    --------    \\\n   //////   ' \\/ `   ---OBERON---  \\\n  //// / // :    : ---NETWORK EX--  \\\n // /   /  /`    '\\-----------------  \\",
    "   _  _  _  _  _  _  _  _  _  \n  / \\/ \\/ \\/ \\/ \\/ \\/ \\/ \\/ \\ \n ( O | B | E | R | O | N | - | E | X )\n  \\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/ "
};

void ui_show_splash() {
    initscr(); start_color(); noecho(); curs_set(0);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    
    attron(COLOR_PAIR(2) | A_BOLD);
    srand(time(NULL));
    mvprintw(LINES/2 - 3, 0, "%s", banners[rand() % 3]);
    refresh();
    attroff(COLOR_PAIR(2) | A_BOLD);
    
    sleep(2); // Ждем 2 секунды
    clear();
}

void ui_init() {
#ifdef _WIN32
    InitializeCriticalSection(&ui_lock);
#endif
    // Цвета: 2-зеленый (основной), 3-красный (крит), 4-желтый (варн)
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    int y, x; getmaxyx(stdscr, y, x);
    gui.header = newwin(3, x, 0, 0);
    gui.main_log = newwin(y - 7, x - 45, 3, 0); // Чуть шире лог
    gui.sidebar = newwin(y - 7, 45, 3, x - 45); // Расширили сайдбар
    gui.input_bar = newwin(3, x, y - 3, 0);
    
    scrollok(gui.main_log, TRUE);
    ui_refresh();
}

void ui_log(const char* msg, int type) {
    LOCK_UI;
    int color = 2; // По умолчанию всё зеленое
    if (type == 1) color = 4; // Yellow
    if (type == 2) color = 3; // Red
    
    wattron(gui.main_log, COLOR_PAIR(color));
    wprintw(gui.main_log, " [*] %s\n", msg);
    wattroff(gui.main_log, COLOR_PAIR(color));
    
    box(gui.main_log, 0, 0);
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_add_law(int port, const char* desc) {
    LOCK_UI;
    static int line = 1;
    if (line > getmaxy(gui.sidebar) - 4) { wclear(gui.sidebar); line = 1; }
    
    wattron(gui.sidebar, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(gui.sidebar, line++, 1, "LAW: PORT %-5d", port);
    wattrset(gui.sidebar, COLOR_PAIR(2));
    
    // Чтобы токены не обрезались, выводим только первые 35 символов
    mvwprintw(gui.sidebar, line++, 2, " > %.38s", desc);
    
    line++; // Пропуск строки между записями
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    wrefresh(gui.sidebar);
    UNLOCK_UI;
}

void ui_refresh() {
    LOCK_UI;
    wbkgd(gui.main_log, COLOR_PAIR(2)); // Весь текст в логе теперь зеленый по дефолту
    box(gui.header, 0, 0); 
    box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0);
    box(gui.input_bar, 0, 0);
    wrefresh(gui.header); wrefresh(gui.main_log);
    wrefresh(gui.sidebar); wrefresh(gui.input_bar);
    UNLOCK_UI;
}

void ui_set_target(const char* target, const char* ip) {
    LOCK_UI;
    wclear(gui.header); box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "TARGET LOCKED: %s [%s]", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
    UNLOCK_UI;
}

void ui_cleanup() { endwin(); }
