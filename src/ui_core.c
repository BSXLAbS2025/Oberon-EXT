#include "ui.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32
    #include <pthread.h>
    pthread_mutex_t ui_lock = PTHREAD_MUTEX_INITIALIZER;
#else
    #include <windows.h>
    CRITICAL_SECTION ui_lock;
#endif

ui_t gui;

#ifdef _WIN32
    #define LOCK_UI EnterCriticalSection(&ui_lock)
    #define UNLOCK_UI LeaveCriticalSection(&ui_lock)
#else
    #define LOCK_UI pthread_mutex_lock(&ui_lock)
    #define UNLOCK_UI pthread_mutex_unlock(&ui_lock)
#endif

const char* banners[] = {
    "  ___  ____  _____ ____   ___  _   _ ",
    " / _ \\| __ )| ____|  _ \\ / _ \\| \\ | |",
    "| | | |  _ \\|  _| | |_) | | | |  \\| |",
    "| |_| | |_) | |___|  _ <| |_| | |\\  |",
    " \\___/|____/|_____|_| \\_\\\\___/|_| \\_|"
};

void ui_show_splash() {
    initscr(); 
    start_color(); 
    noecho(); 
    curs_set(0);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    
    clear();
    attron(COLOR_PAIR(2) | A_BOLD);
    for(int i = 0; i < 5; i++) {
        mvprintw(LINES/2 - 3 + i, (COLS - 40)/2, "%s", banners[i]);
    }
    mvprintw(LINES/2 + 3, (COLS - 20)/2, "CYBER-EDITION v2.35");
    refresh();
    sleep(2);
    clear();
}

void ui_init() {
#ifdef _WIN32
    InitializeCriticalSection(&ui_lock);
#endif
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

    int y, x; 
    getmaxyx(stdscr, y, x);

    // Создаем окна с проверкой на минимальный размер
    gui.header = newwin(3, x, 0, 0);
    gui.main_log = newwin(y - 7, x - 45, 3, 0);
    gui.sidebar = newwin(y - 7, 45, 3, x - 45);
    gui.input_bar = newwin(3, x, y - 3, 0);
    
    scrollok(gui.main_log, TRUE);
    ui_refresh();
}

void ui_log(const char* msg, int type) {
    LOCK_UI;
    int color = 2; 
    if (type == 1) color = 4; // Yellow
    if (type == 2) color = 3; // Red
    if (type == 3) color = 5; // Magenta
    
    wattron(gui.main_log, COLOR_PAIR(color));
    wprintw(gui.main_log, " [%s] %s\n", (type == 3 ? "EXE" : "*"), msg);
    wattroff(gui.main_log, COLOR_PAIR(color));
    
    box(gui.main_log, 0, 0);
    mvwprintw(gui.main_log, 0, 2, " CONSOLE LOG ");
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_show_help() {
    ui_log("--- HELP MENU ---", 1);
    ui_log("target <host> | scan <s-e> | use <path>", 0);
    ui_log("db update     | db status  | exit", 0);
}

void ui_add_law(int port, const char* desc) {
    LOCK_UI;
    static int line = 1;
    if (line > getmaxy(gui.sidebar) - 4) { 
        wclear(gui.sidebar); 
        line = 1; 
    }
    
    wattron(gui.sidebar, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(gui.sidebar, line++, 1, "LAW: PORT %d", port);
    wattrset(gui.sidebar, COLOR_PAIR(2));
    mvwprintw(gui.sidebar, line++, 2, " > %.38s", desc);
    line++;
    
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " REPOSITORY ");
    wrefresh(gui.sidebar);
    UNLOCK_UI;
}

void ui_refresh() {
    LOCK_UI;
    box(gui.header, 0, 0); 
    box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0); 
    box(gui.input_bar, 0, 0);
    
    mvwprintw(gui.header, 1, 2, "OBERON-EXT ENGINE READY");
    
    wrefresh(gui.header); 
    wrefresh(gui.main_log);
    wrefresh(gui.sidebar); 
    wrefresh(gui.input_bar);
    UNLOCK_UI;
}

void ui_set_target(const char* target, const char* ip) {
    LOCK_UI;
    wclear(gui.header); 
    box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "TARGET: %s [%s]", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
    UNLOCK_UI;
}

void ui_cleanup() { endwin(); }
