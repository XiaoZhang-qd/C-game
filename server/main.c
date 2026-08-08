#include "server.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(
    int argc,
    char** argv
)
{

    char ip[64] =
        "0.0.0.0";

    int port =
        25565;

    int max_players =
        8;

    int bomb_cooldown = 30;
    int skill1_cooldown = 300;
    int skill2_cooldown = 300;



    for(
        int i=1;
        i<argc;
        i++
    )
    {


        if(
            (strcmp(argv[i], "-ip")==0 || strcmp(argv[i], "--ip")==0)
            &&
            i+1<argc
        )
        {

            strcpy(
                ip,
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-port")==0 || strcmp(argv[i], "--port")==0)
            &&
            i+1<argc
        )
        {

            port =
            atoi(
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-max")==0 || strcmp(argv[i], "--max")==0)
            &&
            i+1<argc
        )
        {

            max_players =
            atoi(
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-bomb")==0 || strcmp(argv[i], "--bomb")==0)
            &&
            i+1<argc
        )
        {

            bomb_cooldown =
            atoi(
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-skill1")==0 || strcmp(argv[i], "--skill1")==0)
            &&
            i+1<argc
        )
        {

            skill1_cooldown =
            atoi(
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-skill2")==0 || strcmp(argv[i], "--skill2")==0)
            &&
            i+1<argc
        )
        {

            skill2_cooldown =
            atoi(
                argv[++i]
            );

        }


        else if(
            (strcmp(argv[i], "-help")==0 || strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0)
        )
        {

            printf(
                "\nCGame Racer Server\n\n"
                "-ip <address>        Server IP (default: 0.0.0.0)\n"
                "-port <port>        Server port (default: 25565)\n"
                "-max <players>      Max players (default: 8)\n"
                "-bomb <cooldown>    Bomb cooldown in frames (default: 30)\n"
                "-skill1 <cooldown>  Skill1 cooldown in frames (default: 300)\n"
                "-skill2 <cooldown>  Skill2 cooldown in frames (default: 300)\n\n"
            );


            return 0;

        }


    }



    printf(
        "\n=====================\n"
        " CGame Racer Server\n"
        "=====================\n"
    );


    printf(
        "IP   : %s\n",
        ip
    );


    printf(
        "PORT : %d\n",
        port
    );


    printf(
        "MAX  : %d\n",
        max_players
    );

    printf(
        "BOMB CD : %d frames (%.1fs)\n",
        bomb_cooldown,
        (float)bomb_cooldown / 60.0f
    );

    printf(
        "SKILL1 CD: %d frames (%.1fs)\n",
        skill1_cooldown,
        (float)skill1_cooldown / 60.0f
    );

    printf(
        "SKILL2 CD: %d frames (%.1fs)\n",
        skill2_cooldown,
        (float)skill2_cooldown / 60.0f
    );



    CGAME_Server server;



    if(
        server_start(
            &server,
            ip,
            port,
            max_players
        )
        !=0
    )
    {

        printf(
            "Server start failed\n"
        );


        return -1;

    }

    server.config.bomb_cooldown = bomb_cooldown;
    server.config.skill1_cooldown = skill1_cooldown;
    server.config.skill2_cooldown = skill2_cooldown;



    printf(
        "\nServer started!\n"
    );


    printf(
        "Waiting for players...\n\n"
    );



    while(1)
    {

        server_update(
            &server
        );

    }



    server_stop(
        &server
    );



    return 0;

}