#include "oberon.h"
#include "ui.h"

void shell_loop() {
    char input[256];
    while(1) {
        pthread_mutex_lock(&ui_mutex);
        wclear(gui.input_bar);
        box(gui.input_bar, 0, 0);
        wattron(gui.input_bar, COLOR_PAIR(4) | A_BOLD);
        mvwprintw(gui.input_bar, 1, 2, "oberon-ext > ");
        wattroff(gui.input_bar, COLOR_PAIR(4) | A_BOLD);
        wrefresh(gui.input_bar);
        pthread_mutex_unlock(&ui_mutex);

        echo();
        wgetnstr(gui.input_bar, input, 255);
        noecho();

        if (strcmp(input, "exit") == 0) break;
        
        if (strncmp(input, "target ", 7) == 0) {
            char *host = input + 7;
            ui_set_target(host);
            ui_log("Target updated...", 0);
        } else if (strcmp(input, "scan") == 0) {
            ui_log("Initiating multi-threaded scan...", 1);
            // Здесь вызывается твоя функция старта из main.c v4.0
            // Только вместо printf внутри модулей теперь вызываем ui_add_system_law
        } else {
            ui_log("Unknown command. Use: target, scan, exit", 2);
        }
    }
}

int main() {
    init_networking();
    ui_init();
    
    ui_log("Welcome to Oberon-EXT", 1);
    ui_log("Cross-platform recon framework loaded.", 0);
    
    shell_loop();

    ui_cleanup();
    cleanup_networking();
    return 0;
}
