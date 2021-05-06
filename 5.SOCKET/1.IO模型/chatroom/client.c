/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Sun 09 Aug 2020 04:23:26 PM CST
 ************************************************************************/

#include<head.h>
#include "../thread_pool/thread_pool.h"

int sockfd;
struct ChatMsg chatmsg1;
void stop(int signum) {
    chatmsg1.type |= CHAT_FIN;
    send(sockfd, &chatmsg1, sizeof(chatmsg1), 0);
}

int main(int argc, char **argv) {
    int opt, port;
    struct ChatMsg chatmsg;
    char ip[20];
    while ((opt = getopt(argc, argv, "h:p:n::")) != -1) {
        switch(opt) {
            case 'h':
                strcpy(ip, argv[2]);;
                break;
            case 'p':
                port = atoi(argv[4]);
                break;
            case 'n':
                strcpy(chatmsg.name, argv[6]);
                break;
            default:
                fprintf(stderr, "Usage : %s -h ip -p port -n [Name]!\n", argv[0]);
                exit(EXIT_FAILURE);
        } 
    }


    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        exit(EXIT_FAILURE);
    }

    chatmsg.type |= CHAT_SYN;
    send(sockfd, &chatmsg, sizeof(chatmsg), 0);
    if (recv(sockfd, &chatmsg, sizeof(chatmsg), 0) <= 0 ) {
        perror("recv()");
        exit(EXIT_FAILURE);
    }
    if (chatmsg.type & CHAT_NAK) {
        perror("this name has been used");
        exit(EXIT_FAILURE);
    } 
    printf(PINK"<Server>" NONE " : Welcom to chatroom!\n");
    while (1) {
        scanf("%[^\n]s", chatmsg.msg);
        getchar();
        send(sockfd, chatmsg.msg, sizeof(chatmsg.msg), 0);
    }
    signal(SIGINT, stop);
    if (recv(sockfd, &chatmsg1, sizeof(chatmsg1), 0) > 0) {
        DBG(GREEN"<After signal>" NONE " : %d\n", chatmsg1.type & CHAT_FIN1);
        if (chatmsg1.type & CHAT_FIN1) {
            printf("Logout!\n");
            exit(EXIT_FAILURE);
        }
    }
}
