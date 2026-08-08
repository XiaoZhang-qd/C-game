#include "network.h"

#include "../common/socket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../common/protocol.h"


pxpt_socket client_socket;

static int login_ok = 0;

#define MAX_BOMB_QUEUE 32
static PXPT_BombExplosion bomb_queue[MAX_BOMB_QUEUE];
static int bomb_queue_count = 0;

#define MAX_BOMB_PLACED 32
static PXPT_BombPlaced bomb_placed_queue[MAX_BOMB_PLACED];
static int bomb_placed_count = 0;

static PXPT_ServerConfig server_config;
static int config_received = 0;



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



login_ok = 0;
bomb_queue_count = 0;
bomb_placed_count = 0;
config_received = 0;

return 0;


}



void network_disconnect()
{
if(client_socket != PXPT_INVALID_SOCKET)
{
    pxpt_socket_close(client_socket);
    client_socket = PXPT_INVALID_SOCKET;
}
login_ok = 0;
bomb_queue_count = 0;
bomb_placed_count = 0;
config_received = 0;
}



int network_send_login(
const char* name
)
{


PXPT_Packet packet;
memset(
&packet,
0,
sizeof(packet)
);

packet.header.type =
PKT_LOGIN;

packet.header.size =
sizeof(PXPT_Login);

PXPT_Login login;
memset(
&login,
0,
sizeof(login)
);
strncpy(
login.name,
name,
PXPT_MAX_NAME - 1
);
login.name[PXPT_MAX_NAME - 1] = '\0';

memcpy(
packet.data,
&login,
sizeof(login)
);


int r =
pxpt_send(
client_socket,
&packet,
sizeof(packet)
);

if(
r <= 0
)
return -1;


PXPT_Packet resp;
int rr =
pxpt_recv(
client_socket,
&resp,
sizeof(resp)
);

if(
rr <= 0
)
return -1;


if(
resp.header.type
!=
PKT_LOGIN_RESPONSE
)
return -1;


int code;
memcpy(
&code,
resp.data,
sizeof(int)
);

if(
code == LOGIN_OK
)
{

login_ok = 1;

pxpt_socket_set_nonblocking(
    client_socket
);

return 0;

}


return code;

}



int network_is_logged_in()
{

return login_ok;

}



int network_send_player(
float x,
float y,
const char* name,
int hp,
int dead,
int boost_timer,
int shield_timer
)
{


PXPT_Packet packet;
memset(
&packet,
0,
sizeof(packet)
);

packet.header.type=
PKT_PLAYER_UPDATE;

packet.header.size=
sizeof(PXPT_PlayerState);



PXPT_PlayerState state;
memset(
&state,
0,
sizeof(state)
);

state.x=x;

state.y=y;

state.speed=0;

state.lap=0;

state.hp=hp;

state.dead=dead;
state.boost_timer=boost_timer;
state.shield_timer=shield_timer;

if(name)
{
    strncpy(state.name, name, PXPT_MAX_NAME - 1);
    state.name[PXPT_MAX_NAME - 1] = '\0';
}



memcpy(
packet.data,
&state,
sizeof(state)
);



int r = pxpt_send(
client_socket,
&packet,
sizeof(packet)
);

if(r <= 0)
{
    login_ok = 0;
    return -1;
}

return 0;


}



int network_send_bomb_explosion(float x, float y)
{
    PXPT_Packet packet;
    memset(&packet, 0, sizeof(packet));

    packet.header.type = PKT_BOMB_EXPLOSION;
    packet.header.size = sizeof(PXPT_BombExplosion);

    PXPT_BombExplosion exp;
    exp.x = x;
    exp.y = y;
    memcpy(packet.data, &exp, sizeof(exp));

    int r = pxpt_send(client_socket, &packet, sizeof(packet));
    if(r <= 0)
    {
        login_ok = 0;
        return -1;
    }
    return 0;
}



int network_recv_player(
PXPT_PlayerState* out_players,
int max_players
)
{


int received = 0;



while(
received < max_players
)
{

PXPT_Packet packet;

int r =
pxpt_recv(
client_socket,
&packet,
sizeof(packet)
);


if(
r == 0
)
{
    login_ok = 0;
    return -2;
}

if(
r < 0
)
break;



if(
packet.header.type
==
PKT_PLAYER_UPDATE
)
{

if(
packet.header.size
==
sizeof(PXPT_PlayerState)
)
{

memcpy(
&out_players[received],
packet.data,
sizeof(PXPT_PlayerState)
);

received++;

}

}
else if(
packet.header.type
==
PKT_BOMB_EXPLOSION
)
{
    if(
        packet.header.size
        ==
        sizeof(PXPT_BombExplosion)
        && bomb_queue_count < MAX_BOMB_QUEUE
    )
    {
        memcpy(
            &bomb_queue[bomb_queue_count],
            packet.data,
            sizeof(PXPT_BombExplosion)
        );
        bomb_queue_count++;
    }
}
else if(
packet.header.type
==
PKT_SERVER_CONFIG
)
{
    if(
        packet.header.size
        ==
        sizeof(PXPT_ServerConfig)
    )
    {
        memcpy(
            &server_config,
            packet.data,
            sizeof(PXPT_ServerConfig)
        );
        config_received = 1;
    }
}
else if(
packet.header.type
==
PKT_BOMB_PLACED
)
{
    if(
        packet.header.size
        ==
        sizeof(PXPT_BombPlaced)
        && bomb_placed_count < MAX_BOMB_PLACED
    )
    {
        memcpy(
            &bomb_placed_queue[bomb_placed_count],
            packet.data,
            sizeof(PXPT_BombPlaced)
        );
        bomb_placed_count++;
    }
}



}



return received;

}



int network_get_bomb_explosions(PXPT_BombExplosion* out, int max)
{
    int count = 0;
    int to_copy = bomb_queue_count < max ? bomb_queue_count : max;
    for (int i = 0; i < to_copy; i++)
    {
        out[i] = bomb_queue[i];
    }
    bomb_queue_count = 0;
    return to_copy;
}

int network_get_server_config(PXPT_ServerConfig* out)
{
    if (!config_received || !out) return 0;
    *out = server_config;
    return 1;
}

int network_send_bomb_placed(int player_id, float x, float y, float timer)
{
    PXPT_Packet packet;
    memset(&packet, 0, sizeof(packet));

    packet.header.type = PKT_BOMB_PLACED;
    packet.header.size = sizeof(PXPT_BombPlaced);

    PXPT_BombPlaced bp;
    bp.player_id = player_id;
    bp.x = x;
    bp.y = y;
    bp.timer = timer;
    memcpy(packet.data, &bp, sizeof(bp));

    int r = pxpt_send(client_socket, &packet, sizeof(packet));
    if(r <= 0)
    {
        login_ok = 0;
        return -1;
    }
    return 0;
}

int network_get_bomb_placed(PXPT_BombPlaced* out, int max)
{
    int count = 0;
    int to_copy = bomb_placed_count < max ? bomb_placed_count : max;
    for (int i = 0; i < to_copy; i++)
    {
        out[i] = bomb_placed_queue[i];
    }
    bomb_placed_count = 0;
    return to_copy;
}
