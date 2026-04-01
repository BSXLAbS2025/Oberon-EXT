#include "ui.h"
#include <unistd.h>
#include <string.h>

ui_t gui;

void ui_init() {
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0); // Скрыть курсор

    // Цветовые схемы
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Успех
    init_pair(2, COLOR_CYAN, COLOR_BLACK);   // Инфо
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Критично
    init_pair(4, COLOR_YELLOW, COLOR_BLACK); // Внимание

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Создаем окна: y, x, start_y, start_x
    gui.header = newwin(3, max_x, 0, 0);
    gui.main_log = newwin(max_y - 6, max_x - 30, 3, 0);
    gui.side_laws = newwin(max_y - 6, 30, 3, max_x - 30);
    gui.input = newwin(3, max_x, max_y - 3, 0);

    ui_draw_borders();
}

void ui_draw_borders() {
    box(gui.header, 0, 0);
    box(gui.main_log, 0, 0);
    box(gui.side_laws, 0, 0);
    box(gui.input, 0, 0);
    
    mvwprintw(gui.side_laws, 0, 2, "[ SYSTEM LAWS ]");
    mvwprintw(gui.main_log, 0, 2, "[ SCAN LOG ]");
    
    wrefresh(gui.header);
    wrefresh(gui.main_log);
    wrefresh(gui.side_laws);
    wrefresh(gui.input);
}

void ui_target_lock_anim(const char* target) {
    char symbols[] = "|/-\\";
    for (int i = 0; i < 20; i++) {
        wattron(gui.header, COLOR_PAIR(2) | A_BOLD);
        mvwprintw(gui.header, 1, 2, "LOCKING TARGET: %s [%c]", target, symbols[i % 4]);
        wattroff(gui.header, COLOR_PAIR(2) | A_BOLD);
        wrefresh(gui.header);
        usleep(100000);
    }
    mvwprintw(gui.header, 1, 2, "TARGET LOCKED: %s           ", target);
    wattron(gui.header, COLOR_PAIR(1));
    mvwprintw(gui.header, 1, 40, "[ SECURED ]");
    wattroff(gui.header, COLOR_PAIR(1));
    wrefresh(gui.header);
}

void ui_add_law(int port, const char* proto, const char* info) {
    static int law_row = 1;
    wattron(gui.side_laws, COLOR_PAIR(1));
    mvwprintw(gui.side_laws, law_row++, 2, "-> PORT %d/%s", port, proto);
    mvwprintw(gui.side_laws, law_row++, 4, "{ %s }", info);
    wattroff(gui.side_laws, COLOR_PAIR(1));
    wrefresh(gui.side_laws);
}

void ui_cleanup() {
    endwin();
}
