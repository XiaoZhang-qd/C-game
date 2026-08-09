#include "gui.h"

#include <stdio.h>
#include <string.h>

#define HISTORY_X 530
#define HISTORY_Y 90
#define HISTORY_W 250
#define HISTORY_ITEM_H 30
#define HISTORY_VISIBLE 5

static void gui_get_colors(int theme, Color* bg, Color* fg, Color* dim, Color* accent, Color* panel, Color* border)
{
    if (theme == 0)
    {
        *bg = (Color){ 15, 15, 25, 255 };
        *fg = WHITE;
        *dim = GRAY;
        *accent = (Color){ 70, 130, 180, 255 };
        *panel = (Color){ 30, 30, 40, 200 };
        *border = (Color){ 100, 100, 100, 200 };
    }
    else
    {
        *bg = (Color){ 235, 235, 245, 255 };
        *fg = (Color){ 20, 20, 30, 255 };
        *dim = (Color){ 120, 120, 130, 255 };
        *accent = (Color){ 40, 100, 180, 255 };
        *panel = (Color){ 210, 210, 220, 220 };
        *border = (Color){ 130, 130, 140, 200 };
    }
}

void gui_draw_menu(
ServerInput* input
)
{
    Color bg, fg, dim, accent, panel, border;
    gui_get_colors(input->theme, &bg, &fg, &dim, &accent, &panel, &border);

    ClearBackground(bg);

    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(TEXTBOX, BACKGROUND_COLOR, ColorToInt(panel));

    DrawText(
"CGame RACER",
230,
30,
40,
fg
);



GuiLabel(
(Rectangle){
120,
90,
100,
30
},
"Name"
);



GuiTextBox(
(Rectangle){
260,
90,
250,
35
},
input->name,
CGAME_MAX_NAME,
input->name_edit
);



GuiLabel(
(Rectangle){
120,
150,
100,
30
},
"Server IP"
);



GuiTextBox(
(Rectangle){
260,
150,
250,
35
},
input->ip,
64,
input->ip_edit
);



GuiLabel(
(Rectangle){
120,
210,
100,
30
},
"Port"
);



GuiTextBox(
(Rectangle){
260,
210,
250,
35
},
input->port,
16,
input->port_edit
);



GuiLabel(
(Rectangle){
120,
270,
110,
30
},
"Password"
);



GuiTextBox(
(Rectangle){
260,
270,
250,
35
},
input->password,
64,
input->password_edit
);

DrawText(
"(leave empty if none)",
260,
310,
12,
dim
);



if(
IsMouseButtonPressed(
MOUSE_BUTTON_LEFT
)
)
{

Vector2 mouse =
GetMousePosition();

input->name_edit = 0;
input->ip_edit = 0;
input->port_edit = 0;
input->password_edit = 0;

if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
90,
250,
35
}
)
)
{

input->name_edit=1;
input->error_msg[0]='\0';

}



else if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
150,
250,
35
}))
{

input->ip_edit=1;
input->error_msg[0]='\0';

}



else if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
210,
250,
35
}
)
)
{

input->port_edit=1;
input->error_msg[0]='\0';

}



else if(
CheckCollisionPointRec(
mouse,
(Rectangle){
260,
270,
250,
35
}
)
)
{

input->password_edit=1;
input->error_msg[0]='\0';

}



}



if(
GuiButton(
(Rectangle){
180,
360,
250,
50
},
"CONNECT"
))
{

input->connect=1;

}



if(
GuiButton(
(Rectangle){
180,
440,
250,
50
},
"CREATE SERVER"
))
{

input->create_server=1;

}



if(
GuiButton(
(Rectangle){
180,
520,
120,
35
},
"QUIT"
))
{

input->quit_app=1;

}



if(
strlen(input->error_msg)
>
0
)
{

DrawText(
input->error_msg,
180,
565,
18,
RED
);

}


if (GuiButton((Rectangle){ 10, 10, 80, 30 }, input->theme == 0 ? "LIGHT" : "DARK"))
{
    input->theme_toggle = 1;
}

GuiLabel(
(Rectangle){
HISTORY_X,
60,
200,
30
},
"Server History"
);

DrawRectangle(
HISTORY_X,
HISTORY_Y,
HISTORY_W,
HISTORY_VISIBLE * HISTORY_ITEM_H + 10,
panel
);

DrawRectangleLines(
HISTORY_X,
HISTORY_Y,
HISTORY_W,
HISTORY_VISIBLE * HISTORY_ITEM_H + 10,
border
);

if (input->history_count <= 0)
{
    DrawText(
        "(no saved servers)",
        HISTORY_X + 20,
        HISTORY_Y + 20,
        12,
        dim
    );
}
else
{
    int max_display = input->history_count < HISTORY_VISIBLE ? input->history_count : HISTORY_VISIBLE;

    for (int i = 0; i < max_display; i++)
    {
        float item_y = HISTORY_Y + 5 + i * HISTORY_ITEM_H;

        if (input->selected_history == i)
        {
            DrawRectangle(
                HISTORY_X + 2,
                item_y,
                HISTORY_W - 4,
                HISTORY_ITEM_H - 4,
                accent
            );
        }

        char entry_text[128];
        snprintf(
            entry_text,
            sizeof(entry_text),
            "%s - %s:%d",
            input->history[i].name,
            input->history[i].ip,
            input->history[i].port
        );
        DrawText(
            entry_text,
            HISTORY_X + 10,
            item_y + 8,
            12,
            fg
        );

        if (GuiButton(
            (Rectangle){
                HISTORY_X + HISTORY_W - 35,
                item_y + 4,
                28,
                HISTORY_ITEM_H - 8
            },
            "X"
        ))
        {
            input->delete_history = i;
        }
    }

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (int i = 0; i < max_display; i++)
        {
            float item_y = HISTORY_Y + 5 + i * HISTORY_ITEM_H;
            if (CheckCollisionPointRec(
                mouse,
                (Rectangle){
                    HISTORY_X + 2,
                    item_y,
                    HISTORY_W - 40,
                    HISTORY_ITEM_H - 4
                }
            ))
            {
                input->selected_history = i;
                strncpy(input->ip, input->history[i].ip, 63);
                input->ip[63] = '\0';
                char port_str[16];
                snprintf(port_str, sizeof(port_str), "%d", input->history[i].port);
                strncpy(input->port, port_str, 15);
                input->port[15] = '\0';
                if (strlen(input->history[i].name) > 0)
                {
                    strncpy(input->name, input->history[i].name, CGAME_MAX_NAME - 1);
                    input->name[CGAME_MAX_NAME - 1] = '\0';
                }
                input->password[0] = '\0';
                input->error_msg[0] = '\0';
                input->connect = 1;
                break;
            }
        }
    }
}



}



void gui_draw_game(
ServerInput* input
)
{
    Color bg, fg, dim, accent, panel, border;
    gui_get_colors(input->theme, &bg, &fg, &dim, &accent, &panel, &border);

    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(fg));

    if (GuiButton(
        (Rectangle){
            305,
            560,
            100,
            30
        },
        "EXIT"
    ))
    {
        input->exit_game = 1;
    }

    if (GuiButton((Rectangle){ 415, 560, 80, 30 }, input->theme == 0 ? "LIGHT" : "DARK"))
    {
        input->theme_toggle = 1;
    }

    float broadcast_y = 80;
    int start_idx = input->broadcast_count - 5;
    if (start_idx < 0) start_idx = 0;
    for (int i = start_idx; i < input->broadcast_count; i++)
    {
        Color bc_color = fg;
        const char* type_label = "";

        switch (input->broadcasts[i].type)
        {
            case BROADCAST_JOIN:
                bc_color = GREEN;
                type_label = "[JOIN] ";
                break;
            case BROADCAST_LEAVE:
                bc_color = YELLOW;
                type_label = "[LEAVE] ";
                break;
            case BROADCAST_DEATH:
                bc_color = RED;
                type_label = "[DEATH] ";
                break;
            case BROADCAST_SERVER:
                bc_color = SKYBLUE;
                type_label = "[SERVER] ";
                break;
            default:
                break;
        }

        char display[256];
        snprintf(
            display,
            sizeof(display),
            "%s%s",
            type_label,
            input->broadcasts[i].message
        );

        DrawText(
            display,
            10,
            broadcast_y,
            14,
            bc_color
        );

        broadcast_y += 20;
    }
}



void gui_draw_server_setup(
ServerInput* input
)
{
    Color bg, fg, dim, accent, panel, border;
    gui_get_colors(input->theme, &bg, &fg, &dim, &accent, &panel, &border);

    ClearBackground(bg);

    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(fg));
    GuiSetStyle(TEXTBOX, BACKGROUND_COLOR, ColorToInt(panel));

    DrawText(
"Create Server",
280,
30,
36,
fg
);



GuiLabel(
(Rectangle){
150,
90,
120,
30
},
"Server IP"
);

GuiTextBox(
(Rectangle){
290,
90,
250,
35
},
input->server_ip,
64,
input->server_ip_edit
);



GuiLabel(
(Rectangle){
150,
150,
120,
30
},
"Port"
);

GuiTextBox(
(Rectangle){
290,
150,
250,
35
},
input->server_port,
16,
input->server_port_edit
);



GuiLabel(
(Rectangle){
150,
210,
120,
30
},
"Max Players"
);

GuiTextBox(
(Rectangle){
290,
210,
250,
35
},
input->server_max_players,
8,
input->server_max_edit
);



GuiLabel(
(Rectangle){
150,
270,
120,
30
},
"Password"
);

GuiTextBox(
(Rectangle){
290,
270,
250,
35
},
input->server_password,
64,
input->server_password_edit
);

DrawText(
"(leave empty for no password)",
290,
310,
12,
dim
);



GuiLabel(
(Rectangle){
150,
340,
150,
30
},
"Bomb Cooldown (frames)"
);

GuiTextBox(
(Rectangle){
290,
340,
100,
35
},
input->server_bomb_cd,
8,
input->server_bomb_edit
);



GuiLabel(
(Rectangle){
150,
390,
150,
30
},
"Skill1 Cooldown (frames)"
);

GuiTextBox(
(Rectangle){
290,
390,
100,
35
},
input->server_skill1_cd,
8,
input->server_skill1_edit
);



GuiLabel(
(Rectangle){
150,
440,
150,
30
},
"Skill2 Cooldown (frames)"
);

GuiTextBox(
(Rectangle){
290,
440,
100,
35
},
input->server_skill2_cd,
8,
input->server_skill2_edit
);



if(
IsMouseButtonPressed(
MOUSE_BUTTON_LEFT
)
)
{
Vector2 mouse = GetMousePosition();

input->server_ip_edit = 0;
input->server_port_edit = 0;
input->server_max_edit = 0;
input->server_password_edit = 0;
input->server_bomb_edit = 0;
input->server_skill1_edit = 0;
input->server_skill2_edit = 0;

if(CheckCollisionPointRec(mouse, (Rectangle){290,90,250,35}))
    input->server_ip_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,150,250,35}))
    input->server_port_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,210,250,35}))
    input->server_max_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,270,250,35}))
    input->server_password_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,340,100,35}))
    input->server_bomb_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,390,100,35}))
    input->server_skill1_edit = 1;
else if(CheckCollisionPointRec(mouse, (Rectangle){290,440,100,35}))
    input->server_skill2_edit = 1;
}



if(
GuiButton(
(Rectangle){
180,
510,
200,
50
},
"START SERVER"
))
{
input->start_server = 1;
}



if(
GuiButton(
(Rectangle){
420,
510,
200,
50
},
"BACK"
))
{
input->back_to_menu = 1;
}

if (GuiButton((Rectangle){ 10, 10, 80, 30 }, input->theme == 0 ? "LIGHT" : "DARK"))
{
    input->theme_toggle = 1;
}

}
