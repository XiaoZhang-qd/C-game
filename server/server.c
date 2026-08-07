#include "server.h"

#include <stdio.h>
#include <string.h>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif



/*
    发送单个玩家状态
*/

static void send_player_state(
    PXPT_Player* to,
    PXPT_Player* player
)
{

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



    memcpy(
        packet.data,
        &player->state,
        sizeof(PXPT_PlayerState)
    );



    pxpt_send(
        to->tcp,
        &packet,
        sizeof(packet.header)
        +
        packet.header.size
    );

}



/*
    广播所有玩家
*/

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
            !server->players[i].connected
        )
            continue;



        for(
            int j=0;
            j<server->max_players;
            j++
        )
        {


            if(
                i==j
            )
                continue;



            if(
                server->players[j].connected
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
        return -1;




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

        server->players[i].connected=0;

        server->players[i].id=i;

    }



    server->running=1;



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



        for(
            int i=0;
            i<server->max_players;
            i++
        )
        {



            if(
                !server->players[i].connected
            )
            {


                PXPT_Player* player =
                    &server->players[i];



                player->tcp =
                    client;



                player->connected =
                    1;



                player->id =
                    i;




                strcpy(
                    player->name,
                    "Player"
                );



                player->state.x =
                    100+i*50;


                player->state.y =
                    100;



                printf(
                    "Player %d connected\n",
                    i
                );



                break;


            }


        }


    }





    /*
        接收玩家数据
    */


    for(
        int i=0;
        i<server->max_players;
        i++
    )
    {


        PXPT_Player* player =
            &server->players[i];



        if(
            !player->connected
        )
            continue;




        PXPT_Packet packet;



        int r =
        recv(
            player->tcp,
            &packet,
            sizeof(packet),
            MSG_DONTWAIT
        );



        if(
            r>0
        )
        {


            if(
                packet.header.type
                ==
                PKT_LOGIN
            )
            {


                PXPT_Login* login =
                (PXPT_Login*)packet.data;



                strncpy(
                    player->name,
                    login->name,
                    31
                );



                printf(
                    "Player %d name: %s\n",
                    i,
                    player->name
                );


            }



        }


    }




    /*
        同步玩家
    */

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


    pxpt_socket_close(
        server->listen_socket
    );


    pxpt_socket_cleanup();


}