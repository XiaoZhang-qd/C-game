#ifndef CGAME_PACKET_H
#define CGAME_PACKET_H


#include "protocol.h"


#define MAX_PACKET 1024



typedef struct
{

    CGAME_PacketHeader header;


    unsigned char data[MAX_PACKET];


} CGAME_Packet;



#endif