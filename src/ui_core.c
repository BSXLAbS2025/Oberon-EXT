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

// Универсальная функция переноса текста для любого окна
void wprint_wrapped(WINDOW *win, const char *prefix, const char *text, int color_pair) {
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    int width = max_x - 4; // Отступы для рамок
    
    wattron(win, COLOR_PAIR(color_pair));
    
    // Печатаем префикс
    wprintw(win, " %s", prefix);
    
    int prefix_len = strlen(prefix) + 2;
    int current_pos = 0;
    int text_len = strlen(text);
    int first_line_width = width - prefix_len;

    while (current_pos < text_len) {
        int chunk = (current_pos == 0) ? first_line_width : width;
        if (chunk <= 0) chunk = 1; // Защита от слишком узких окон

        if (current_pos + chunk > text_len) chunk = text_len - current_pos;

        // Если это не первая строка, делаем отступ, чтобы текст был под текстом, а не под префиксом
        if (current_pos > 0) {
            wprintw(win, "\n   "); // 3 пробела для выравнивания
        }

        waddnstr(win, text + current_pos, chunk);
        current_pos += chunk;
    }
    wprintw(win, "\n");
    wattroff(win, COLOR_PAIR(color_pair));
    
    box(win, 0, 0); // Перерисовываем рамку, так как прокрутка её стирает
    wrefresh(win);
}

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
    // Для сайдбара скролл не включаем, там ручная очистка
    ui_refresh();
}

void ui_log(const char* msg, int type) {
    LOCK_UI;
    int color = 2; 
    const char* prefix = "[*]";
    
    if (type == 1) color = 4; // Warn
    if (type == 2) color = 3; // Crit
    if (type == 3) { color = 5; prefix = "[EXPLOIT]"; }
    
    wprint_wrapped(gui.main_log, prefix, msg, color);
    
    // Чтобы рамка лога не "уезжала" при скролле
    box(gui.main_log, 0, 0);
    mvwprintw(gui.main_log, 0, 2, " CONSOLE LOG ");
    wrefresh(gui.main_log);
    UNLOCK_UI;
}

void ui_show_help() {
    ui_log("COMMANDS: help, target <host>, scan <start> <end>, use <module>, db update, db status, exit", 1);
}

void ui_add_law(int port, const char* desc) {
    LOCK_UI;
    static int current_line = 1;
    int max_y, max_x;
    getmaxyx(gui.sidebar, max_y, max_x);
    
    if (current_line > max_y - 5) {
        wclear(gui.sidebar);
        current_line = 1;
    }

    wattron(gui.sidebar, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(gui.sidebar, current_line++, 1, "LAW: PORT %d", port);
    wattrset(gui.sidebar, COLOR_PAIR(2));
    
    // Перенос для сайдбара (без скролла)
    int start_y = current_line;
    int width = max_x - 4;
    int len = strlen(desc);
    int pos = 0;
    while (pos < len && current_line < max_y - 1) {
        mvwaddnstr(gui.sidebar, current_line++, 2, desc + pos, width);
        pos += width;
    }
    
    current_line++; 
    box(gui.sidebar, 0, 0);
    mvwprintw(gui.sidebar, 0, 2, " REPOSITORY ");
    wrefresh(gui.sidebar);
    UNLOCK_UI;
}

void ui_refresh() {
    box(gui.header, 0, 0); 
    box(gui.main_log, 0, 0);
    box(gui.sidebar, 0, 0); 
    box(gui.input_bar, 0, 0);
    wrefresh(gui.header); 
    wrefresh(gui.main_log);
    wrefresh(gui.sidebar); 
    wrefresh(gui.input_bar);
}

void ui_set_target(const char* target, const char* ip) {
    wclear(gui.header); 
    box(gui.header, 0, 0);
    wattron(gui.header, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(gui.header, 1, 2, "OBERON-EXT >> TARGET: %s [%s]", target, ip);
    wattroff(gui.header, COLOR_PAIR(1) | A_BOLD);
    wrefresh(gui.header);
}

void ui_cleanup() { endwin(); }
