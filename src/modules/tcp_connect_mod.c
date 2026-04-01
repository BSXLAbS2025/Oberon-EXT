// src/modules/tcp_connect.c (Версия EXT)
#include "oberon.h"
#include "ui.h"

THREAD_FUNC tcp_connect_mod(void* arg) {
    scan_task_t* task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == INVALID_SOCKET) {
        free(task);
        return NULL;
    }

    // Устанавливаем короткий таймаут для агрессивного сканирования
    #ifdef _WIN32
        DWORD timeout = 1500;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    #else
        struct timeval tv = {1, 500000}; 
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    #endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(task->port);
    addr.sin_addr.s_addr = inet_addr(task->ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        // Логика определения ОС через TTL
        int ttl = 0;
        socklen_t optlen = sizeof(ttl);
        char info[256];
        char* os = "Unknown OS";
        
        if (getsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, &optlen) == 0) {
            if (ttl <= 64) os = "Linux/Unix";
            else if (ttl <= 128) os = "Windows";
        }

        // Попытка взять баннер
        char banner[128] = "No Banner";
        send(sock, "\r\n", 2, 0);
        int b = recv(sock, banner, sizeof(banner) - 1, 0);
        if (b > 0) {
            banner[b] = '\0';
            for(int i = 0; i < b; i++) if(banner[i] < 32) banner[i] = ' ';
        }

        // ВАЖНО: Отправляем данные в UI
        snprintf(info, sizeof(info), "OS: %s | Banner: %s", os, banner);
        ui_add_system_law(task->port, info);
        
        char log_msg[128];
        snprintf(log_msg, sizeof(log_msg), "Found open port: %d", task->port);
        ui_log(log_msg, 1); // 1 - зеленый лог
    }

    close_socket(sock);
    free(task);
    return NULL;
}
