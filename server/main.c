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



    for(
        int i=1;
        i<argc;
        i++
    )
    {


        if(
            strcmp(argv[i], "-ip")==0
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
            strcmp(argv[i], "-port")==0
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
            strcmp(argv[i], "-max")==0
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
            strcmp(argv[i], "-help")==0
        )
        {

            printf(
                "\nPXPT Racer Server\n\n"
                "-ip <address>\n"
                "-port <port>\n"
                "-max <players>\n\n"
            );


            return 0;

        }


    }



    printf(
        "\n=====================\n"
        " PXPT Racer Server\n"
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




    PXPT_Server server;



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