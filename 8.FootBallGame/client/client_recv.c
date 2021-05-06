/*************************************************************************
	> File Name: client_recv.c
	> Author: Aiiii
	> Mail: 2646967818@qq.com
	> Created Time: Sun 01 Nov 2020 08:38:37 PM CST
 ************************************************************************/

#include"head.h"
extern int sockfd;

void *client_recv(void *arg) {
    while (1) {
        struct FootBallMsg msg;
        struct Player player;
        bzero(&msg, sizeof(msg));
        recv(sockfd, (void *)&msg, sizeof(msg), 0);
        strcpy(player.name, msg.name);
        player.team = msg.team;
        if (msg.type & FT_HEART) {
            DBG(RED"HeartBeat from Server \n" NONE);
            msg.type = FT_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
        } else if (msg.type & FT_MSG) {
            DBG(GREEN"Server Msg" NONE "%s\n", msg.msg);
            //Show_Message
        } else if (msg.type & FT_WALL) {
            //Show_Message
        } else if (msg.type & FT_FIN) {
            DBG(GREEN"Server is going to stop.\n"NONE);
            close(sockfd);
            endwin();
            exit(EXIT_SUCCESS);
        } else if (msg.type & FT_MAP) {
            //Show_Message
            // parse_spirit(msg.msg, msg.size);
        }

    }
}
