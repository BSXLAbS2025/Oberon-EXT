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
#define LOCK_UI pthread_mutex_lock(&ui_lock)
#define UNLOCK_UI pthread_mutex_unlock(&ui_lock)

const char* banners[] = {
    "  ___  ____  _____ ____   ___  _   _ \n / _ \\| __ )| ____|  _ \\ / _ \\| \\ | |\n| | | |  _ \\|  _| | |_) | | | |  \\| |\n| |_| | |_) | |___|  _ <| |_| | |\\  |\n \\___/|____/|_____|_| \\_\\\\___/|_| \\_|\n      CYBER-EDITION v2.33",
    "      [ SYSTEM READY ]\n   [ DATABASE ONLINE ]\n   [ EXPLOITS LOADED ]"
};

void ui_show_splash() {
    initscr(); start_color(); noecho(); curs_set(0);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(LINES/2 - 2, 0, "%s", banners[0]);
    refresh();
    sleep(2);
    clear();
}

void ui_init() {
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); 

    int y, x; getmaxyx(stdscr, y, x);
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
    if (type == 1) color = 4; // Warn/Help
    if (type == 2) color = 3; // Crit
    if (type == 3) color = 5; // Exploit
    
    wattron(gui.main_log, COLOR_PAIR(color));
    wprintw(gui.main_log, " [%s] %s\n", (type == 3 ? "EXPLOIT" : "*"), msg);
    wattroff(gui.main_log, COLOR_PAIR(color));
    
    box(gui.main_log, 0, 0);
    mvwprintw(gui.main_log, 0, 2, " CONSOLE LOG ");
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_show_help() {
    ui_log("--- AVAILABLE COMMANDS ---", 1);
    ui_log("help           - Show this menu", 0);
    ui_log("target <host>  - Resolve and set target", 0);
    ui_log("scan <s-e>     - Multi-threaded port scan", 0);
    ui_log("use <path>     - Run module (e.g. aux/mac)", 3);
    ui_log("db update      - Sync repository", 1);
    ui_log("exit           - Terminate session", 2);
}

void ui_add_law(int port, const char* desc) {
    LOCK_UI;
    static int line = 1;
    if (line > getmaxy(gui.sidebar) - 4) { wclear(gui.sidebar); line = 1; }
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
    box(gui.header, 0, 0); box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0); box(gui.input_bar, 0, 0);
    wrefresh(gui.header); wrefresh(gui.main_log);
    wrefresh(gui.sidebar); wrefresh(gui.input_bar);
}

void ui_set_target(const char* target, const char* ip) {
    wclear(gui.header); box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "OBERON-EXT >> TARGET: %s [%s]", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
}

void ui_cleanup() { endwin(); }
