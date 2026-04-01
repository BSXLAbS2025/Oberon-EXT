#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif
#include "ui.h"
#include <pthread.h>
#include <unistd.h>

typedef struct {
    WINDOW *header;
    WINDOW *log;
    WINDOW *sidebar;
    WINDOW *input;
} ui_ctx_t;

ui_ctx_t gui;
pthread_mutex_t ui_lock = PTHREAD_MUTEX_INITIALIZER;

void ui_init() {
    initscr(); start_color(); cbreak(); noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);   // Header
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // Laws
    init_pair(3, COLOR_YELLOW, COLOR_BLACK); // Prompt
    
    int my, mx; getmaxyx(stdscr, my, mx);
    gui.header = newwin(3, mx, 0, 0);
    gui.log    = newwin(my-6, mx-35, 3, 0);
    gui.sidebar = newwin(my-6, 35, 3, mx-35);
    gui.input  = newwin(3, mx, my-3, 0);
    scrollok(gui.log, TRUE);
    ui_refresh();
}

void ui_refresh() {
    pthread_mutex_lock(&ui_lock);
    box(gui.header, 0, 0); mvwprintw(gui.header, 0, 2, " OBERON-EXT v5.0 ");
    box(gui.log, 0, 0);
    box(gui.sidebar, 0, 0); mvwprintw(gui.sidebar, 0, 2, " SYSTEM LAWS ");
    box(gui.input, 0, 0);
    wrefresh(gui.header); wrefresh(gui.log); wrefresh(gui.sidebar); wrefresh(gui.input);
    pthread_mutex_unlock(&ui_lock);
}

void ui_target_anim(const char* target) {
    pthread_mutex_lock(&ui_lock);
    for(int i=0; i<10; i++) {
        mvwprintw(gui.header, 1, 2, "LOCKING TARGET: [%s] %c", target, "|/-\\"[i%4]);
        wrefresh(gui.header); usleep(60000);
    }
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "TARGET LOCKED: [%s]          ", target);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
    pthread_mutex_unlock(&ui_lock);
}

void ui_add_law(int port, const char* info) {
    pthread_mutex_lock(&ui_lock);
    static int line = 1;
    wattron(gui.sidebar, COLOR_PAIR(2));
    mvwprintw(gui.sidebar, line++, 1, "-> PORT %-5d | %s", port, info);
    wattroff(gui.sidebar, COLOR_PAIR(2));
    if(line >= getmaxy(gui.sidebar)-2) line = 1;
    wrefresh(gui.sidebar);
    pthread_mutex_unlock(&ui_lock);
}

void ui_log(const char* msg) {
    pthread_mutex_lock(&ui_lock);
    wprintw(gui.log, " [*] %s\n", msg);
    box(gui.log, 0, 0);
    wrefresh(gui.log);
    pthread_mutex_unlock(&ui_lock);
}
