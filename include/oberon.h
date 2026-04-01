#ifndef OBERON_H
#define OBERON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #define sleep_ms(ms) Sleep(ms)
    #define close_socket closesocket
    typedef int socklen_t;
    #define THREAD_FUNC DWORD WINAPI
    #define THREAD_HANDLE HANDLE
    #define SOCKET_TYPE SOCKET
#else
    #include <sys/time.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <pthread.h>
    #define sleep_ms(ms) usleep((ms) * 1000)
    #define close_socket close
    typedef int SOCKET;
    #define SOCKET_TYPE SOCKET
    #define INVALID_SOCKET -1
    #define THREAD_FUNC void*
    #define THREAD_HANDLE pthread_t
#endif

typedef struct {
    char ip[64];
    int port;
    int delay;
} scan_task_t;

void init_networking();
void cleanup_networking();
char* resolve_host(char *hostname);

#endif
