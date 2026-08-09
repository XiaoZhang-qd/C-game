#ifndef CGAME_GAME_H
#define CGAME_GAME_H


#include "../common/protocol.h"

#define CGAME_MAX_BOMBS 16
#define CGAME_MAX_HP 100
#define CGAME_RESPAWN_TIME 180

typedef struct {
    float x;
    float y;
    float timer;
    int active;
} Bomb;


typedef struct
{

float x;

float y;

float speed;

char name[CGAME_MAX_NAME];

int boost_timer;

int shield_timer;

int bomb_cooldown;

int skill1_cooldown;

int skill2_cooldown;

int hp;

int max_hp;

int dead;

int respawn_timer;

}PlayerCar;



void game_init();



void game_update();



void game_draw();



PlayerCar* game_get_player();



void game_set_player_name(const char* name);



const char* player_name_get();



void game_update_remote(
CGAME_PlayerState* remote_players,
int count
);



int game_get_bomb_count();

Bomb* game_get_bombs();

void game_apply_bomb_explosion(float x, float y);

void game_apply_server_config(const CGAME_ServerConfig* cfg);

int game_get_max_players();

int game_get_bomb_cooldown_max();

int game_get_skill1_cooldown_max();

int game_get_skill2_cooldown_max();

void game_add_remote_bomb(int player_id, float x, float y, float timer);

void game_set_theme(int theme);
int game_get_theme(void);

#endif
