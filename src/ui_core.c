#include "ui.h"
#include <string.h>
#include <unistd.h>

ui_t gui;
pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;

void ui_init() {
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Цветовые схемы
    init_pair(1, COLOR_CYAN, COLOR_BLACK);   // Header
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Success/Ports
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Critical/Alert
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Prompt

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Создаем окна (y, x, start_y, start_x)
    gui.header = newwin(3, max_x, 0, 0);
    gui.main_log = newwin(max_y - 7, max_x - 30, 3, 0);
    gui.sidebar = newwin(max_y - 7, 30, 3, max_x - 30);
    gui.input_bar = newwin(3, max_x, max_y - 3, 0);

    scrollok(gui.main_log, TRUE);
    ui_refresh_all();
}

void ui_set_target(const char* target) {
    pthread_mutex_lock(&ui_mutex);
    wclear(gui.header);
    box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, " TARGET LOCKED: [%s] ", target);
    
    // Анимация сканирования (Target Locked Effect)
    const char* anim = "|/-\\";
    for(int i = 0; i < 10; i++) {
        mvwprintw(gui.header, 1, strlen(target) + 18, "[%c]", anim[i % 4]);
        wrefresh(gui.header);
        usleep(50000);
    }
    mvwprintw(gui.header, 1, strlen(target) + 18, "[ READY ]");
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
    pthread_mutex_unlock(&ui_mutex);
}

void ui_add_system_law(int port, const char* info) {
    pthread_mutex_lock(&ui_mutex);
    static int line = 1;
    wattron(gui.sidebar, COLOR_PAIR(2));
    mvwprintw(gui.sidebar, line++, 1, "LAW #%d: PORT %d", line, port);
    mvwprintw(gui.sidebar, line++, 1, "  -> %s", info);
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    wattroff(gui.sidebar, COLOR_PAIR(2));
    wrefresh(gui.sidebar);
    pthread_mutex_unlock(&ui_mutex);
}

void ui_log(const char* msg, int type) {
    pthread_mutex_lock(&ui_mutex);
    if (type == 1) wattron(gui.main_log, COLOR_PAIR(2));
    if (type == 2) wattron(gui.main_log, COLOR_PAIR(3));
    
    wprintw(gui.main_log, " [*] %s\n", msg);
    
    wattroff(gui.main_log, COLOR_PAIR(2) | COLOR_PAIR(3));
    box(gui.main_log, 0, 0);
    wrefresh(gui.main_log);
    pthread_mutex_unlock(&ui_mutex);
}

void ui_refresh_all() {
    box(gui.header, 0, 0);
    mvwprintw(gui.header, 0, 2, " OBERON-EXT FRAMEWORK v5.0 ");
    box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    box(gui.input_bar, 0, 0);
    
    wrefresh(gui.header);
    wrefresh(gui.main_log);
    wrefresh(gui.sidebar);
    wrefresh(gui.input_bar);
}

void ui_cleanup() {
    endwin();
}
