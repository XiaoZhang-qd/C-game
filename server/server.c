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
    pxpt_socket sock,
    int code
)
{

    PXPT_Packet packet;
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

    pxpt_send(
        sock,
        &packet,
        sizeof(packet)
    );

}



static int is_name_taken(
    PXPT_Server* server,
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



static void disconnect_player(
    PXPT_Player* player
)
{

    pxpt_socket_close(
        player->tcp
    );
    player->tcp =
        PXPT_INVALID_SOCKET;
    player->connected = 0;
    player->logged_in = 0;
    player->name[0] = '\0';
    player->state.id = -1;
    player->state.name[0] = '\0';
    player->state.hp = 100;
    player->state.dead = 0;

}



static void send_player_state(
    PXPT_Player* to,
    PXPT_Player* player
)
{

    if(!player->logged_in)
        return;

    PXPT_Packet packet;
    memset(
        &packet,
        0,
        sizeof(packet)
    );
    packet.header.type =
        PKT_PLAYER_UPDATE;
    packet.header.size =
        sizeof(PXPT_PlayerState);

    PXPT_PlayerState state;
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
        PXPT_MAX_NAME - 1
    );
    state.name[PXPT_MAX_NAME - 1] = '\0';

    memcpy(
        packet.data,
        &state,
        sizeof(PXPT_PlayerState)
    );

    pxpt_send(
        to->tcp,
        &packet,
        sizeof(packet)
    );

}



static void broadcast_players(
    PXPT_Server* server
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



int server_start(
    PXPT_Server* server,
    const char* ip,
    int port,
    int max_players
)
{

    memset(
        server,
        0,
        sizeof(PXPT_Server)
    );

    strcpy(
        server->ip,
        ip
    );

    server->port =
        port;

    server->max_players =
        max_players;

    if(
        pxpt_socket_init()!=0
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
        PXPT_INVALID_SOCKET
    )
        return -1;

    pxpt_socket_set_nonblocking(
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
        pxpt_socket_close(server->listen_socket);
        server->listen_socket = PXPT_INVALID_SOCKET;
        return -1;
    }

    listen(
        server->listen_socket,
        max_players
    );

    for(
        int i=0;
        i<PXPT_MAX_PLAYERS;
        i++
    )
    {

        server->players[i].tcp =
            PXPT_INVALID_SOCKET;
        server->players[i].connected = 0;
        server->players[i].logged_in = 0;
        server->players[i].id = i;
        server->players[i].state.id = -1;
        server->players[i].state.hp = 100;
        server->players[i].state.dead = 0;

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
    PXPT_Server* server
)
{

    if(
        !server->running
    )
        return;

    struct sockaddr_in client_addr;

#ifdef _WIN32
    int size =
        sizeof(client_addr);
#else
    socklen_t size =
        sizeof(client_addr);
#endif

    pxpt_socket client =
        accept(
            server->listen_socket,
            (struct sockaddr*)&client_addr,
            &size
        );

    if(
        client != PXPT_INVALID_SOCKET
    )
    {

        pxpt_socket_set_nonblocking(
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
                PXPT_INVALID_SOCKET
            )
            {

                PXPT_Player* player =
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
            pxpt_socket_close(client);
        }

    }



    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {

        PXPT_Player* player =
            &server->players[i];

        if(
            player->tcp
            ==
            PXPT_INVALID_SOCKET
        )
            continue;

        PXPT_Packet packet;
        int r =
        pxpt_recv(
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
                player
            );
            continue;

        }

        if(
            r < 0
        )
            continue;

        if(
            packet.header.type
            ==
            PKT_LOGIN
        )
        {

            PXPT_Login* login =
                (PXPT_Login*)packet.data;

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
                    player
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
                    player
                );
                continue;

            }

            strncpy(
                player->name,
                login->name,
                PXPT_MAX_NAME - 1
            );
            player->name[PXPT_MAX_NAME - 1] = '\0';
            player->logged_in = 1;

            send_login_response(
                player->tcp,
                LOGIN_OK
            );

            {
                PXPT_Packet cfg_pkt;
                memset(&cfg_pkt, 0, sizeof(cfg_pkt));
                cfg_pkt.header.type = PKT_SERVER_CONFIG;
                cfg_pkt.header.size = sizeof(PXPT_ServerConfig);
                memcpy(cfg_pkt.data, &server->config, sizeof(PXPT_ServerConfig));
                pxpt_send(player->tcp, &cfg_pkt, sizeof(cfg_pkt));
            }

            printf(
                "Player %d logged in: %s\n",
                i,
                player->name
            );

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
                sizeof(PXPT_PlayerState)
            )
                continue;

            PXPT_PlayerState* state =
                (PXPT_PlayerState*)packet.data;

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
                sizeof(PXPT_BombExplosion)
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

                    pxpt_send(
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
                sizeof(PXPT_BombPlaced)
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

                    pxpt_send(
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
    PXPT_Server* server
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
            PXPT_INVALID_SOCKET
        )
        {

            pxpt_socket_close(
                server->players[i].tcp
            );
            server->players[i].tcp =
                PXPT_INVALID_SOCKET;

        }

    }

    pxpt_socket_close(
        server->listen_socket
    );

    server->listen_socket = PXPT_INVALID_SOCKET;
    pxpt_socket_cleanup();

}
