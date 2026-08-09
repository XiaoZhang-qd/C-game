#ifndef CGAME_GUI_H
#define CGAME_GUI_H


#include "raylib.h"
#include "raygui.h"

#include "../common/protocol.h"
#include "network.h"

#ifdef _WIN32
#undef DrawText
#endif


typedef enum
{

    GUI_MENU,

    GUI_GAME,

    GUI_SERVER_SETUP

} GUI_State;



typedef struct
{

    char name[CGAME_MAX_NAME];

    char ip[64];

    char port[16];

    char password[64];


    int name_edit;

    int ip_edit;

    int port_edit;

    int password_edit;


    int connect;


    int create_server;

    int exit_game;

    int quit_app;

    int theme;
    int theme_toggle;

    int selected_history;
    int delete_history;

    int server_edit;

    int server_ip_edit;
    int server_port_edit;
    int server_max_edit;
    int server_password_edit;
    int server_bomb_edit;
    int server_skill1_edit;
    int server_skill2_edit;

    char server_ip[64];
    char server_port[16];
    char server_max_players[8];
    char server_password[64];
    char server_bomb_cd[8];
    char server_skill1_cd[8];
    char server_skill2_cd[8];

    int start_server;
    int back_to_menu;

    char error_msg[128];

    CGAME_ServerEntry history[CGAME_MAX_SERVER_HISTORY];
    int history_count;

    CGAME_Broadcast broadcasts[CGAME_BROADCAST_QUEUE_SIZE];
    int broadcast_count;

} ServerInput;



void gui_draw_menu(
    ServerInput* input
);

void gui_draw_game(
    ServerInput* input
);

void gui_draw_server_setup(
    ServerInput* input
);



#endif
