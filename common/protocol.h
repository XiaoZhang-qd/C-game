#ifndef PXPT_PROTOCOL_H
#define PXPT_PROTOCOL_H


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


} PXPT_PacketHeader;




typedef struct
{

int id;


char name[PXPT_MAX_NAME];


float x;

float y;


float speed;


int lap;

int hp;

int dead;

int boost_timer;

int shield_timer;


} PXPT_PlayerState;




typedef struct
{

char name[PXPT_MAX_NAME];


} PXPT_Login;



typedef struct
{

float x;
float y;

} PXPT_BombExplosion;



typedef struct
{

int player_id;
float x;
float y;
float timer;

} PXPT_BombPlaced;



typedef struct
{

int max_players;

int bomb_cooldown;

int skill1_cooldown;

int skill2_cooldown;

} PXPT_ServerConfig;



typedef struct
{

int count;


PXPT_PlayerState players[PXPT_MAX_PLAYERS];


} PXPT_PlayerList;





typedef struct
{

PXPT_PacketHeader header;


unsigned char data[512];


} PXPT_Packet;



#endif