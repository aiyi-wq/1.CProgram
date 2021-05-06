/*************************************************************************
	> File Name: 4.thread_pool_reactor.c
	> Author: 
	> Mail: 
	> Created Time: Sun 16 Aug 2020 10:43:55 AM CST
 ************************************************************************/
#include "head.h"
#include "../thread_pool/thread_pool.h"
#define MAXTHREAD 10
#define MAXQUEUE 20
#define MAXEVENTS 10

struct User user[MAXUSER];
struct ChatMsg chatmsg2;
int epollfd;
int sockfd;

void logout(int signum) {
    chatmsg2.type |= CHAT_FIN;
    for (int i = 5; user[i].real_name && i <= MAXUSER; i++) {
        if (user[i].online == 1) {
           // strcpy(chatmsg2.msg, "Server interface has closed!\n");
            send(user[i].fd, &chatmsg2, sizeof(chatmsg2), 0);
        }
    }
    printf("Server interface has closed!\n");
    exit(EXIT_FAILURE);
}

void delete(int signum) {
    if (recv(sockfd, &chatmsg2, sizeof(chatmsg2), 0) <= 0) return ;
    DBG(GREEN"<In delete>" NONE " : chatmsg2.type & CHAT_FIN = %d\n", chatmsg2.type & CHAT_FIN);
    if (chatmsg2.type & CHAT_FIN) {
        chatmsg2.type |= CHAT_FIN1;
        DBG(GREEN"<Before send>" NONE " : chatmsg2.type & CHAT_FIN1 = %d\n", chatmsg2.type & CHAT_FIN1);
        send(user[sockfd].fd, &chatmsg2, sizeof(chatmsg2), 0);
    }
}

void cmpname(struct ChatMsg *str, int fd) {
    for (int i = 5; user[i].real_name != NULL && i <= MAXUSER; i++) {
        if (strcmp(user[i].real_name, str->name) == 0) {
            str->type |= CHAT_NAK;
            send(fd, str, sizeof(str), 0);
            return ;
        } else {
            str->type |= CHAT_ACK;
            strcpy(user[fd].real_name, str->name);
            send(fd, str, sizeof(str), 0);
            break;
        }
    }
    return ;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(1);
    }
    int server_listen, port;
    port = atoi(argv[1]);
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(1);
    }

    struct task_queue taskQueue;
    task_queue_init(&taskQueue, MAXQUEUE);
    signal(SIGINT, logout);

    pthread_t *tid = (pthread_t *)calloc(MAXTHREAD, sizeof(pthread_t));

    for (int i = 0; i < MAXTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)&taskQueue);
    }

    if ((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create()");
        exit(1);
    }

    struct epoll_event ev, events[MAXEVENTS];
    ev.data.fd = server_listen;
    ev.events = EPOLLIN;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }
    
    while (1) {
        struct ChatMsg chatmsg;
        //memset(&chatmsg, 0, sizeof(chatmsg));
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }
        DBG(YELLOW"<Debg>"NONE" : After wait nfds = %d\n", nfds);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_listen  && (events[i].events & EPOLLIN)) {
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept()");
                    exit(1);
                }
                ev.events = EPOLLIN | EPOLLET; //边缘触发
                ev.data.fd = sockfd;
                if (recv(sockfd, &chatmsg, sizeof(chatmsg), 0) <= 0) {
                    perror("recv()");
                    exit(EXIT_FAILURE);
                }
                cmpname(&chatmsg, sockfd);
                memset(&chatmsg, 0, sizeof(chatmsg));
                user[sockfd].fd  = sockfd;
                user[sockfd].flag = 10;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                    perror("epoll_ctl()");
                    exit(1);
                }
            } else {
                if (events[i].events & EPOLLIN) {
                    signal(SIGINT, delete);
                    task_queue_push(&taskQueue, events[i].data.fd);
                }
                /*sleep(3);
                if (events[i].events & EPOLLHUP) {
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    DBG("events[i].data.fd = %d\n", events[i].data.fd);
                    close(events[i].data.fd);
                    printf("Logout!\n");
                }
                */
            }
        }
    }
    return 0;
}
