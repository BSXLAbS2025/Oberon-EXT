#include "ui.h"
#include <unistd.h>

ui_t gui;
#ifdef _WIN32
    CRITICAL_SECTION ui_lock;
    #define LOCK_UI EnterCriticalSection(&ui_lock)
    #define UNLOCK_UI LeaveCriticalSection(&ui_lock)
#else
    pthread_mutex_t ui_lock = PTHREAD_MUTEX_INITIALIZER;
    #define LOCK_UI pthread_mutex_lock(&ui_lock)
    #define UNLOCK_UI pthread_mutex_unlock(&ui_lock)
#endif

void ui_init() {
#ifdef _WIN32
    InitializeCriticalSection(&ui_lock);
#endif
    initscr(); start_color(); cbreak(); noecho(); keypad(stdscr, TRUE);
    init_pair(1, COLOR_CYAN, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    int y, x; getmaxyx(stdscr, y, x);
    gui.header = newwin(3, x, 0, 0);
    gui.main_log = newwin(y - 7, x - 40, 3, 0);
    gui.sidebar = newwin(y - 7, 40, 3, x - 40);
    gui.input_bar = newwin(3, x, y - 3, 0);
    scrollok(gui.main_log, TRUE);
    ui_refresh();
}

void ui_refresh() {
    LOCK_UI;
    box(gui.header, 0, 0); mvwprintw(gui.header, 0, 2, " OBERON-EXT v5.0 ");
    box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0); mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    box(gui.input_bar, 0, 0);
    wrefresh(gui.header); wrefresh(gui.main_log);
    wrefresh(gui.sidebar); wrefresh(gui.input_bar);
    UNLOCK_UI;
}

void ui_set_target(const char* target, const char* ip) {
    LOCK_UI;
    wclear(gui.header); box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "TARGET LOCKED: %s (%s)", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
    UNLOCK_UI;
}

void ui_add_law(int port, const char* desc) {
    LOCK_UI;
    static int line = 1;
    wattron(gui.sidebar, COLOR_PAIR(2));
    mvwprintw(gui.sidebar, line++, 1, "LAW: PORT %d", port);
    mvwprintw(gui.sidebar, line++, 2, " > %s", desc);
    wattroff(gui.sidebar, COLOR_PAIR(2));
    wrefresh(gui.sidebar);
    UNLOCK_UI;
}

void ui_log(const char* msg) {
    LOCK_UI;
    wprintw(gui.main_log, " [*] %s\n", msg);
    box(gui.main_log, 0, 0);
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_cleanup() { endwin(); }
