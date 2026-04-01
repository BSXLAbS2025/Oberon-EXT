#include "oberon.h"
#include "ui.h"

void* tcp_connect_mod(void* arg) {
    scan_task_t* task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock == INVALID_SOCKET) {
        free(task);
        return NULL;
    }

    // Твои тайм-ауты
#ifdef _WIN32
    DWORD timeout = 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
#endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(task->port);
    addr.sin_addr.s_addr = inet_addr(task->ip);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        // Определение ОС через TTL (твоя фишка)
        int ttl = 0;
        socklen_t optlen = sizeof(ttl);
        char* os = "Unknown";
        if (getsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&ttl, &optlen) == 0) {
            if (ttl <= 64) os = "Linux/Unix";
            else if (ttl <= 128) os = "Windows";
        }

        // Берем баннер (твоя логика из v4.0)
        char banner[128] = "none";
        send(sock, "\r\n", 2, 0); 
        int b = recv(sock, banner, sizeof(banner) - 1, 0);
        if (b > 0) {
            for(int i = 0; i < b; i++) if(banner[i] < 32) banner[i] = ' ';
            banner[b] = '\0';
        }

        // Формируем описание для System Laws
        char law_desc[256];
        snprintf(law_desc, sizeof(law_desc), "OS: %s | TTL: %d | Banner: %s", os, ttl, banner);
        
        ui_add_law(task->port, law_desc);
        
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "TCP Port %d is OPEN", task->port);
        ui_log(log_msg,1);
    }

    close_socket(sock);
    free(task);
    return NULL;
}
