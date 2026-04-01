#ifndef UI_H
#define UI_H

#include <ncurses.h>

typedef struct {
    WINDOW *header;
    WINDOW *log;
    WINDOW *sidebar;
    WINDOW *input;
} ui_t;

void ui_init();
void ui_update_status(const char* target, int threads);
void ui_add_port(int port, const char* service, const char* os);
void ui_log(const char* message);
void ui_cleanup();

#endif
