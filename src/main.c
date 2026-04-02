#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "oberon.h"
#include "ui.h"
#include "modules.h"

char current_target[128] = "";
char current_ip[64] = "";
const char* DB_URL = "https://github.com/BSXLAbS2025/Oberon-EXT-db";

// Универсальный исполнитель системных команд
void sys_exec(const char* command) {
    char buffer[256];
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        ui_log("SYSTEM ERROR: Failed to execute pipe.", 2);
        return;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        ui_log(buffer, 0);
    }
    pclose(pipe);
}

// Обновление всей структуры БД
void db_update() {
    ui_log("Synchronizing with BSXLAbS2025/Oberon-EXT-db...", 1);
    if (access("db", F_OK) == 0) {
        ui_log("Pulling latest modules...", 0);
        sys_exec("cd db && git pull");
    } else {
        ui_log("Cloning remote repository...", 0);
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "git clone %s db", DB_URL);
        sys_exec(cmd);
    }
    ui_log("DB Update Complete.", 2);
}

// УНИВЕРСАЛЬНАЯ КОМАНДА USE
// Теперь можно писать: use aux/mac_check или use exploits/ssh_brute
void run_module(const char* rel_path) {
    if (strlen(current_ip) == 0) {
        ui_log("ERROR: Target IP not set. Use 'target <host>'", 2);
        return;
    }

    char full_path[512];
    // Ищем файл .sh по указанному пути внутри папки db/
    snprintf(full_path, sizeof(full_path), "db/%s.sh", rel_path);

    if (access(full_path, F_OK) == 0) {
        char exec_cmd[1024];
        // Передаем IP цели первым аргументом в скрипт
        snprintf(exec_cmd, sizeof(exec_cmd), "bash %s %s", full_path, current_ip);
        
        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg), "Launching module: %s", rel_path);
        ui_log(log_msg, 3); // Маджента для модулей
        
        sys_exec(exec_cmd);
    } else {
        char err[256];
        snprintf(err, sizeof(err), "Module not found: %s.sh", full_path);
        ui_log(err, 2);
    }
}

void run_scan_engine(int start, int end) {
    if (strlen(current_ip) == 0) {
        ui_log("ERROR: No target set.", 2);
        return;
    }
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        if (!task) continue;
        strncpy(task->ip, current_ip, sizeof(task->ip));
        task->port = p;
        pthread_t thread;
        if (pthread_create(&thread, NULL, (void* (*)(void*))banner_grab_mod, task) == 0) {
            pthread_detach(thread);
        } else {
            free(task);
        }
    }
}

int main() {
    init_networking();
    ui_show_splash();
    ui_init();
    
    ui_log("Oberon-EXT v2.35 'Modular' Engine Online.", 0);
    ui_log("Ready for manual module deployment.", 1);

    char cmd[256];
    while(1) {
        mvwprintw(gui.input_bar, 1, 2, "oberon@ext >           ");
        wmove(gui.input_bar, 1, 15);
        echo(); wgetnstr(gui.input_bar, cmd, 255); noecho();
        wclear(gui.input_bar); box(gui.input_bar, 0, 0); wrefresh(gui.input_bar);

        if (strcmp(cmd, "help") == 0) {
            ui_show_help();
        } else if (strncmp(cmd, "target ", 7) == 0) {
            strncpy(current_target, cmd + 7, sizeof(current_target) - 1);
            char* resolved = resolve_host(current_target);
            if (resolved) {
                strncpy(current_ip, resolved, sizeof(current_ip) - 1);
                ui_set_target(current_target, current_ip);
                ui_log("Target locked.", 0);
            } else {
                ui_log("CRITICAL: Host resolution failed.", 2);
            }
        } else if (strncmp(cmd, "scan ", 5) == 0) {
            int s, e;
            if (sscanf(cmd + 5, "%d %d", &s, &e) == 2) run_scan_engine(s, e);
        } else if (strcmp(cmd, "db update") == 0) {
            db_update();
        } else if (strncmp(cmd, "use ", 4) == 0) {
            run_module(cmd + 4);
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strlen(cmd) > 0) {
            ui_log("Unknown command. Type 'help'.", 1);
        }
    }
    ui_cleanup();
    cleanup_networking();
    return 0;
}
