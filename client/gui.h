#ifndef PXPT_GUI_H
#define PXPT_GUI_H


#include "raylib.h"
#include "raygui.h"


typedef enum
{

    GUI_MENU,

    GUI_GAME

} GUI_State;



typedef struct
{

    char ip[64];

    char port[16];


    int ip_edit;

    int port_edit;


    int connect;


    int create_server;


} ServerInput;



void gui_draw_menu(
    ServerInput* input
);



#endif