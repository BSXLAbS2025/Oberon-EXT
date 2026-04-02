\#include "oberon.h"
#include "ui.h"
#include "modules.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char current_target[128] = "";
char current_ip[64] = "";
const char* DB_URL = "https://github.com/BSXLAbS2025/Oberon-EXT-db";

// Функция для выполнения системных команд и вывода в лог
void sys_exec(const char* command) {
    char buffer[128];
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        ui_log("SYSTEM ERROR: Failed to execute command.", 2);
        return;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        ui_log(buffer, 0);
    }
    pclose(pipe);
}

// Реальное обновление базы данных из твоего репозитория
void db_update() {
    ui_log("Connecting to BSXLAbS2025/Oberon-EXT-db...", 1);
    
    // Проверяем наличие git, если нет - используем wget
    if (access("/usr/bin/git", F_OK) == 0) {
        if (access("db", F_OK) == 0) {
            ui_log("Updating existing database via git pull...", 0);
            sys_exec("cd db && git pull");
        } else {
            ui_log("Cloning remote database via git clone...", 0);
            char cmd[256];
            sprintf(cmd, "git clone %s db", DB_URL);
            sys_exec(cmd);
        }
    } else {
        ui_log("Git not found. Attempting raw download via wget...", 4);
        sys_exec("mkdir -p db && wget -qO db/master.zip https://github.com/BSXLAbS2025/Oberon-EXT-db/archive/refs/heads/main.zip");
    }
    ui_log("Database synchronization complete.", 0);
}

// Поиск и запуск эксплоита из скачанной папки db/
void run_exploit_module(const char* module_path) {
    char full_path[256];
    sprintf(full_path, "db/exploits/%s.sh", module_path); // Ищем shell-скрипт эксплоита

    if (access(full_path, F_OK) == 0) {
        char exec_cmd[512];
        sprintf(exec_cmd, "bash %s %s", full_path, current_ip);
        ui_log("Executing exploit module...", 3);
        sys_exec(exec_cmd);
    } else {
        ui_log("ERROR: Module not found in db/exploits/", 2);
    }
}

void run_scan_engine(int start, int end) {
    if (strlen(current_ip) == 0) {
        ui_log("ERROR: No target set. Use 'target <host>' first.", 2);
        return;
    }

    char msg[128];
    sprintf(msg, "Launching thread pool for ports %d-%d", start, end);
    ui_log(msg, 0);

    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        strcpy(task->ip, current_ip);
        task->port = p;

        pthread_t thread;
        pthread_create(&thread, NULL, (void*)banner_grab_mod, task);
        pthread_detach(thread);
    }
}

int main() {
    init_networking();
    ui_show_splash();
    ui_init();
    
    ui_log("Oberon-EXT v2.33 Professional Engine Online.", 0);
    ui_log("System architecture: Multi-threaded / POSIX Sockets.", 0);
    ui_log("Linked to DB: BSXLAbS2025/Oberon-EXT-db", 1);

    char cmd[256];
    while(1) {
        mvwprintw(gui.input_bar, 1, 2, "oberon@ext >           ");
        wmove(gui.input_bar, 1, 15);
        echo(); wgetnstr(gui.input_bar, cmd, 255); noecho();
        wclear(gui.input_bar); box(gui.input_bar, 0, 0); wrefresh(gui.input_bar);

        if (strcmp(cmd, "help") == 0) {
            ui_show_help();
        } else if (strncmp(cmd, "target ", 7) == 0) {
            strcpy(current_target, cmd + 7);
            char* resolved = resolve_host(current_target);
            if (resolved) {
                strcpy(current_ip, resolved);
                ui_set_target(current_target, current_ip);
                ui_log("Target identification successful.", 0);
            } else {
                ui_log("CRITICAL: Host resolution failed.", 2);
            }
        } else if (strncmp(cmd, "scan ", 5) == 0) {
            int s, e;
            if (sscanf(cmd + 5, "%d %d", &s, &e) == 2) run_scan_engine(s, e);
        } else if (strcmp(cmd, "db update") == 0) {
            db_update();
        } else if (strncmp(cmd, "use ", 4) == 0) {
            // Команда в стиле MSF: use exploits/linux/ssh_brute
            run_exploit_module(cmd + 4);
        } else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strlen(cmd) > 0) {
            ui_log("Unknown command. Type 'help' for options.", 1);
        }
    }

    ui_cleanup();
    cleanup_networking();
    return 0;
}
