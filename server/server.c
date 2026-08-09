#include "server.h"

#include <stdio.h>
#include <string.h>



#ifdef _WIN32



#define CloseWindow CloseWindowWin32
#define Rectangle RectangleWin32
#define ShowCursor ShowCursorWin32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CloseWindow
#undef Rectangle
#undef ShowCursor

#else
#include <unistd.h>
#include <fcntl.h>
#endif



static void send_login_response(
    cgame_socket sock,
    int code
)
{

    CGAME_Packet packet;
    memset(
        &packet,
        0,
        sizeof(packet)
    );
    packet.header.type =
        PKT_LOGIN_RESPONSE;
    packet.header.size =
        sizeof(int);

    memcpy(
        packet.data,
        &code,
        sizeof(int)
    );

    cgame_send(
        sock,
        &packet,
        sizeof(packet)
    );

}



static int is_name_taken(
    CGAME_Server* server,
    const char* name,
    int exclude_index
)
{

    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {

        if(
            !server->players[i].logged_in
        )
            continue;

        if(
            i == exclude_index
        )
            continue;

        if(
            strcmp(
                server->players[i].name,
                name
            ) == 0
        )
            return 1;

    }

    return 0;

}



static void send_broadcast(
    CGAME_Server* server,
    int type,
    const char* name,
    const char* message
);

static void disconnect_player(
    CGAME_Player* player,
    CGAME_Server* server
)
{
    if (player->logged_in)
    {
        char leave_msg[128];
        snprintf(leave_msg, sizeof(leave_msg), "%s left the server", player->name);
        send_broadcast(server, BROADCAST_LEAVE, player->name, leave_msg);
    }

    cgame_socket_close(
        player->tcp
    );
    player->tcp =
        CGAME_INVALID_SOCKET;
    player->connected = 0;
    player->logged_in = 0;
    player->name[0] = '\0';
    player->state.id = -1;
    player->state.name[0] = '\0';
    player->state.hp = 100;
    player->state.dead = 0;

}



static void send_player_state(
    CGAME_Player* to,
    CGAME_Player* player
)
{

    if(!player->logged_in)
        return;

    CGAME_Packet packet;
    memset(
        &packet,
        0,
        sizeof(packet)
    );
    packet.header.type =
        PKT_PLAYER_UPDATE;
    packet.header.size =
        sizeof(CGAME_PlayerState);

    CGAME_PlayerState state;
    memset(&state, 0, sizeof(state));
    state.x = player->state.x;
    state.y = player->state.y;
    state.speed = player->state.speed;
    state.lap = player->state.lap;
    state.id = player->id;
    state.hp = player->state.hp;
    state.dead = player->state.dead;
    state.boost_timer = player->state.boost_timer;
    state.shield_timer = player->state.shield_timer;
    strncpy(
        state.name,
        player->name,
        CGAME_MAX_NAME - 1
    );
    state.name[CGAME_MAX_NAME - 1] = '\0';

    memcpy(
        packet.data,
        &state,
        sizeof(CGAME_PlayerState)
    );

    cgame_send(
        to->tcp,
        &packet,
        sizeof(packet)
    );

}



static void broadcast_players(
    CGAME_Server* server
)
{

    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {

        if(
            !server->players[i].logged_in
        )
            continue;

        for(
            int j=0;
            j<server->max_players;
            j++
        )
        {

            if(
                i == j
            )
                continue;

            if(
                server->players[j].logged_in
            )
            {

                send_player_state(
                    &server->players[j],
                    &server->players[i]
                );

            }

        }

    }


}


static void send_broadcast(
    CGAME_Server* server,
    int type,
    const char* name,
    const char* message
)
{
    CGAME_Broadcast bc;
    memset(&bc, 0, sizeof(bc));
    bc.type = type;
    if (name) strncpy(bc.name, name, CGAME_MAX_NAME - 1);
    if (message) strncpy(bc.message, message, sizeof(bc.message) - 1);

    CGAME_Packet packet;
    memset(&packet, 0, sizeof(packet));
    packet.header.type = PKT_BROADCAST;
    packet.header.size = sizeof(CGAME_Broadcast);
    memcpy(packet.data, &bc, sizeof(bc));

    for (int i = 0; i < server->max_players; i++)
    {
        if (server->players[i].logged_in)
        {
            cgame_send(server->players[i].tcp, &packet, sizeof(packet));
        }
    }
}



int server_start(
    CGAME_Server* server,
    const char* ip,
    int port,
    int max_players,
    const char* password
)
{

    memset(
        server,
        0,
        sizeof(CGAME_Server)
    );

    strcpy(
        server->ip,
        ip
    );

    server->port =
        port;

    server->max_players =
        max_players;

    if (password && strlen(password) > 0)
    {
        strncpy(server->password, password, 63);
        server->password[63] = '\0';
    }

    if(
        cgame_socket_init()!=0
    )
        return -1;

    server->listen_socket =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );

    if(
        server->listen_socket
        ==
        CGAME_INVALID_SOCKET
    )
        return -1;

    cgame_socket_set_nonblocking(
        server->listen_socket
    );

    struct sockaddr_in addr;
    memset(
        &addr,
        0,
        sizeof(addr)
    );
    addr.sin_family =
        AF_INET;
    addr.sin_port =
        htons(port);
    addr.sin_addr.s_addr =
        INADDR_ANY;

    if(
        bind(
            server->listen_socket,
            (struct sockaddr*)&addr,
            sizeof(addr)
        )
        <0
    )
    {
        cgame_socket_close(server->listen_socket);
        server->listen_socket = CGAME_INVALID_SOCKET;
        return -1;
    }

    listen(
        server->listen_socket,
        max_players
    );

    for(
        int i=0;
        i<CGAME_MAX_PLAYERS;
        i++
    )
    {

        server->players[i].tcp =
            CGAME_INVALID_SOCKET;
        server->players[i].connected = 0;
        server->players[i].logged_in = 0;
        server->players[i].id = i;
        server->players[i].state.id = -1;
        server->players[i].state.hp = 100;
        server->players[i].state.dead = 0;
        server->players[i].last_active_frame = 0;

    }

    server->config.max_players = max_players;
    server->config.bomb_cooldown = 30;
    server->config.skill1_cooldown = 300;
    server->config.skill2_cooldown = 300;

    server->running = 1;

    printf(
        "Server listening %s:%d\n",
        ip,
        port
    );

    return 0;

}



void server_update(
    CGAME_Server* server
)
{

    if(
        !server->running
    )
        return;

    static int server_frame = 0;
    server_frame++;

    struct sockaddr_in client_addr;

#ifdef _WIN32
    int size =
        sizeof(client_addr);
#else
    socklen_t size =
        sizeof(client_addr);
#endif

    cgame_socket client =
        accept(
            server->listen_socket,
            (struct sockaddr*)&client_addr,
            &size
        );

    if(
        client != CGAME_INVALID_SOCKET
    )
    {

        cgame_socket_set_nonblocking(
            client
        );

        int slot_found = 0;

        for(
            int i=0;
            i<server->max_players;
            i++
        )
        {

            if(
                server->players[i].tcp
                ==
                CGAME_INVALID_SOCKET
            )
            {

                CGAME_Player* player =
                    &server->players[i];

                player->tcp =
                    client;
                player->connected = 1;
                player->logged_in = 0;
                player->id = i;
                player->name[0] = '\0';
                player->state.x =
                    100+i*50;
                player->state.y =
                    100;
                player->state.id = -1;
                player->state.name[0] = '\0';
                player->state.hp = 100;
                player->state.dead = 0;
                player->last_active_frame = 0;

                printf(
                    "Player %d connected (awaiting login)\n",
                    i
                );

                slot_found = 1;
                break;

            }

        }

        if(!slot_found)
        {
            printf("Server full, rejecting connection\n");
            cgame_socket_close(client);
        }

    }



    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {

        CGAME_Player* player =
            &server->players[i];

        if(
            player->tcp
            ==
            CGAME_INVALID_SOCKET
        )
            continue;

        CGAME_Packet packet;
        int r =
        cgame_recv(
            player->tcp,
            &packet,
            sizeof(packet)
        );

        if(
            r == 0
        )
        {

            printf(
                "Player %d disconnected\n",
                i
            );
            disconnect_player(
                player,
                server
            );
            continue;

        }

        if(
            r < 0
        )
        {
            if (player->logged_in &&
                (server_frame - player->last_active_frame) > 1000)
            {
                printf(
                    "Player %d timed out (inactive)\n",
                    i
                );
                disconnect_player(
                    player,
                    server
                );
            }
            continue;
        }

        player->last_active_frame = server_frame;

        if(
            packet.header.type
            ==
            PKT_LOGIN
        )
        {
            if (strlen(server->password) > 0)
            {
                send_login_response(
                    player->tcp,
                    LOGIN_SERVER_LOCKED
                );
                printf(
                    "Player %d rejected: server requires password\n",
                    i
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            CGAME_Login* login =
                (CGAME_Login*)packet.data;

            if(
                strlen(login->name)
                == 0
            )
            {
                send_login_response(
                    player->tcp,
                    LOGIN_NAME_EMPTY
                );
                printf(
                    "Player %d rejected: empty name\n",
                    i
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            if(
                is_name_taken(
                    server,
                    login->name,
                    i
                )
            )
            {
                send_login_response(
                    player->tcp,
                    LOGIN_NAME_TAKEN
                );
                printf(
                    "Player %d rejected: name '%s' already taken\n",
                    i,
                    login->name
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            strncpy(
                player->name,
                login->name,
                CGAME_MAX_NAME - 1
            );
            player->name[CGAME_MAX_NAME - 1] = '\0';
            player->logged_in = 1;

            send_login_response(
                player->tcp,
                LOGIN_OK
            );

            {
                CGAME_Packet cfg_pkt;
                memset(&cfg_pkt, 0, sizeof(cfg_pkt));
                cfg_pkt.header.type = PKT_SERVER_CONFIG;
                cfg_pkt.header.size = sizeof(CGAME_ServerConfig);
                memcpy(cfg_pkt.data, &server->config, sizeof(CGAME_ServerConfig));
                cgame_send(player->tcp, &cfg_pkt, sizeof(cfg_pkt));
            }

            printf(
                "Player %d logged in: %s\n",
                i,
                player->name
            );

            {
                char join_msg[128];
                snprintf(join_msg, sizeof(join_msg), "%s joined the server", player->name);
                send_broadcast(server, BROADCAST_JOIN, player->name, join_msg);
            }
        }
        else if(
            packet.header.type
            ==
            PKT_LOGIN_PASSWORD
        )
        {
            CGAME_LoginPassword* login =
                (CGAME_LoginPassword*)packet.data;

            if (strlen(server->password) > 0 &&
                strcmp(login->password, server->password) != 0)
            {
                send_login_response(
                    player->tcp,
                    LOGIN_WRONG_PASSWORD
                );
                printf(
                    "Player %d rejected: wrong password\n",
                    i
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            if(
                strlen(login->name)
                == 0
            )
            {
                send_login_response(
                    player->tcp,
                    LOGIN_NAME_EMPTY
                );
                printf(
                    "Player %d rejected: empty name\n",
                    i
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            if(
                is_name_taken(
                    server,
                    login->name,
                    i
                )
            )
            {
                send_login_response(
                    player->tcp,
                    LOGIN_NAME_TAKEN
                );
                printf(
                    "Player %d rejected: name '%s' already taken\n",
                    i,
                    login->name
                );
                disconnect_player(
                    player,
                    server
                );
                continue;
            }

            strncpy(
                player->name,
                login->name,
                CGAME_MAX_NAME - 1
            );
            player->name[CGAME_MAX_NAME - 1] = '\0';
            player->logged_in = 1;

            send_login_response(
                player->tcp,
                LOGIN_OK
            );

            {
                CGAME_Packet cfg_pkt;
                memset(&cfg_pkt, 0, sizeof(cfg_pkt));
                cfg_pkt.header.type = PKT_SERVER_CONFIG;
                cfg_pkt.header.size = sizeof(CGAME_ServerConfig);
                memcpy(cfg_pkt.data, &server->config, sizeof(CGAME_ServerConfig));
                cgame_send(player->tcp, &cfg_pkt, sizeof(cfg_pkt));
            }

            printf(
                "Player %d logged in: %s\n",
                i,
                player->name
            );

            {
                char join_msg[128];
                snprintf(join_msg, sizeof(join_msg), "%s joined the server", player->name);
                send_broadcast(server, BROADCAST_JOIN, player->name, join_msg);
            }
        }
        else if(
            packet.header.type
            ==
            PKT_PLAYER_UPDATE
        )
        {

            if(
                !player->logged_in
            )
                continue;

            if(
                packet.header.size
                !=
                sizeof(CGAME_PlayerState)
            )
                continue;

            CGAME_PlayerState* state =
                (CGAME_PlayerState*)packet.data;

            int was_dead = player->state.dead;
            player->state.x =
                state->x;
            player->state.y =
                state->y;
            player->state.speed =
                state->speed;
            player->state.hp =
                state->hp;
            player->state.dead =
                state->dead;
            player->state.boost_timer =
                state->boost_timer;
            player->state.shield_timer =
                state->shield_timer;

            if (!was_dead && state->dead)
            {
                char death_msg[128];
                snprintf(death_msg, sizeof(death_msg), "%s died", player->name);
                send_broadcast(server, BROADCAST_DEATH, player->name, death_msg);
            }

        }
        else if(
            packet.header.type
            ==
            PKT_BOMB_EXPLOSION
        )
        {

            if(
                !player->logged_in
            )
                continue;

            if(
                packet.header.size
                !=
                sizeof(CGAME_BombExplosion)
            )
                continue;

            for(
                int j=0;
                j<server->max_players;
                j++
            )
            {

                if(
                    j == i
                )
                    continue;

                if(
                    server->players[j].logged_in
                )
                {

                    cgame_send(
                        server->players[j].tcp,
                        &packet,
                        sizeof(packet)
                    );

                }

            }

        }
        else if(
            packet.header.type
            ==
            PKT_BOMB_PLACED
        )
        {

            if(
                !player->logged_in
            )
                continue;

            if(
                packet.header.size
                !=
                sizeof(CGAME_BombPlaced)
            )
                continue;

            for(
                int j=0;
                j<server->max_players;
                j++
            )
            {

                if(
                    j == i
                )
                    continue;

                if(
                    server->players[j].logged_in
                )
                {

                    cgame_send(
                        server->players[j].tcp,
                        &packet,
                        sizeof(packet)
                    );

                }

            }

        }

    }



    broadcast_players(
        server
    );



#ifdef _WIN32
    Sleep(10);
#else
    usleep(10000);
#endif

}



void server_stop(
    CGAME_Server* server
)
{

    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {

        if(
            server->players[i].tcp
            !=
            CGAME_INVALID_SOCKET
        )
        {

            cgame_socket_close(
                server->players[i].tcp
            );
            server->players[i].tcp =
                CGAME_INVALID_SOCKET;

        }

    }

    cgame_socket_close(
        server->listen_socket
    );

    server->listen_socket = CGAME_INVALID_SOCKET;
    cgame_socket_cleanup();

}
