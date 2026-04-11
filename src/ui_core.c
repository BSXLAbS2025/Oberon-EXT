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

extern int is_pwn_mode; 
ui_t gui;

#define LOCK_UI pthread_mutex_lock(&ui_lock)
#define UNLOCK_UI pthread_mutex_unlock(&ui_lock)

const char* banners[] = {
    "  ___  ____  _____ ____   ___  _   _ \n / _ \\| __ )| ____|  _ \\ / _ \\| \\ | |\n| | | |  _ \\|  _| | |_) | | | |  \\| |\n| |_| | |_) | |___|  _ <| |_| | |\\  |\n \\___/|____/|_____|_| \\_\\\\___/|_| \\_|\n      CYBER-EDITION v2.33",
    "      [ SYSTEM READY ]\n   [ DATABASE ONLINE ]\n   [ EXPLOITS LOADED ]"
};

void ui_show_splash() {
    initscr(); start_color(); noecho(); curs_set(0);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(LINES/2 - 2, 0, "%s", banners[0]);
    refresh();
    sleep(2);
    clear();
}

void ui_init() {
    int y, x; getmaxyx(stdscr, y, x);
    gui.header = newwin(3, x, 0, 0);
    gui.main_log = newwin(y - 7, x - 45, 3, 0);
    gui.sidebar = newwin(y - 7, 45, 3, x - 45);
    gui.input_bar = newwin(3, x, y - 3, 0);
    scrollok(gui.main_log, TRUE);
    ui_refresh();
}

void ui_refresh() {
    int frame_color = is_pwn_mode ? 3 : 1;
    wattron(gui.header, COLOR_PAIR(frame_color)); box(gui.header, 0, 0); wattroff(gui.header, COLOR_PAIR(frame_color));
    wattron(gui.main_log, COLOR_PAIR(frame_color)); box(gui.main_log, 0, 0); wattroff(gui.main_log, COLOR_PAIR(frame_color));
    wattron(gui.sidebar, COLOR_PAIR(frame_color)); box(gui.sidebar, 0, 0); wattroff(gui.sidebar, COLOR_PAIR(frame_color));
    wattron(gui.input_bar, COLOR_PAIR(frame_color)); box(gui.input_bar, 0, 0); wattroff(gui.input_bar, COLOR_PAIR(frame_color));
    wrefresh(gui.header); wrefresh(gui.main_log); wrefresh(gui.sidebar); wrefresh(gui.input_bar);
}

void ui_log(const char* msg, int type) {
    LOCK_UI;
    int color = 2; 
    if (type == 1) color = 4; // Warning
    if (type == 2) color = 3; // Critical
    if (type == 3) color = 5; // Exploit/Sudo Logic

    wattron(gui.main_log, COLOR_PAIR(color));
    wprintw(gui.main_log, " [%s] %s\n", (type == 3 ? "EXPLOIT" : "*"), msg);
    wattroff(gui.main_log, COLOR_PAIR(color));
    int frame_color = is_pwn_mode ? 3 : 1;
    wattron(gui.main_log, COLOR_PAIR(frame_color)); box(gui.main_log, 0, 0); wattroff(gui.main_log, COLOR_PAIR(frame_color));
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_show_help() {
    ui_log("--- CORE COMMANDS ---", 1);
    ui_log("help            - Documentation", 0);
    ui_log("target <host>   - Target acquisition", 0);
    ui_log("scan <s-e>      - Threaded reconnaissance", 0);
    ui_log("exploit <path>  - Module execution", 3);
    ui_log("search <query>  - DB lookup", 1);
    ui_log("exit            - Terminate", 2);
    // sudo скрыт
}

void ui_set_target(const char* target, const char* ip) {
    wclear(gui.header); 
    int frame_color = is_pwn_mode ? 3 : 1;
    wattron(gui.header, COLOR_PAIR(frame_color)); box(gui.header, 0, 0); wattroff(gui.header, COLOR_PAIR(frame_color));
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "OBERON-EXT >> TARGET: %s [%s]", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
}

void ui_cleanup() { endwin(); }
