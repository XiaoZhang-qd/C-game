#ifndef PXPT_SOCKET_H
#define PXPT_SOCKET_H


#include "platform.h"


#if defined(PXPT_WINDOWS)


#define CloseWindow CloseWindowWin32
#define Rectangle RectangleWin32
#define ShowCursor ShowCursorWin32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#undef CloseWindow
#undef Rectangle
#undef ShowCursor


typedef SOCKET pxpt_socket;


#define PXPT_INVALID_SOCKET INVALID_SOCKET



#else


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef int pxpt_socket;


#define PXPT_INVALID_SOCKET (-1)



#endif




int pxpt_socket_init();


void pxpt_socket_cleanup();


void pxpt_socket_close(
    pxpt_socket s
);



pxpt_socket pxpt_tcp_connect(
    const char* ip,
    int port
);



int pxpt_send(
    pxpt_socket s,
    const void* data,
    int size
);



int pxpt_recv(
    pxpt_socket s,
    void* data,
    int size
);



int pxpt_socket_set_nonblocking(
    pxpt_socket s
);



#endif