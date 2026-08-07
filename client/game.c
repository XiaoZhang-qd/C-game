#include "game.h"

#include "raylib.h"



static PlayerCar player;



void game_init()
{

player.x=400;

player.y=300;

player.speed=250;


}



void game_update()
{


float dt=
GetFrameTime();



if(
IsKeyDown(KEY_W)
||
IsKeyDown(KEY_UP)
)
player.y-=player.speed*dt;



if(
IsKeyDown(KEY_S)
||
IsKeyDown(KEY_DOWN)
)
player.y+=player.speed*dt;



if(
IsKeyDown(KEY_A)
||
IsKeyDown(KEY_LEFT)
)
player.x-=player.speed*dt;



if(
IsKeyDown(KEY_D)
||
IsKeyDown(KEY_RIGHT)
)
player.x+=player.speed*dt;


}



void game_draw()
{


DrawCircle(
player.x,
player.y,
25,
RED
);



DrawText(
"PLAYER",
player.x-35,
player.y-50,
20,
WHITE
);


}



PlayerCar* game_get_player()
{

return &player;

}