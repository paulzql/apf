#include <iostream>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#ifdef WINCE
   #pragma comment(lib, "WS2.LIB")
#else
   #pragma comment(lib, "WS2_32.LIB")
#endif
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define SOCKET unsigned int
#endif

using namespace std;

/******************Color Print***********************************/
// windows
#ifdef _WIN32
#define GREEN_PRINT(fmt, ...) {SetConsoleTextAttribute(\
    GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);\
    printf(fmt, __VA_ARGS__);SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);}
#define RED_PRINT(fmt, ...) {SetConsoleTextAttribute(\
    GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);\
    printf(fmt, __VA_ARGS__);SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);}
#define BLUE_PRINT(fmt, ...) {SetConsoleTextAttribute(\
    GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);\
    printf(fmt, __VA_ARGS__);SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);}
// linux
#else
#define GREEN_PRINT(fmt, args...) {printf("\033[32m"fmt"\033[0m", ##args);}
#define RED_PRINT(fmt, args...) {printf("\033[31m"fmt"\033[0m", ##args);}
#define BLUE_PRINT(fmt, args...) {printf("\033[34m"fmt"\033[0m", ##args);}
#endif // win32
/***************************************************************/

#define DEFAULT_PORT 4096

#define MAX_BUF 2048

enum LogType {
    LOG_INFO,
    LOG_WARN,
    LOG_DEBUG,
    LOG_ERROR
};

LogType GetLogType(const char* log) {
    LogType type = LOG_INFO;
    const char* ptype = log;
    ptype = strchr(ptype, ' ');
    if (ptype) {
        ptype = strchr(ptype, ' ');
    }
    if (ptype) {
        ptype++;
        if (strncmp(ptype, "Warn", 4) == 0) {
            type = LOG_WARN;
        } else if (strncmp(ptype, "Debug", 5) == 0) {
            type = LOG_DEBUG;
        } else if (strncmp(ptype, "Error", 5) == 0) {
            type = LOG_ERROR;
        }
    }

    return type;
}

int main()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = ntohs(DEFAULT_PORT);

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested =MAKEWORD( 1, 1 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we couldn't find a useable */
        /* winsock.dll. */
        RED_PRINT("%s", "Error: cannot init socket\n");
        return 0;
    }
#endif
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sock) {
        RED_PRINT("%s", "Error: cannot init socket\n");
        return 0;
    }
    if (0 != bind(sock, (sockaddr*)&addr, sizeof(addr))) {
        RED_PRINT("Error: cannot bind socket on %d\n", DEFAULT_PORT);
        return 0;
    }

    GREEN_PRINT("Listened and watting for log on port %d ...\n", DEFAULT_PORT);

    sockaddr_in addr_client;
#ifdef WIN32
    int len = sizeof(addr_client);
#else
    unsigned int len = sizeof(addr_client);
#endif
    char buf[MAX_BUF];
    while (recvfrom(sock, buf, MAX_BUF, 0, (sockaddr*)&addr_client, &len) >= 0) {
        switch(GetLogType(buf)) {
        case LOG_INFO:
            GREEN_PRINT("%s", buf);
            break;
        case LOG_DEBUG:
            BLUE_PRINT("%s", buf);
            break;
        case LOG_WARN:
            printf(buf);
            break;
        case LOG_ERROR:
            RED_PRINT("%s", buf);
            break;
        default:
            break;
        }
    }

#ifdef WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0;
}


