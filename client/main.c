#include "raylib.h"

#include "gui.h"

#include "game.h"

#include "network.h"

#include "../common/socket.h"

#include "../common/hide_terminal.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>

// Win32 low-level keyboard hook to intercept ESC during gameplay
// Using dynamic loading to avoid <windows.h> symbol conflicts with raylib

// Custom types (not available with WIN32_LEAN_AND_MEAN)
typedef LRESULT (CALLBACK* LowLevelKeyboardProc)(int, WPARAM, LPARAM);
typedef HHOOK (*SetWindowsHookExW_t)(int, LowLevelKeyboardProc, HINSTANCE, DWORD);
typedef BOOL (*UnhookWindowsHookEx_t)(HHOOK);
typedef LRESULT (*CallNextHookEx_t)(HHOOK, int, WPARAM, LPARAM);

// Custom KBDLLHOOKSTRUCT definition (not available with WIN32_LEAN_AND_MEAN)
typedef struct tagCgameKBDLLHOOKSTRUCT {
    DWORD     vkCode;
    DWORD     scanCode;
    DWORD     flags;
    DWORD     time;
    ULONG_PTR dwExtraInfo;
} CgameKBDLLHOOKSTRUCT;

// Constants
#ifndef WH_KEYBOARD_LL
#define WH_KEYBOARD_LL 13
#endif
#ifndef HC_ACTION
#define HC_ACTION 0
#endif
#ifndef WM_KEYDOWN
#define WM_KEYDOWN 0x0100
#endif
#ifndef WM_SYSKEYDOWN
#define WM_SYSKEYDOWN 0x0104
#endif
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

// Global state for keyboard hook
static HHOOK g_keyboardHook = NULL;
static int g_gameplayActive = 0;
static int g_escPressed = 0;

// Low-level keyboard hook procedure
static LRESULT CALLBACK CgameKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= HC_ACTION && g_gameplayActive)
    {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            CgameKBDLLHOOKSTRUCT* kbData = (CgameKBDLLHOOKSTRUCT*)lParam;
            if (kbData->vkCode == VK_ESCAPE)
            {
                g_escPressed = 1;
                return 1; // Block ESC from reaching the application
            }
        }
    }

    // Load CallNextHookEx dynamically and pass through
    static CallNextHookEx_t pCallNextHookEx = NULL;
    if (!pCallNextHookEx)
    {
        HMODULE hUser32 = LoadLibraryA("user32.dll");
        if (hUser32)
        {
            pCallNextHookEx = (CallNextHookEx_t)GetProcAddress(hUser32, "CallNextHookEx");
        }
    }
    if (pCallNextHookEx)
    {
        return pCallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }
    return 1;
}

// Install keyboard hook
static void cgame_install_keyboard_hook(void)
{
    if (g_keyboardHook) return;

    static SetWindowsHookExW_t pSetWindowsHookExW = NULL;
    if (!pSetWindowsHookExW)
    {
        HMODULE hUser32 = LoadLibraryA("user32.dll");
        if (hUser32)
        {
            pSetWindowsHookExW = (SetWindowsHookExW_t)GetProcAddress(hUser32, "SetWindowsHookExW");
        }
    }
    if (pSetWindowsHookExW)
    {
        g_keyboardHook = pSetWindowsHookExW(WH_KEYBOARD_LL, CgameKeyboardProc, GetModuleHandleA(NULL), 0);
    }
}

// Remove keyboard hook
static void cgame_remove_keyboard_hook(void)
{
    if (!g_keyboardHook) return;

    static UnhookWindowsHookEx_t pUnhookWindowsHookEx = NULL;
    if (!pUnhookWindowsHookEx)
    {
        HMODULE hUser32 = LoadLibraryA("user32.dll");
        if (hUser32)
        {
            pUnhookWindowsHookEx = (UnhookWindowsHookEx_t)GetProcAddress(hUser32, "UnhookWindowsHookEx");
        }
    }
    if (pUnhookWindowsHookEx)
    {
        pUnhookWindowsHookEx(g_keyboardHook);
    }
    g_keyboardHook = NULL;
}
#endif



#define SERVER_HISTORY_FILE "cgame_servers.txt"
#define CGAME_CONFIG_FILE "cgame_config.txt"


static int config_load_theme()
{
    FILE* f = fopen(CGAME_CONFIG_FILE, "r");
    if (!f) return 0;
    int theme = 0;
    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        int v;
        if (sscanf(line, "theme=%d", &v) == 1)
        {
            theme = v ? 1 : 0;
            break;
        }
    }
    fclose(f);
    return theme;
}

static void config_save_theme(int theme)
{
    FILE* f = fopen(CGAME_CONFIG_FILE, "w");
    if (!f) return;
    fprintf(f, "theme=%d\n", theme);
    fclose(f);
}



static void launch_server_process(ServerInput* input)
{
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
        "c-game-server-x64.exe -ip %s -port %s -max %s -bomb %s -skill1 %s -skill2 %s",
        input->server_ip,
        input->server_port,
        input->server_max_players,
        input->server_bomb_cd,
        input->server_skill1_cd,
        input->server_skill2_cd
    );

    if (strlen(input->server_password) > 0)
    {
        char pass_cmd[1100];
        snprintf(pass_cmd, sizeof(pass_cmd), "%s -password %s", cmd, input->server_password);
        strncpy(cmd, pass_cmd, sizeof(cmd) - 1);
        cmd[sizeof(cmd) - 1] = '\0';
    }

#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;

    if (CreateProcessA(
        NULL,
        cmd,
        NULL, NULL, FALSE,
        0,
        NULL, NULL,
        &si, &pi
    ))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
#else
    system(cmd);
#endif
}



int main()
{


cgame_socket_init();



InitWindow(
800,
600,
"CGame Racer"
);



SetTargetFPS(60);

SetExitKey(KEY_NULL);

#ifdef _WIN32
    cgame_install_keyboard_hook();
#endif



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

input.password[0] = '\0';

strcpy(input.server_ip, "0.0.0.0");
strcpy(input.server_port, "25565");
strcpy(input.server_max_players, "8");
input.server_password[0] = '\0';
strcpy(input.server_bomb_cd, "30");
strcpy(input.server_skill1_cd, "300");
strcpy(input.server_skill2_cd, "300");

input.theme = config_load_theme();
game_set_theme(input.theme);



GUI_State state =
GUI_MENU;

game_init();



input.history_count = network_load_server_history(
    SERVER_HISTORY_FILE,
    input.history,
    CGAME_MAX_SERVER_HISTORY
);



while(
1
)
{

BeginDrawing();

Color bg_col = input.theme == 0 ? (Color){ 15, 15, 25, 255 } : (Color){ 235, 235, 245, 255 };
ClearBackground(bg_col);

if (input.theme_toggle)
{
    input.theme_toggle = 0;
    input.theme = input.theme == 0 ? 1 : 0;
    game_set_theme(input.theme);
    config_save_theme(input.theme);
}



if (state == GUI_MENU)
{
    EnableWindowClose();

#ifdef _WIN32
    g_gameplayActive = 0;
#endif

    gui_draw_menu(&input);

    if (IsKeyPressed(KEY_ESCAPE) || WindowShouldClose())
    {
        input.quit_app = 1;
    }

    if (input.quit_app)
    {
        input.quit_app = 0;
        break;
    }

    if (input.delete_history >= 0 && input.delete_history < input.history_count)
    {
        network_delete_server_history(SERVER_HISTORY_FILE, input.delete_history);
        input.history_count = network_load_server_history(
            SERVER_HISTORY_FILE,
            input.history,
            CGAME_MAX_SERVER_HISTORY
        );
        input.delete_history = -1;
        input.selected_history = -1;
    }

    if (input.create_server)
    {
        input.create_server = 0;
        state = GUI_SERVER_SETUP;
    }

    if (input.connect)
    {
        input.error_msg[0] = '\0';

        int port = atoi(input.port);

        if (network_connect(input.ip, port) == 0)
        {
            int ret;
            if (strlen(input.password) > 0)
            {
                ret = network_send_login_with_password(input.name, input.password);
            }
            else
            {
                ret = network_send_login(input.name);
            }

            if (ret == 0)
            {
                game_set_player_name(input.name);

                network_add_server_history(
                    SERVER_HISTORY_FILE,
                    input.ip,
                    port,
                    input.name
                );
                input.history_count = network_load_server_history(
                    SERVER_HISTORY_FILE,
                    input.history,
                    CGAME_MAX_SERVER_HISTORY
                );

                state = GUI_GAME;
                input.broadcast_count = 0;
            }
            else
            {
                if (ret == LOGIN_NAME_TAKEN)
                {
                    strcpy(input.error_msg, "Name already taken! Please choose another.");
                }
                else if (ret == LOGIN_NAME_EMPTY)
                {
                    strcpy(input.error_msg, "Name cannot be empty!");
                }
                else if (ret == LOGIN_WRONG_PASSWORD)
                {
                    strcpy(input.error_msg, "Wrong password!");
                }
                else if (ret == LOGIN_SERVER_LOCKED)
                {
                    strcpy(input.error_msg, "Server requires a password!");
                }
                else
                {
                    strcpy(input.error_msg, "Login failed. Try again.");
                }

                network_disconnect();
            }
        }
        else
        {
            strcpy(input.error_msg, "Cannot connect to server.");
        }

        input.connect = 0;
    }
}
else if (state == GUI_SERVER_SETUP)
{
    EnableWindowClose();

#ifdef _WIN32
    g_gameplayActive = 0;
#endif

    gui_draw_server_setup(&input);

    if (IsKeyPressed(KEY_ESCAPE))
    {
        input.back_to_menu = 1;
    }

    if (input.back_to_menu)
    {
        input.back_to_menu = 0;
        state = GUI_MENU;
    }

    if (input.start_server)
    {
        input.start_server = 0;
        launch_server_process(&input);
        state = GUI_MENU;
        strcpy(input.error_msg, "Server started in a new window.");
    }
}
else if (state == GUI_GAME)
{
    DisableWindowClose();

#ifdef _WIN32
    g_gameplayActive = 1;
    g_escPressed = 0;
#endif

    game_update();

#ifdef _WIN32
    if (g_escPressed)
    {
        g_escPressed = 0;
#else
    if (IsKeyPressed(KEY_ESCAPE))
    {
#endif
        EnableWindowClose();
        network_disconnect();
        state = GUI_MENU;
        input.error_msg[0] = '\0';
        input.password[0] = '\0';
        input.exit_game = 0;
        input.broadcast_count = 0;
    }

    PlayerCar* p = game_get_player();

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
        EnableWindowClose();
        network_disconnect();
        state = GUI_MENU;
        input.error_msg[0] = '\0';
        strcpy(input.error_msg, "Disconnected from server.");
        input.password[0] = '\0';
    }

    CGAME_PlayerState remote_updates[CGAME_MAX_PLAYERS];
    int received = network_recv_player(remote_updates, CGAME_MAX_PLAYERS);

    if(received == -2)
    {
        EnableWindowClose();
        network_disconnect();
        state = GUI_MENU;
        input.error_msg[0] = '\0';
        strcpy(input.error_msg, "Disconnected from server.");
        input.password[0] = '\0';
    }

    if(received > 0)
    {
        game_update_remote(remote_updates, received);
    }

    CGAME_BombExplosion explosions[32];
    int exp_count = network_get_bomb_explosions(explosions, 32);

    for(int i=0; i<exp_count; i++)
    {
        game_apply_bomb_explosion(explosions[i].x, explosions[i].y);
    }

    CGAME_BombPlaced placed_bombs[32];
    int placed_count = network_get_bomb_placed(placed_bombs, 32);

    for(int i=0; i<placed_count; i++)
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

    CGAME_Broadcast new_bcasts[CGAME_BROADCAST_QUEUE_SIZE];
    int new_count = network_get_broadcasts(new_bcasts, CGAME_BROADCAST_QUEUE_SIZE);
    for (int i = 0; i < new_count && input.broadcast_count < CGAME_BROADCAST_QUEUE_SIZE; i++)
    {
        input.broadcasts[input.broadcast_count] = new_bcasts[i];
        input.broadcast_count++;
    }
    if (input.broadcast_count > CGAME_BROADCAST_QUEUE_SIZE)
    {
        int overflow = input.broadcast_count - CGAME_BROADCAST_QUEUE_SIZE;
        memmove(
            input.broadcasts,
            input.broadcasts + overflow,
            CGAME_BROADCAST_QUEUE_SIZE * sizeof(CGAME_Broadcast)
        );
        input.broadcast_count = CGAME_BROADCAST_QUEUE_SIZE;
    }

    game_draw();

    gui_draw_game(&input);

    if (input.exit_game)
    {
        EnableWindowClose();
        network_disconnect();
        state = GUI_MENU;
        input.error_msg[0] = '\0';
        input.password[0] = '\0';
        input.exit_game = 0;
        input.broadcast_count = 0;
    }
}



EndDrawing();

}



#ifdef _WIN32
    cgame_remove_keyboard_hook();
#endif

CloseWindow();

network_disconnect();
cgame_socket_cleanup();

return 0;

}