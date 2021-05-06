/*************************************************************************
	> File Name: ../../../common/client_recv.c
	> Author: 
	> Mail: 
	> Created Time: Wed 19 Aug 2020 04:39:00 PM CST
 ************************************************************************/

#include <head.h>
extern int sockfd;
extern WINDOW *msg_win, *sub_msg_win, *input_win, *sub_input_win, *help_win, *sub_help_win;

void *client_recv(void *arg) {
    struct ChatMsg msg;
    while (1) {
        bzero(&msg, sizeof(msg));
        if (recv(sockfd, (void *)&msg, sizeof(msg), 0) <= 0) {
            perror("client_recv()");
            exit(EXIT_FAILURE);
        }
        if (msg.type & CHAT_NAK) {
            //printf(L_PINK"SERVER INFO : " NONE " :  %s\n", msg.msg);
            print_message(sub_msg_win, &msg, 0);
        } else if (msg.type & CHAT_PUB) {
            //printf(BLUE"%s" NONE " : %s", msg.name, msg.msg);
            print_message(sub_msg_win, &msg, 0);
        } else if (msg.type & CHAT_HEART) {
            //DBG(RED"❤❤"NONE " : heart beat from server!\n");
            msg.type = CHAT_ACK;
            send(sockfd, (void *)&msg, sizeof(msg), 0);
        } else if (msg.type & CHAT_PRI) {
            //printf(BLUE"%s" GREEN"*" NONE " : %s\n", msg.name, msg.msg);
            print_message(sub_msg_win, &msg, 0);
        } else if (msg.type & CHAT_SYS) {
            //printf(L_PINK"%s" GREEN " : %s\n", "SERVER INFO :", msg.msg);
            print_message(sub_msg_win, &msg, 1);
        } else if (msg.type & (CHAT_FIN_1 | CHAT_FIN)) {
            sprintf(msg.msg, "Bye!");
            print_message(sub_msg_win, &msg, 1);
            //printf(GREEN"Bye!\n" NONE);
            exit(EXIT_FAILURE);
        }
    }
}
