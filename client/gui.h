#ifndef CGAME_GUI_H
#define CGAME_GUI_H


#include "raylib.h"
#include "raygui.h"

#include "../common/protocol.h"


typedef enum
{

    GUI_MENU,

    GUI_GAME

} GUI_State;



typedef struct
{

    char name[CGAME_MAX_NAME];

    char ip[64];

    char port[16];


    int name_edit;

    int ip_edit;

    int port_edit;


    int connect;


    int create_server;


    char error_msg[128];

} ServerInput;



void gui_draw_menu(
    ServerInput* input
);



#endif