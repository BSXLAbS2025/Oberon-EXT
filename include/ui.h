#ifndef UI_EXT_H
#define UI_EXT_H

#include <ncurses.h>

// Окна интерфейса
typedef struct {
    WINDOW *header;    // Статус и цель
    WINDOW *main_log;  // Бегущие строки сканирования
    WINDOW *side_laws; // System Laws (найденные порты)
    WINDOW *input;     // Командная строка
} ui_t;

void ui_init();
void ui_draw_borders();
void ui_target_lock_anim(const char* target);
void ui_add_law(int port, const char* proto, const char* info); // Добавить "закон" (порт)
void ui_log(const char* msg, int color_pair);
void ui_cleanup();

#endif
