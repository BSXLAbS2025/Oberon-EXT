#ifndef UI_EXT_H
#define UI_EXT_H

#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

#include <pthread.h>

typedef struct {
    WINDOW *header;     // Статус: Target, Threads, Time
    WINDOW *main_log;   // Бегущие логи
    WINDOW *sidebar;    // System Laws (Open Ports)
    WINDOW *input_bar;  // Командная строка oberon-ext >
} ui_t;

void ui_init();
void ui_cleanup();
void ui_log(const char* msg, int type); // 0-info, 1-warn, 2-crit
void ui_add_system_law(int port, const char* info);
void ui_set_target(const char* target);
void ui_refresh_all();
char* ui_get_input();

// Мьютекс для синхронизации печати из разных потоков (сканеров)
extern pthread_mutex_t ui_mutex;

#endif
