#include "oberon.h"
#include "ui.h"
#include "modules.h"

char current_target[128] = "";
char current_ip[64] = "";

void run_scan(int start, int end) {
    ui_log("Engaging High-Speed Multi-threading...", 0);
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        strcpy(task->ip, current_ip);
        task->port = p;

        THREAD_HANDLE thread;
#ifdef _WIN32
        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)banner_grab_mod, task, 0, NULL);
        if (thread) CloseHandle(thread);
#else
        pthread_create(&thread, NULL, banner_grab_mod, task);
        pthread_detach(thread);
#endif
        // usleep убран для максимальной скорости 1000+ портов/сек
    }
}

int main() {
    init_networking();
    ui_show_splash(); // Тот самый рандомный баннер
    ui_init();
    
    ui_log("Oberon-EXT v2.33 Online. System Laws Loaded.", 0);

    char cmd[256];
    while(1) {
        mvwprintw(gui.input_bar, 1, 2, "oberon-ext >           ");
        wmove(gui.input_bar, 1, 15);
        echo(); wgetnstr(gui.input_bar, cmd, 255); noecho();
        wclear(gui.input_bar); box(gui.input_bar, 0, 0); wrefresh(gui.input_bar);

        if (strncmp(cmd, "target ", 7) == 0) {
            strcpy(current_target, cmd + 7);
            char* resolved = resolve_host(current_target);
            if (resolved) {
                strcpy(current_ip, resolved);
                ui_set_target(current_target, current_ip);
                ui_log("Target identification complete.", 0);
            } else {
                ui_log("CRITICAL: Target unreachable!", 2);
            }
        } else if (strncmp(cmd, "scan ", 5) == 0) {
            int s, e;
            if(sscanf(cmd + 5, "%d %d", &s, &e) == 2) run_scan(s, e);
        } else if (strcmp(cmd, "exit") == 0) break;
    }

    ui_cleanup();
    cleanup_networking();
    return 0;
}
