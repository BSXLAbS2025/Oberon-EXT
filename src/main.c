#include "oberon.h"
#include "ui.h"
#include "modules.h"

char current_target[128] = "";
char current_ip[64] = "";

void run_scan(int start, int end, int mode_udp) {
    ui_log("Scanning engine started...", 0); // Добавили 0
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        if(!task) continue;
        strcpy(task->ip, current_ip);
        task->port = p;

        THREAD_HANDLE thread;
#ifdef _WIN32
        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(mode_udp ? udp_raw_mod : banner_grab_mod), task, 0, NULL);
        if (thread) CloseHandle(thread);
#else
        pthread_create(&thread, NULL, (mode_udp ? udp_raw_mod : banner_grab_mod), task);
        pthread_detach(thread);
#endif
        usleep(10000); 
    }
}

int main() {
    init_networking();
    ui_init();
    ui_log("Oberon-EXT Framework Online.", 0); // Добавили 0

    char cmd[256];
    while(1) {
        // Теперь gui.input_bar будет виден благодаря extern в ui.h
        mvwprintw(gui.input_bar, 1, 2, "oberon-ext >           ");
        wmove(gui.input_bar, 1, 15);
        echo(); wgetnstr(gui.input_bar, cmd, 255); noecho();
        wclear(gui.input_bar); box(gui.input_bar, 0, 0); wrefresh(gui.input_bar);

        if (strncmp(cmd, "target ", 7) == 0) {
            strcpy(current_target, cmd + 7);
            char* resolved = resolve_host(current_target);
            if (resolved) {
                strcpy(current_ip, resolved);
                ui_set_target(current_target, current_ip); // Тут 2 аргумента, как и в ui_core.c
                ui_log("DNS Resolved successfully.", 0);
            } else {
                ui_log("Error: Could not resolve host!", 2);
            }
        } else if (strncmp(cmd, "scan ", 5) == 0) {
            if (strlen(current_ip) < 7) { 
                ui_log("Set target first!", 2); 
                continue; 
            }
            int start, end;
            if(sscanf(cmd + 5, "%d %d", &start, &end) == 2) {
                run_scan(start, end, 0);
            }
        } else if (strcmp(cmd, "exit") == 0) break;
    }

    ui_cleanup();
    cleanup_networking();
    return 0;
}
