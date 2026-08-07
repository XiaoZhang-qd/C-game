#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H


#include "../common/socket.h"


int network_connect(
const char* ip,
int port
);



void network_send_player(
float x,
float y
);



#endif