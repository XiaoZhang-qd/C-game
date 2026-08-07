#include "network.h"

#include "../common/socket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../common/protocol.h"


pxpt_socket client_socket;



int network_connect(
const char* ip,
int port
)
{


client_socket=
pxpt_tcp_connect(
ip,
port
);



if(
client_socket==
PXPT_INVALID_SOCKET
)
{

return -1;

}



return 0;


}



void network_send_player(
float x,
float y
)
{


PXPT_Packet packet;



packet.header.type=
PKT_PLAYER_UPDATE;



packet.header.size=
sizeof(PXPT_PlayerState);



PXPT_PlayerState state;



state.x=x;

state.y=y;

state.speed=0;

state.lap=0;



memcpy(
packet.data,
&state,
sizeof(state)
);



pxpt_send(
client_socket,
&packet,
sizeof(packet.header)+
packet.header.size
);



}