#include "oberon.h"
#include "ui.h"

THREAD_FUNC udp_raw_mod(void* arg) {
    scan_task_t *task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) { free(task); return 0; }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    
#ifdef _WIN32
    DWORD timeout = 800;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {0, 800000};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif

    server.sin_family = AF_INET;
    server.sin_port = htons(task->port);
    server.sin_addr.s_addr = inet_addr(task->ip);

    // Твоя тактика: шлем пустой пакет и ждем ответа
    sendto(sock, "", 0, 0, (struct sockaddr *)&server, sizeof(server));
    char b;
    if (recvfrom(sock, &b, 1, 0, NULL, NULL) >= 0) {
        ui_add_law(task->port, "UDP OPEN/FILTERED");
        
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "UDP Port %d activity detected", task->port);
        ui_log(log_msg);
    }
    
    close_socket(sock);
    free(task);
    return 0;
}
