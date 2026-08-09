#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H


#include "../common/socket.h"
#include "../common/protocol.h"

#define CGAME_MAX_SERVER_HISTORY 16
#define CGAME_BROADCAST_QUEUE_SIZE 32

typedef struct
{
    char ip[64];
    int port;
    char name[CGAME_MAX_NAME];
} CGAME_ServerEntry;


int network_connect(
const char* ip,
int port
);



void network_disconnect();



int network_send_login(
const char* name
);

int network_send_login_with_password(
const char* name,
const char* password
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
CGAME_PlayerState* out_players,
int max_players
);



int network_send_bomb_explosion(float x, float y);
int network_send_bomb_placed(int player_id, float x, float y, float timer);
int network_get_bomb_explosions(CGAME_BombExplosion* out, int max);
int network_get_bomb_placed(CGAME_BombPlaced* out, int max);
int network_get_server_config(CGAME_ServerConfig* out);

int network_get_broadcasts(CGAME_Broadcast* out, int max);

int network_save_server_history(const char* file_path);
int network_load_server_history(const char* file_path, CGAME_ServerEntry* out, int max);
int network_add_server_history(const char* file_path, const char* ip, int port, const char* name);
int network_delete_server_history(const char* file_path, int index);


#endif
