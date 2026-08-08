#ifndef CGAME_SERVER_H
#define CGAME_SERVER_H


#include "../common/socket.h"
#include "../common/protocol.h"
#include "../common/config.h"



typedef struct
{

cgame_socket tcp;

int connected;

int logged_in;

int id;

char name[CGAME_MAX_NAME];

CGAME_PlayerState state;

}CGAME_Player;



typedef struct
{

cgame_socket listen_socket;

char ip[64];

int port;

int running;

int max_players;

CGAME_ServerConfig config;

CGAME_Player players[CGAME_MAX_PLAYERS];



}CGAME_Server;



int server_start(
CGAME_Server* server,
const char* ip,
int port,
int max_players
);



void server_update(
CGAME_Server* server
);



void server_stop(
CGAME_Server* server
);



#endif