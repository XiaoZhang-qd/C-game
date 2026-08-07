#ifndef PXPT_PACKET_H
#define PXPT_PACKET_H


#include "protocol.h"


#define MAX_PACKET 1024



typedef struct
{

    PXPT_Header header;


    unsigned char data[MAX_PACKET];


} PXPT_Packet;



#endif