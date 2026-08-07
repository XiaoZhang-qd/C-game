#ifndef PXPT_GAME_H
#define PXPT_GAME_H


typedef struct
{

float x;

float y;

float speed;


}PlayerCar;



void game_init();


void game_update();


void game_draw();


PlayerCar* game_get_player();



#endif