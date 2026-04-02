#ifndef UI_H
#define UI_H

#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

typedef struct {
    WINDOW *header;
    WINDOW *main_log;
    WINDOW *sidebar;
    WINDOW *input_bar;
} ui_t;

extern ui_t gui;

void ui_init();
void ui_show_splash();
void ui_refresh();
void ui_set_target(const char* target, const char* ip);
void ui_add_law(int port, const char* desc);
void ui_log(const char* msg, int type); // 0-info, 1-warn, 2-crit, 3-exploit
void ui_show_help(); // Новая команда помощи
void ui_cleanup();

#endif
