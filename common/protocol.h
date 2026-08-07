#ifndef PXPT_PROTOCOL_H
#define PXPT_PROTOCOL_H


#include "config.h"



#define PKT_LOGIN          1
#define PKT_PLAYER_UPDATE  2
#define PKT_PLAYER_LIST    3



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


} PXPT_PlayerState;




typedef struct
{

char name[PXPT_MAX_NAME];


} PXPT_Login;




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