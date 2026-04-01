#include "oberon.h"
#include "ui.h"
#include "modules.h"

char current_target[128] = "";
char current_ip[64] = "";

void run_scan(int start, int end, int mode_udp) {
    ui_log("Scanning engine started...");
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
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
        usleep(10000); // Чтобы не забить UI мгновенно
    }
}

int main() {
    init_networking();
    ui_init();
    ui_log("Oberon-EXT Framework Online.");

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
                ui_log("DNS Resolved successfully.");
            } else {
                ui_log("Error: Could not resolve host!");
            }
        } else if (strncmp(cmd, "scan ", 5) == 0) {
            if (strlen(current_ip) < 7) { ui_log("Set target first!"); continue; }
            int start, end;
            sscanf(cmd + 5, "%d %d", &start, &end);
            run_scan(start, end, 0);
        } else if (strcmp(cmd, "exit") == 0) break;
    }

    ui_cleanup();
    cleanup_networking();
    return 0;
}
