#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "oberon.h"
#include "ui.h"
#include "modules.h"

char current_target[128] = "";
char current_ip[64] = "";
const char* DB_URL = "https://github.com/BSXLAbS2025/Oberon-EXT-db";
int is_pwn_mode = 0; // Флаг для продвинутого режима

void sys_exec(const char* command) {
    char buffer[256];
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        ui_log("SYSTEM ERROR: Failed to execute command.", 2);
        return;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        ui_log(buffer, 0);
    }
    pclose(pipe);
}

void db_update() {
    ui_log("Connecting to BSXLAbS2025/Oberon-EXT-db...", 1);
    if (access("/usr/bin/git", F_OK) == 0) {
        if (access("db", F_OK) == 0) {
            ui_log("Updating database via git pull...", 0);
            sys_exec("cd db && git pull");
        } else {
            ui_log("Cloning database via git clone...", 0);
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "git clone %s db", DB_URL);
            sys_exec(cmd);
        }
    } else {
        ui_log("Git not found. Using wget...", 4);
        sys_exec("mkdir -p db && wget -qO db/master.zip https://github.com/BSXLAbS2025/Oberon-EXT-db/archive/refs/heads/main.zip");
    }
    ui_log("Database synchronization complete.", 0);
}

void run_exploit_module(const char* module_path) {
    if (strlen(current_ip) == 0) {
        ui_log("ERROR: Set target first!", 2);
        return;
    }
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "db/%s.sh", module_path);
    if (access(full_path, F_OK) == 0) {
        char exec_cmd[1024];
        snprintf(exec_cmd, sizeof(exec_cmd), "bash %s %s", full_path, current_ip);
        ui_log("Executing exploit module...", 3);
        sys_exec(exec_cmd);
    } else {
        ui_log("ERROR: Module not found in db/", 2);
    }
}

void run_scan_engine(int start, int end) {
    if (strlen(current_ip) == 0) {
        ui_log("ERROR: No target set. Use 'target <host>'", 2);
        return;
    }
    char msg[128];
    snprintf(msg, sizeof(msg), "Scanning ports %d-%d...", start, end);
    ui_log(msg, 0);
    for (int p = start; p <= end; p++) {
        scan_task_t *task = malloc(sizeof(scan_task_t));
        if (!task) continue;
        strncpy(task->ip, current_ip, sizeof(task->ip));
        task->port = p;
#ifndef _WIN32
        pthread_t thread;
        if (pthread_create(&thread, NULL, (void* (*)(void*))banner_grab_mod, task) == 0) {
            pthread_detach(thread);
        } else {
            free(task);
        }
#endif
    }
}

// Поиск модулей
void db_search(const char* query) {
    char cmd[512];
    ui_log("Searching for modules...", 1);
    snprintf(cmd, sizeof(cmd), "find db -name '*%s*' | sed 's/^db\\///'", query);
    sys_exec(cmd);
}

int main() {
    init_networking();
    ui_show_splash();
    ui_init();
    
    ui_log("Oberon-EXT v2.33 Professional Engine Online.", 0);
    ui_log("Linked to DB: BSXLAbS2025/Oberon-EXT-db", 1);

    char cmd[256];
    while(1) {
        if (is_pwn_mode) {
            wattron(gui.input_bar, COLOR_PAIR(3) | A_BOLD);
            mvwprintw(gui.input_bar, 1, 2, "root@oberon# ");
            wattroff(gui.input_bar, COLOR_PAIR(3) | A_BOLD);
        } else {
            mvwprintw(gui.input_bar, 1, 2, "oberon@ext > ");
        }
        
        wmove(gui.input_bar, 1, 15);
        echo(); wgetnstr(gui.input_bar, cmd, 255); noecho();
        wclear(gui.input_bar); 
        ui_refresh();

        if (strcmp(cmd, "help") == 0) {
            ui_show_help();
            if (is_pwn_mode) {
                ui_log("--- ADVANCED COMMANDS ---", 3);
                ui_log("banner grab --aggressive  - Aggressive service analysis", 0);
                ui_log("stack-fingerprint        - TCP/IP Stack Analysis (TTL/Window)", 0);
                ui_log("crawl --depth <n>        - Recursive web crawler", 0);
                ui_log("method-check             - HTTP Methods (OPTIONS, TRACE)", 0);
                ui_log("back                     - Drop root privileges", 1);
            }
        } 
        else if (strcmp(cmd, "sudo") == 0) {
            is_pwn_mode = 1;
            ui_log("INTERNAL PRIVILEGE ESCALATION: SUCCESS.", 3);
            ui_refresh();
        }
        else if (strcmp(cmd, "back") == 0) {
            is_pwn_mode = 0;
            ui_log("Privileges dropped.", 1);
            ui_refresh();
        }
        else if (strncmp(cmd, "search ", 7) == 0) {
            db_search(cmd + 7);
        }
        else if (strncmp(cmd, "target ", 7) == 0) {
            strncpy(current_target, cmd + 7, sizeof(current_target) - 1);
            char* resolved = resolve_host(current_target);
            if (resolved) {
                strncpy(current_ip, resolved, sizeof(current_ip) - 1);
                ui_set_target(current_target, current_ip);
                ui_log("Target identification successful.", 0);
            } else {
                ui_log("CRITICAL: Host resolution failed.", 2);
            }
        } 
        else if (strncmp(cmd, "scan ", 5) == 0) {
            int s, e;
            if (sscanf(cmd + 5, "%d %d", &s, &e) == 2) run_scan_engine(s, e);
        } 
        else if (strcmp(cmd, "db status") == 0 || strcmp(cmd, "db update") == 0) {
            db_update();
        } 
        else if (strncmp(cmd, "exploit ", 8) == 0) {
            run_exploit_module(cmd + 8);
        } 
        else if (strncmp(cmd, "use ", 4) == 0) {
            run_exploit_module(cmd + 4);
        } 
        /* РЕАЛИЗАЦИЯ ADVANCED COMMANDS */
        else if (is_pwn_mode && strcmp(cmd, "banner grab --aggressive") == 0) {
            if (strlen(current_ip) == 0) { ui_log("ERROR: No target", 2); continue; }
            ui_log("Launching aggressive fingerprinting...", 3);
            char b_cmd[512];
            snprintf(b_cmd, sizeof(b_cmd), "nmap -sV --version-intensity 9 %s", current_ip);
            sys_exec(b_cmd);
        }
        else if (is_pwn_mode && strcmp(cmd, "stack-fingerprint") == 0) {
            if (strlen(current_ip) == 0) { ui_log("ERROR: No target", 2); continue; }
            ui_log("Analyzing TCP/IP Stack...", 3);
            char s_cmd[512];
            snprintf(s_cmd, sizeof(s_cmd), "ping -c 1 %s | grep 'ttl='", current_ip);
            sys_exec(s_cmd);
            ui_log("Check TTL: 64=Linux, 128=Windows, 255=Cisco/Solaris", 1);
        }
        else if (is_pwn_mode && strncmp(cmd, "crawl ", 6) == 0) {
            int depth = 1;
            sscanf(cmd + 6, "--depth %d", &depth);
            ui_log("Starting web crawler...", 3);
            char c_cmd[512];
            snprintf(c_cmd, sizeof(c_cmd), "wget --spider -r -l %d %s 2>&1 | grep '^--'", depth, current_target);
            sys_exec(c_cmd);
        }
        else if (is_pwn_mode && strcmp(cmd, "method-check") == 0) {
            if (strlen(current_ip) == 0) { ui_log("ERROR: No target", 2); continue; }
            ui_log("Testing HTTP Methods...", 3);
            char m_cmd[512];
            snprintf(m_cmd, sizeof(m_cmd), "curl -v -X OPTIONS http://%s 2>&1 | grep 'Allow:'", current_ip);
            sys_exec(m_cmd);
        }
        /* ЗАПРЕТ ДОСТУПА */
        else if (!is_pwn_mode && (strstr(cmd, "aggressive") || strstr(cmd, "stack") || strstr(cmd, "crawl") || strstr(cmd, "method"))) {
            ui_log("ACCESS DENIED: Advanced Mode required.", 2);
        }
        else if (strcmp(cmd, "exit") == 0) {
            break;
        } else if (strlen(cmd) > 0) {
            ui_log("Unknown command. Type 'help'.", 1);
        }
    }

    ui_cleanup();
    cleanup_networking();
    return 0;
}
