#include "raylib.h"


#include "gui.h"

#include "game.h"

#include "network.h"


#include "../common/socket.h"


#include <stdlib.h>

#include <string.h>



int main()
{


pxpt_socket_init();



InitWindow(
800,
600,
"PXPT Racer"
);



SetTargetFPS(60);



GuiSetStyle(
DEFAULT,
TEXT_SIZE,
20
);



ServerInput input;



memset(
&input,
0,
sizeof(input)
);



strcpy(
input.ip,
"127.0.0.1"
);



strcpy(
input.port,
"25565"
);



GUI_State state =
GUI_MENU;



game_init();




while(
!WindowShouldClose()
)
{


BeginDrawing();



ClearBackground(
BLACK
);



switch(state)
{


case GUI_MENU:

{

gui_draw_menu(
&input
);



if(
input.connect
)
{

int port=
atoi(
input.port
);



if(
network_connect(
input.ip,
port
)==0
)
{

state=
GUI_GAME;

}



input.connect=0;


}



}

break;




case GUI_GAME:

{

game_update();


game_draw();



PlayerCar* p=
game_get_player();



network_send_player(
p->x,
p->y
);



}

break;



}



EndDrawing();


}



CloseWindow();



pxpt_socket_cleanup();



return 0;

}