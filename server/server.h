#ifndef PXPT_SERVER_H
#define PXPT_SERVER_H


#include "../common/socket.h"
#include "../common/protocol.h"
#include "../common/config.h"



typedef struct
{

pxpt_socket tcp;

int connected;

int logged_in;

int id;

char name[PXPT_MAX_NAME];

PXPT_PlayerState state;

}PXPT_Player;



typedef struct
{

pxpt_socket listen_socket;

char ip[64];

int port;

int running;

int max_players;

PXPT_ServerConfig config;

PXPT_Player players[PXPT_MAX_PLAYERS];



}PXPT_Server;



int server_start(
PXPT_Server* server,
const char* ip,
int port,
int max_players
);



void server_update(
PXPT_Server* server
);



void server_stop(
PXPT_Server* server
);



#endif