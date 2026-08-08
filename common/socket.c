#include "socket.h"
#include "platform.h"

#include <string.h>

#if defined(PXPT_WINDOWS)
    #include <ws2tcpip.h>
#elif defined(PXPT_WEB)
    #include <emscripten.h>
    #include <emscripten_socket.h>
#else
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

int pxpt_socket_init()
{
#if defined(PXPT_WINDOWS)
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
#elif defined(PXPT_WEB)
    return emscripten_socket_init() ? 0 : -1;
#else
    return 0;
#endif
}

void pxpt_socket_cleanup()
{
#if defined(PXPT_WINDOWS)
    WSACleanup();
#endif
}

void pxpt_socket_close(pxpt_socket s)
{
#if defined(PXPT_WINDOWS)
    closesocket(s);
#elif defined(PXPT_WEB)
    emscripten_socket_close(s);
#else
    close(s);
#endif
}

pxpt_socket pxpt_tcp_connect(const char* ip, int port)
{
    pxpt_socket s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == PXPT_INVALID_SOCKET)
        return PXPT_INVALID_SOCKET;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

#if defined(PXPT_WINDOWS)
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
#elif defined(PXPT_WEB)
    addr.sin_addr.s_addr = inet_addr(ip);
#else
    addr.sin_addr.s_addr = inet_addr(ip);
#endif

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        pxpt_socket_close(s);
        return PXPT_INVALID_SOCKET;
    }

    return s;
}

int pxpt_send(pxpt_socket s, const void* data, int size)
{
#if defined(PXPT_WEB)
    return emscripten_socket_send(s, data, size);
#else
    return send(s, data, size, 0);
#endif
}

int pxpt_recv(pxpt_socket s, void* data, int size)
{
#if defined(PXPT_WEB)
    return emscripten_socket_recv(s, data, size);
#else
    return recv(s, data, size, 0);
#endif
}

int pxpt_socket_set_nonblocking(pxpt_socket s)
{
#if defined(PXPT_WINDOWS)
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#elif defined(PXPT_WEB)
    return emscripten_socket_set_nonblocking(s, 1);
#else
    int flags = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, flags | O_NONBLOCK);
#endif
}
