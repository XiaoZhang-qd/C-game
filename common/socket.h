#ifndef CGAME_SOCKET_H
#define CGAME_SOCKET_H


#include "platform.h"


#if defined(CGAME_WINDOWS)


#define CloseWindow CloseWindowWin32
#define Rectangle RectangleWin32
#define ShowCursor ShowCursorWin32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#undef CloseWindow
#undef Rectangle
#undef ShowCursor


typedef SOCKET cgame_socket;


#define CGAME_INVALID_SOCKET INVALID_SOCKET



#else


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef int cgame_socket;


#define CGAME_INVALID_SOCKET (-1)



#endif




int cgame_socket_init();


void cgame_socket_cleanup();


void cgame_socket_close(
    cgame_socket s
);



cgame_socket cgame_tcp_connect(
    const char* ip,
    int port
);



int cgame_send(
    cgame_socket s,
    const void* data,
    int size
);



int cgame_recv(
    cgame_socket s,
    void* data,
    int size
);



int cgame_socket_set_nonblocking(
    cgame_socket s
);



#endif