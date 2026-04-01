#include "oberon.h"
#include "ui.h"

THREAD_FUNC banner_grab_mod(void* arg) {
    scan_task_t *task = (scan_task_t*)arg;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { free(task); return 0; }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(task->ip);
    server.sin_port = htons(task->port);

    // Тайм-ауты (твоя логика)
#ifdef _WIN32
    DWORD timeout = 2000; 
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        char buffer[256] = {0};
        send(sock, "HEAD / HTTP/1.0\r\n\r\n", 19, 0);
        int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
        
        if (bytes > 0) {
            for(int i=0; i<bytes; i++) if(buffer[i] < 32) buffer[i] = ' ';
            ui_add_law(task->port, buffer);
        } else {
            ui_add_law(task->port, "Open (No banner)");
        }
    }

    close_socket(sock);
    free(task);
    return 0;
}
