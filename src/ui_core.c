#include "ui.h"
#include "oberon.h"
#include <unistd.h>

// Для работы мьютексов на Linux/Mac нужен этот инклюд
#ifndef _WIN32
    #include <pthread.h>
#endif

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
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

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
    box(gui.header, 0, 0); mvwprintw(gui.header, 0, 2, " OBERON-EXT v2.33 ");
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
    if (line > getmaxy(gui.sidebar) - 3) line = 1; // Защита от переполнения
    wattron(gui.sidebar, COLOR_PAIR(2));
    mvwprintw(gui.sidebar, line++, 1, "LAW: PORT %d", port);
    mvwprintw(gui.sidebar, line++, 2, " > %.35s", desc);
    wattroff(gui.sidebar, COLOR_PAIR(2));
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    wrefresh(gui.sidebar);
    UNLOCK_UI;
}

// ИСПРАВЛЕНО: Теперь принимает 2 аргумента, как в ui.h
void ui_log(const char* msg, int type) {
    LOCK_UI;
    if (type == 1) wattron(gui.main_log, COLOR_PAIR(2)); // Green
    if (type == 2) wattron(gui.main_log, COLOR_PAIR(3)); // Red
    
    wprintw(gui.main_log, " [*] %s\n", msg);
    
    wattroff(gui.main_log, COLOR_PAIR(2) | COLOR_PAIR(3));
    box(gui.main_log, 0, 0);
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_cleanup() { 
    endwin(); 
#ifdef _WIN32
    DeleteCriticalSection(&ui_lock);
#endif
}
