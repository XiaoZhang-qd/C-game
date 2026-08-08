#include "raylib.h"

#include "gui.h"

#include "game.h"

#include "network.h"

#include "../common/socket.h"

#include "../common/hide_terminal.h"

#include <stdlib.h>

#include <string.h>



int main()
{


cgame_socket_init();



InitWindow(
800,
600,
"CGame Racer"
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
input.name,
"Player"
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

input.error_msg[0] = '\0';

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

int ret =
network_send_login(
input.name
);

if(
ret == 0
)
{

game_set_player_name(
input.name
);

state =
GUI_GAME;

}
else
{

if(
ret == LOGIN_NAME_TAKEN
)
{

strcpy(
input.error_msg,
"Name already taken! Please choose another."
);

}
else if(
ret == LOGIN_NAME_EMPTY
)
{

strcpy(
input.error_msg,
"Name cannot be empty!"
);

}
else
{

strcpy(
input.error_msg,
"Login failed. Try again."
);

}


network_disconnect();

}

}
else
{

strcpy(
input.error_msg,
"Cannot connect to server."
);

}


input.connect = 0;

}


}
break;



case GUI_GAME:
{

game_update();



PlayerCar* p=
game_get_player();



int send_ret = network_send_player(
p->x,
p->y,
p->name,
p->hp,
p->dead,
p->boost_timer,
p->shield_timer
);

if(send_ret < 0)
{
    network_disconnect();
    state = GUI_MENU;
    input.error_msg[0] = '\0';
    strcpy(input.error_msg, "Disconnected from server.");
    break;
}



CGAME_PlayerState remote_updates[CGAME_MAX_PLAYERS];
int received =
network_recv_player(
remote_updates,
CGAME_MAX_PLAYERS
);

if(received == -2)
{
    network_disconnect();
    state = GUI_MENU;
    input.error_msg[0] = '\0';
    strcpy(input.error_msg, "Disconnected from server.");
    break;
}

if(
received > 0
)
{

game_update_remote(
remote_updates,
received
);

}



CGAME_BombExplosion explosions[32];
int exp_count =
network_get_bomb_explosions(
explosions,
32
);

for(
int i=0;
i<exp_count;
i++
)
{

game_apply_bomb_explosion(
explosions[i].x,
explosions[i].y
);

}



CGAME_BombPlaced placed_bombs[32];
int placed_count =
network_get_bomb_placed(
placed_bombs,
32
);

for(
int i=0;
i<placed_count;
i++
)
{

game_add_remote_bomb(
placed_bombs[i].player_id,
placed_bombs[i].x,
placed_bombs[i].y,
placed_bombs[i].timer
);

}



CGAME_ServerConfig svcfg;
if (network_get_server_config(&svcfg))
{
    game_apply_server_config(&svcfg);
}



game_draw();



}
break;



}


EndDrawing();

}



CloseWindow();

network_disconnect();
cgame_socket_cleanup();

return 0;

}
