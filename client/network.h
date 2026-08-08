#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H


#include "../common/socket.h"
#include "../common/protocol.h"


int network_connect(
const char* ip,
int port
);



void network_disconnect();



int network_send_login(
const char* name
);



int network_is_logged_in();



int network_send_player(
float x,
float y,
const char* name,
int hp,
int dead,
int boost_timer,
int shield_timer
);



int network_recv_player(
PXPT_PlayerState* out_players,
int max_players
);



int network_send_bomb_explosion(float x, float y);
int network_send_bomb_placed(int player_id, float x, float y, float timer);
int network_get_bomb_explosions(PXPT_BombExplosion* out, int max);
int network_get_bomb_placed(PXPT_BombPlaced* out, int max);
int network_get_server_config(PXPT_ServerConfig* out);



#endif
