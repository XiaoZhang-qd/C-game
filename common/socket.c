#include "socket.h"
#include "platform.h"

#include <string.h>

#if defined(CGAME_WINDOWS)
    #include <ws2tcpip.h>
#elif defined(CGAME_WEB)
    #include <emscripten.h>
    #include <emscripten_socket.h>
#else
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

int cgame_socket_init()
{
#if defined(CGAME_WINDOWS)
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
#elif defined(CGAME_WEB)
    return emscripten_socket_init() ? 0 : -1;
#else
    return 0;
#endif
}

void cgame_socket_cleanup()
{
#if defined(CGAME_WINDOWS)
    WSACleanup();
#endif
}

void cgame_socket_close(cgame_socket s)
{
#if defined(CGAME_WINDOWS)
    closesocket(s);
#elif defined(CGAME_WEB)
    emscripten_socket_close(s);
#else
    close(s);
#endif
}

cgame_socket cgame_tcp_connect(const char* ip, int port)
{
    cgame_socket s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == CGAME_INVALID_SOCKET)
        return CGAME_INVALID_SOCKET;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

#if defined(CGAME_WINDOWS)
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
#elif defined(CGAME_WEB)
    addr.sin_addr.s_addr = inet_addr(ip);
#else
    addr.sin_addr.s_addr = inet_addr(ip);
#endif

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cgame_socket_close(s);
        return CGAME_INVALID_SOCKET;
    }

    return s;
}

int cgame_send(cgame_socket s, const void* data, int size)
{
#if defined(CGAME_WEB)
    return emscripten_socket_send(s, data, size);
#else
    return send(s, data, size, 0);
#endif
}

int cgame_recv(cgame_socket s, void* data, int size)
{
#if defined(CGAME_WEB)
    return emscripten_socket_recv(s, data, size);
#else
    return recv(s, data, size, 0);
#endif
}

int cgame_socket_set_nonblocking(cgame_socket s)
{
#if defined(CGAME_WINDOWS)
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode);
#elif defined(CGAME_WEB)
    return emscripten_socket_set_nonblocking(s, 1);
#else
    int flags = fcntl(s, F_GETFL, 0);
    return fcntl(s, F_SETFL, flags | O_NONBLOCK);
#endif
}
