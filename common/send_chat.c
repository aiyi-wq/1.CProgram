/*************************************************************************
	> File Name: ../common/send_chat.c
	> Author: Aiiii
	> Mail: 2646967818@qq.com
	> Created Time: Sun 01 Nov 2020 08:52:04 PM CST
 ************************************************************************/

#include"head.h"

extern int sockfd;
extern WINDOW *Write;
extern struct FootBallMsg chat_msg;

void send_chat() {
    echo();
    bzero(chat_msg.msg, sizeof(chat_msg.msg));
    w_gotoxy_putc(Write, 1, 1, "Input Message : ");
    mvwscanw(Write, 2, 1, "[^\n]s", chat_msg.msg);
    if (strlen(chat_msg.msg)) {
        send(sockfd, (void *)&chat_msg, sizeof(chat_msg), 0);
    }
    wclear(Write);
    box(Write, 0, 0);
    wrefresh(Write);
    noecho();
    cbreak();
}
