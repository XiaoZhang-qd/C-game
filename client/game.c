#include "game.h"

#include "raylib.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


static PlayerCar player;

static CGAME_PlayerState remote_players[CGAME_MAX_PLAYERS];
static int remote_count = 0;

static float last_update_time[CGAME_MAX_PLAYERS];

static Bomb bombs[CGAME_MAX_BOMBS];
static int bomb_count = 0;

#define MAX_REMOTE_BOMBS 16
static CGAME_BombPlaced remote_bombs[MAX_REMOTE_BOMBS];
static int remote_bomb_count = 0;

static CGAME_ServerConfig server_cfg;
static int config_received = 0;

static int controls_visible = 1;

static int btn_up_pressed = 0;
static int btn_down_pressed = 0;
static int btn_left_pressed = 0;
static int btn_right_pressed = 0;
static int btn_bomb_pressed = 0;
static int btn_skill1_pressed = 0;
static int btn_skill2_pressed = 0;

static int btn_bomb_prev = 0;
static int btn_skill1_prev = 0;
static int btn_skill2_prev = 0;

static Rectangle btn_up_rect = { 60, 410, 70, 60 };
static Rectangle btn_down_rect = { 60, 520, 70, 60 };
static Rectangle btn_left_rect = { 10, 470, 70, 60 };
static Rectangle btn_right_rect = { 110, 470, 70, 60 };
static Rectangle btn_bomb_rect = { 695, 495, 90, 90 };
static Rectangle btn_skill1_rect = { 590, 435, 70, 70 };
static Rectangle btn_skill2_rect = { 680, 375, 70, 70 };
static Rectangle btn_toggle_rect = { 745, 10, 45, 45 };

#define WORLD_WIDTH 2000
#define WORLD_HEIGHT 1500
#define PLAYER_RADIUS 25

static float camera_x = 400.0f;
static float camera_y = 300.0f;


static int point_in_rect(float px, float py, Rectangle r)
{
    return (px >= r.x && px <= r.x + r.width &&
            py >= r.y && py <= r.y + r.height);
}


static float dist_sq(float x1, float y1, float x2, float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}


static void update_touch_buttons()
{
    btn_up_pressed = 0;
    btn_down_pressed = 0;
    btn_left_pressed = 0;
    btn_right_pressed = 0;
    btn_bomb_pressed = 0;
    btn_skill1_pressed = 0;
    btn_skill2_pressed = 0;

    int touch_count = GetTouchPointCount();

    for (int t = 0; t < touch_count; t++)
    {
        Vector2 tp = GetTouchPosition(t);

        if (point_in_rect(tp.x, tp.y, btn_up_rect)) btn_up_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_down_rect)) btn_down_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_left_rect)) btn_left_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_right_rect)) btn_right_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_bomb_rect)) btn_bomb_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_skill1_rect)) btn_skill1_pressed = 1;
        if (point_in_rect(tp.x, tp.y, btn_skill2_rect)) btn_skill2_pressed = 1;
    }

    if (touch_count == 0)
    {
        Vector2 mp = GetMousePosition();
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (point_in_rect(mp.x, mp.y, btn_up_rect)) btn_up_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_down_rect)) btn_down_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_left_rect)) btn_left_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_right_rect)) btn_right_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_bomb_rect)) btn_bomb_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_skill1_rect)) btn_skill1_pressed = 1;
            if (point_in_rect(mp.x, mp.y, btn_skill2_rect)) btn_skill2_pressed = 1;
        }
    }

    Vector2 mp = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (point_in_rect(mp.x, mp.y, btn_toggle_rect))
        {
            controls_visible = !controls_visible;
        }
    }

    static int prev_toggle_touch = 0;
    int touching_toggle = 0;
    for (int t = 0; t < touch_count; t++)
    {
        Vector2 tp = GetTouchPosition(t);
        if (point_in_rect(tp.x, tp.y, btn_toggle_rect))
        {
            touching_toggle = 1;
            break;
        }
    }
    if (touching_toggle && !prev_toggle_touch)
    {
        controls_visible = !controls_visible;
    }
    prev_toggle_touch = touching_toggle;
}


static int is_bomb_key()
{
    return IsKeyPressed(KEY_SPACE);
}

static int is_skill1_key()
{
    return IsKeyPressed(KEY_J) || IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1);
}

static int is_skill2_key()
{
    return IsKeyPressed(KEY_K) || IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2);
}


void game_init()
{

player.x=(float)GetRandomValue(PLAYER_RADIUS+20, WORLD_WIDTH-PLAYER_RADIUS-20);

player.y=(float)GetRandomValue(PLAYER_RADIUS+20, WORLD_HEIGHT-PLAYER_RADIUS-20);

camera_x = player.x;
camera_y = player.y;

player.speed=250;

player.boost_timer = 0;
player.shield_timer = 0;
player.bomb_cooldown = 0;
player.skill1_cooldown = 0;
player.skill2_cooldown = 0;

player.hp = CGAME_MAX_HP;
player.max_hp = CGAME_MAX_HP;
player.dead = 0;
player.respawn_timer = 0;

server_cfg.max_players = CGAME_MAX_PLAYERS;
server_cfg.bomb_cooldown = 30;
server_cfg.skill1_cooldown = 300;
server_cfg.skill2_cooldown = 300;
config_received = 0;

remote_count=0;

for(
int i=0;
i<CGAME_MAX_PLAYERS;
i++
)
{

remote_players[i].id=-1;
last_update_time[i]=-999.0f;

}

bomb_count = 0;
memset(bombs, 0, sizeof(bombs));

remote_bomb_count = 0;
memset(remote_bombs, 0, sizeof(remote_bombs));

btn_bomb_prev = 0;
btn_skill1_prev = 0;
btn_skill2_prev = 0;

controls_visible = 1;

}



void game_update()
{

float dt=
GetFrameTime();

update_touch_buttons();


if (player.dead)
{
    player.respawn_timer--;
    if (player.respawn_timer <= 0)
    {
        player.dead = 0;
        player.hp = player.max_hp;
        player.x = (float)GetRandomValue(PLAYER_RADIUS+20, WORLD_WIDTH-PLAYER_RADIUS-20);
        player.y = (float)GetRandomValue(PLAYER_RADIUS+20, WORLD_HEIGHT-PLAYER_RADIUS-20);
        camera_x = player.x;
        camera_y = player.y;
        player.shield_timer = 120;
    }
    return;
}


int move_up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP) || btn_up_pressed;
int move_down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN) || btn_down_pressed;
int move_left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || btn_left_pressed;
int move_right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) || btn_right_pressed;

float current_speed = player.speed;
if (player.boost_timer > 0)
{
    current_speed *= 2.0f;
    player.boost_timer--;
}

if (player.shield_timer > 0)
    player.shield_timer--;

if (player.bomb_cooldown > 0)
    player.bomb_cooldown--;

if (player.skill1_cooldown > 0)
    player.skill1_cooldown--;

if (player.skill2_cooldown > 0)
    player.skill2_cooldown--;

if (move_up)
player.y-=current_speed*dt;


if (move_down)
player.y+=current_speed*dt;


if (move_left)
player.x-=current_speed*dt;


if (move_right)
player.x+=current_speed*dt;


if (player.x < PLAYER_RADIUS) player.x = PLAYER_RADIUS;
if (player.x > WORLD_WIDTH - PLAYER_RADIUS) player.x = WORLD_WIDTH - PLAYER_RADIUS;
if (player.y < PLAYER_RADIUS) player.y = PLAYER_RADIUS;
if (player.y > WORLD_HEIGHT - PLAYER_RADIUS) player.y = WORLD_HEIGHT - PLAYER_RADIUS;


camera_x = player.x;
camera_y = player.y;



int bomb_pressed_now = btn_bomb_pressed && !btn_bomb_prev;
int skill1_pressed_now = btn_skill1_pressed && !btn_skill1_prev;
int skill2_pressed_now = btn_skill2_pressed && !btn_skill2_prev;

btn_bomb_prev = btn_bomb_pressed;
btn_skill1_prev = btn_skill1_pressed;
btn_skill2_prev = btn_skill2_pressed;

if ((is_bomb_key() || bomb_pressed_now) && player.bomb_cooldown <= 0)
{
    if (bomb_count < CGAME_MAX_BOMBS)
    {
        bombs[bomb_count].x = player.x;
        bombs[bomb_count].y = player.y + 30;
        bombs[bomb_count].timer = 1.5f;
        bombs[bomb_count].active = 1;
        bomb_count++;
        player.bomb_cooldown = server_cfg.bomb_cooldown;

        extern int network_send_bomb_placed(int player_id, float x, float y, float timer);
        network_send_bomb_placed(0, bombs[bomb_count-1].x, bombs[bomb_count-1].y, 1.5f);
    }
}

if ((is_skill1_key() || skill1_pressed_now) && player.skill1_cooldown <= 0)
{
    player.boost_timer = 120;
    player.skill1_cooldown = server_cfg.skill1_cooldown;
}

if ((is_skill2_key() || skill2_pressed_now) && player.skill2_cooldown <= 0)
{
    player.shield_timer = 180;
    player.skill2_cooldown = server_cfg.skill2_cooldown;
}


for (int i = bomb_count - 1; i >= 0; i--)
{
    if (bombs[i].active)
    {
        bombs[i].timer -= dt;
        if (bombs[i].timer <= 0.0f)
        {
            float ex = bombs[i].x;
            float ey = bombs[i].y;

            extern int network_send_bomb_explosion(float x, float y);
            network_send_bomb_explosion(ex, ey);

            game_apply_bomb_explosion(ex, ey);

            bombs[i].active = 0;
            for (int j = i; j < bomb_count - 1; j++)
                bombs[j] = bombs[j + 1];
            bomb_count--;
        }
    }
}

for (int i = remote_bomb_count - 1; i >= 0; i--)
{
    remote_bombs[i].timer -= dt;
    if (remote_bombs[i].timer <= 0.0f)
    {
        for (int j = i; j < remote_bomb_count - 1; j++)
            remote_bombs[j] = remote_bombs[j + 1];
        remote_bomb_count--;
    }
}


}



static void draw_button(Rectangle rect, const char* label, Color color, int pressed)
{
    Color fill = pressed ? color : (Color){ color.r, color.g, color.b, 120 };
    Color border = (Color){ color.r, color.g, color.b, 200 };

    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, 2, border);
    DrawText(label, rect.x + rect.width/2 - 10, rect.y + rect.height/2 - 8, 16, WHITE);
}


static void draw_hp_bar(float x, float y, int hp, int max_hp)
{
    float bar_w = 50.0f;
    float bar_h = 5.0f;
    float pct = (float)hp / (float)max_hp;

    DrawRectangle(x - bar_w/2, y, bar_w, bar_h, DARKGRAY);

    Color hp_color = GREEN;
    if (pct < 0.5f) hp_color = YELLOW;
    if (pct < 0.25f) hp_color = RED;

    DrawRectangle(x - bar_w/2, y, bar_w * pct, bar_h, hp_color);
    DrawRectangleLinesEx((Rectangle){ x - bar_w/2, y, bar_w, bar_h }, 1, WHITE);
}


void game_draw()
{


if (player.dead)
{
    DrawRectangle(0, 0, 800, 600, (Color){ 80, 0, 0, 180 });

    DrawText("YOU DIED", 280, 250, 40, RED);

    char respawn_text[64];
    float seconds = (float)player.respawn_timer / 60.0f;
    snprintf(respawn_text, sizeof(respawn_text), "Respawning in %.1f...", seconds);
    DrawText(respawn_text, 300, 310, 24, WHITE);

    return;
}


Camera2D camera;
camera.target = (Vector2){ camera_x, camera_y };
float cw = (float)GetScreenWidth();
float ch = (float)GetScreenHeight();
if (cw < 1.0f) cw = 800.0f;
if (ch < 1.0f) ch = 600.0f;
camera.offset = (Vector2){ cw * 0.5f, ch * 0.5f };
camera.zoom = 1.0f;
camera.rotation = 0.0f;

BeginMode2D(camera);

DrawRectangleLinesEx((Rectangle){ 0, 0, WORLD_WIDTH, WORLD_HEIGHT }, 4, (Color){ 80, 80, 80, 200 });

if (player.shield_timer > 0)
{
    DrawCircle(
    player.x,
    player.y,
    35,
    (Color){ 100, 200, 255, 120 }
    );
}

if (player.boost_timer > 0)
{
    DrawCircle(
    player.x,
    player.y,
    30,
    (Color){ 255, 200, 50, 100 }
    );
}

DrawCircle(
player.x,
player.y,
25,
RED
);



DrawText(
player_name_get(),
player.x-40,
player.y-50,
20,
WHITE
);

draw_hp_bar(player.x, player.y - 40, player.hp, player.max_hp);



float now = GetTime();

for(
int i=0;
i<remote_count;
i++
)
{

CGAME_PlayerState* p =
&remote_players[i];

if(now - last_update_time[i] > 3.0f)
continue;


if (p->dead)
{
    DrawText("X_X", p->x - 15, p->y - 10, 20, GRAY);
    DrawText(p->name, p->x-40, p->y-50, 16, GRAY);
    continue;
}

if (p->shield_timer > 0)
{
    DrawCircle(p->x, p->y, 35, (Color){ 100, 200, 255, 120 });
}

if (p->boost_timer > 0)
{
    DrawCircle(p->x, p->y, 30, (Color){ 255, 200, 50, 100 });
}

DrawCircle(
p->x,
p->y,
25,
BLUE
);



DrawText(
p->name,
p->x-40,
p->y-50,
20,
WHITE
);

draw_hp_bar(p->x, p->y - 40, p->hp, 100);

}



for (int i = 0; i < bomb_count; i++)
{
    if (bombs[i].active)
    {
        float alpha = bombs[i].timer < 0.5f ? (bombs[i].timer / 0.5f) : 1.0f;
        Color bomb_color = { 255, 100, 0, (unsigned char)(200 * alpha) };
        DrawCircle(bombs[i].x, bombs[i].y, 12, bomb_color);
        DrawCircle(bombs[i].x, bombs[i].y, 6, (Color){ 255, 255, 200, 220 });

        if (bombs[i].timer < 0.3f)
        {
            DrawCircle(bombs[i].x, bombs[i].y, 30, (Color){ 255, 150, 0, 80 });
        }
    }
}

for (int i = 0; i < remote_bomb_count; i++)
{
    float alpha = remote_bombs[i].timer < 0.5f ? (remote_bombs[i].timer / 0.5f) : 1.0f;
    Color bomb_color = { 255, 100, 0, (unsigned char)(200 * alpha) };
    DrawCircle(remote_bombs[i].x, remote_bombs[i].y, 12, bomb_color);
    DrawCircle(remote_bombs[i].x, remote_bombs[i].y, 6, (Color){ 255, 255, 200, 220 });

    if (remote_bombs[i].timer < 0.3f)
    {
        DrawCircle(remote_bombs[i].x, remote_bombs[i].y, 30, (Color){ 255, 150, 0, 80 });
    }
}

EndMode2D();


if (controls_visible)
{
    draw_button(btn_up_rect, "^", (Color){ 180, 180, 180, 0 }, btn_up_pressed);
    draw_button(btn_down_rect, "v", (Color){ 180, 180, 180, 0 }, btn_down_pressed);
    draw_button(btn_left_rect, "<", (Color){ 180, 180, 180, 0 }, btn_left_pressed);
    draw_button(btn_right_rect, ">", (Color){ 180, 180, 180, 0 }, btn_right_pressed);

    draw_button(btn_bomb_rect, "BOMB", (Color){ 220, 50, 50, 0 }, btn_bomb_pressed);
    draw_button(btn_skill1_rect, "1", (Color){ 220, 180, 30, 0 }, btn_skill1_pressed);
    draw_button(btn_skill2_rect, "2", (Color){ 50, 120, 220, 0 }, btn_skill2_pressed);

    if (player.bomb_cooldown > 0 && server_cfg.bomb_cooldown > 0)
    {
        float pct = (float)player.bomb_cooldown / (float)server_cfg.bomb_cooldown;
        DrawRectangle(btn_bomb_rect.x, btn_bomb_rect.y, btn_bomb_rect.width * pct, 4, (Color){ 100, 100, 100, 150 });
        char cd_text[16];
        snprintf(cd_text, sizeof(cd_text), "%.1f", (float)player.bomb_cooldown / 60.0f);
        int tw = MeasureText(cd_text, 10);
        DrawText(cd_text, btn_bomb_rect.x + btn_bomb_rect.width / 2 - tw / 2, btn_bomb_rect.y + btn_bomb_rect.height - 12, 10, WHITE);
    }

    if (player.skill1_cooldown > 0 && server_cfg.skill1_cooldown > 0)
    {
        float pct = (float)player.skill1_cooldown / (float)server_cfg.skill1_cooldown;
        DrawRectangle(btn_skill1_rect.x, btn_skill1_rect.y, btn_skill1_rect.width * pct, 4, (Color){ 100, 100, 100, 150 });
        char cd_text[16];
        snprintf(cd_text, sizeof(cd_text), "%.1f", (float)player.skill1_cooldown / 60.0f);
        int tw = MeasureText(cd_text, 10);
        DrawText(cd_text, btn_skill1_rect.x + btn_skill1_rect.width / 2 - tw / 2, btn_skill1_rect.y + btn_skill1_rect.height - 12, 10, WHITE);
    }

    if (player.skill2_cooldown > 0 && server_cfg.skill2_cooldown > 0)
    {
        float pct = (float)player.skill2_cooldown / (float)server_cfg.skill2_cooldown;
        DrawRectangle(btn_skill2_rect.x, btn_skill2_rect.y, btn_skill2_rect.width * pct, 4, (Color){ 100, 100, 100, 150 });
        char cd_text[16];
        snprintf(cd_text, sizeof(cd_text), "%.1f", (float)player.skill2_cooldown / 60.0f);
        int tw = MeasureText(cd_text, 10);
        DrawText(cd_text, btn_skill2_rect.x + btn_skill2_rect.width / 2 - tw / 2, btn_skill2_rect.y + btn_skill2_rect.height - 12, 10, WHITE);
    }
}

DrawRectangleRec(btn_toggle_rect, (Color){ 60, 60, 60, 180 });
DrawRectangleLinesEx(btn_toggle_rect, 2, (Color){ 150, 150, 150, 200 });
if (controls_visible)
    DrawText("<<", btn_toggle_rect.x + 8, btn_toggle_rect.y + 10, 18, WHITE);
else
    DrawText(">>", btn_toggle_rect.x + 8, btn_toggle_rect.y + 10, 18, WHITE);


if (player.boost_timer > 0)
{
    float pct = (float)player.boost_timer / 120.0f;
    DrawRectangle(10, 10, 100 * pct, 8, YELLOW);
    DrawText("BOOST", 10, 22, 12, YELLOW);
}

if (player.shield_timer > 0)
{
    float pct = (float)player.shield_timer / 180.0f;
    DrawRectangle(10, 40, 100 * pct, 8, SKYBLUE);
    DrawText("SHIELD", 10, 52, 12, SKYBLUE);
}


}



PlayerCar* game_get_player()
{

return &player;


}



void game_set_player_name(const char* name)
{
strncpy(player.name, name, CGAME_MAX_NAME - 1);
player.name[CGAME_MAX_NAME - 1] = '\0';
}



const char* player_name_get()
{
return player.name;
}



int game_get_bomb_count()
{
return bomb_count;
}

Bomb* game_get_bombs()
{
return bombs;
}



void game_update_remote(
CGAME_PlayerState* updates,
int count
)
{

float now = GetTime();

for(
int i=0;
i<count;
i++
)
{

CGAME_PlayerState* src =
&updates[i];

int found = 0;

for(
int j=0;
j<remote_count;
j++
)
{

if(
remote_players[j].id
==
src->id
)
{

remote_players[j] =
*src;
last_update_time[j] = now;

found = 1;

break;


}


}


if(
!found
&&
remote_count < CGAME_MAX_PLAYERS
)
{

remote_players[remote_count] =
*src;
last_update_time[remote_count] = now;
remote_count++;

}

}

}


void game_apply_bomb_explosion(float x, float y)
{
    const float radius = 60.0f;
    const float radius_sq = radius * radius;
    const int damage = 40;

    if (!player.dead && player.shield_timer <= 0)
    {
        if (dist_sq(x, y, player.x, player.y) <= radius_sq)
        {
            player.hp -= damage;
            if (player.hp <= 0)
            {
                player.hp = 0;
                player.dead = 1;
                player.respawn_timer = CGAME_RESPAWN_TIME;
            }
        }
    }

    for (int i = 0; i < remote_count; i++)
    {
        if (remote_players[i].dead)
            continue;

        if (dist_sq(x, y, remote_players[i].x, remote_players[i].y) <= radius_sq)
        {
            remote_players[i].hp -= damage;
            if (remote_players[i].hp <= 0)
            {
                remote_players[i].hp = 0;
                remote_players[i].dead = 1;
            }
        }
    }
}


void game_apply_server_config(const CGAME_ServerConfig* cfg)
{
    if (!cfg) return;
    server_cfg = *cfg;
    config_received = 1;
}

int game_get_max_players()
{
    return server_cfg.max_players;
}

int game_get_bomb_cooldown_max()
{
    return server_cfg.bomb_cooldown;
}

int game_get_skill1_cooldown_max()
{
    return server_cfg.skill1_cooldown;
}

int game_get_skill2_cooldown_max()
{
    return server_cfg.skill2_cooldown;
}

void game_add_remote_bomb(int player_id, float x, float y, float timer)
{
    if (remote_bomb_count < MAX_REMOTE_BOMBS)
    {
        remote_bombs[remote_bomb_count].player_id = player_id;
        remote_bombs[remote_bomb_count].x = x;
        remote_bombs[remote_bomb_count].y = y;
        remote_bombs[remote_bomb_count].timer = timer;
        remote_bomb_count++;
    }
}
