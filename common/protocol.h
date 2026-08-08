#ifndef CGAME_PROTOCOL_H
#define CGAME_PROTOCOL_H


#include "config.h"



#define PKT_LOGIN          1
#define PKT_PLAYER_UPDATE  2
#define PKT_PLAYER_LIST    3
#define PKT_LOGIN_RESPONSE 4
#define PKT_BOMB_EXPLOSION 5
#define PKT_SERVER_CONFIG  6
#define PKT_BOMB_PLACED    7

#define LOGIN_OK           1
#define LOGIN_NAME_TAKEN   2
#define LOGIN_NAME_EMPTY   3



typedef struct
{

int type;

int size;


} CGAME_PacketHeader;




typedef struct
{

int id;


char name[CGAME_MAX_NAME];


float x;

float y;


float speed;


int lap;

int hp;

int dead;

int boost_timer;

int shield_timer;


} CGAME_PlayerState;




typedef struct
{

char name[CGAME_MAX_NAME];


} CGAME_Login;



typedef struct
{

float x;
float y;

} CGAME_BombExplosion;



typedef struct
{

int player_id;
float x;
float y;
float timer;

} CGAME_BombPlaced;



typedef struct
{

int max_players;

int bomb_cooldown;

int skill1_cooldown;

int skill2_cooldown;

} CGAME_ServerConfig;



typedef struct
{

int count;


CGAME_PlayerState players[CGAME_MAX_PLAYERS];


} CGAME_PlayerList;





typedef struct
{

CGAME_PacketHeader header;


unsigned char data[512];


} CGAME_Packet;



#endif