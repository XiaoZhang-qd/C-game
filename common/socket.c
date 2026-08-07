#include "socket.h"


#include <string.h>


#ifdef PXPT_WINDOWS


#include <ws2tcpip.h>


#else


#include <arpa/inet.h>
#include <unistd.h>


#endif





int pxpt_socket_init()
{


#ifdef PXPT_WINDOWS


WSADATA wsa;


return WSAStartup(

MAKEWORD(2,2),

&wsa

);



#else


return 0;



#endif

}




void pxpt_socket_cleanup()
{


#ifdef PXPT_WINDOWS


WSACleanup();



#endif


}





void pxpt_socket_close(
pxpt_socket s
)
{


#ifdef PXPT_WINDOWS


closesocket(s);



#else


close(s);



#endif


}






pxpt_socket pxpt_tcp_connect(
const char* ip,
int port
)
{


pxpt_socket s;


s =
socket(
AF_INET,
SOCK_STREAM,
0
);



if(
s==PXPT_INVALID_SOCKET
)

return PXPT_INVALID_SOCKET;



struct sockaddr_in addr;


memset(
&addr,
0,
sizeof(addr)
);



addr.sin_family =
AF_INET;



addr.sin_port =
htons(port);



#ifdef PXPT_WINDOWS

addr.sin_addr.S_un.S_addr =
inet_addr(ip);


#else

addr.sin_addr.s_addr =
inet_addr(ip);


#endif




if(
connect(
s,
(struct sockaddr*)&addr,
sizeof(addr)
)<0
)
{


pxpt_socket_close(s);


return PXPT_INVALID_SOCKET;


}



return s;


}






int pxpt_send(
pxpt_socket s,
const void* data,
int size
)
{


return send(
s,
data,
size,
0
);


}






int pxpt_recv(
pxpt_socket s,
void* data,
int size
)
{


return recv(
s,
data,
size,
0
);


}