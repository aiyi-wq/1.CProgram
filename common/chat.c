/*************************************************************************
	> File Name: chat.c
	> Author: 
	> Mail: 
	> Created Time: Wed 19 Aug 2020 02:47:27 PM CST
 ************************************************************************/

#include<head.h>

extern struct User *users;
extern int epollfd;
extern int maxfd;
extern int sockfd;//客户端
extern int cnt_online;

void add_event(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *)user;
    DBG(PINK"Main Reactor" NONE " : adding %s to main reactor\n", user->name);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }
}

void del_event(int epollfd, struct User *user) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, user->fd, NULL);
    DBG(PINK"Main Reactor" NONE " : deleted %s from main reactor\n", user->name);
}

int find_sub(struct User *user, int max) {
    for (int i = 1; i < max; i++) {
        if (!user[i].online) {
            return i;
        }
    }
    return -1;
}

int do_login(struct User *user, struct ChatMsg *msg) {
    int online = 0;
    for (int i = 1; i <= maxfd; i++) {
        if (users[i].online && !strcmp(msg->name, users[i].name)) {
            online = 1;
        }
    }   
    if (online) {
        DBG(YELLOW"Login Error" NONE " : %s is already online\n", msg->name);
        sprintf(msg->msg, "%s is already online", msg->name);
        msg->type = CHAT_NAK;
        user->online = 0;
        send(user->fd, msg, sizeof(struct ChatMsg), 0);
        close(user->fd);
        del_event(epollfd, user);
        return 1;
    } else {
        sprintf(msg->msg, "%s you have login success!\n", msg->name);
        msg->type = CHAT_ACK;
        send(user->fd, msg, sizeof(struct ChatMsg), 0);
        user->online = 1;
        user->flag = 10;
        strcpy(user->real_name, msg->name);
        strcpy(user->name, msg->name);
        cnt_online++;
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        getpeername(user->fd, (struct sockaddr *)&client, &len);
        strcpy(user->ip, inet_ntoa(client.sin_addr));
        DBG(L_GREEN"Login Success" NONE " %s has real name %s, system defualtly use real_name!\n", user->name, user->real_name);
        strcpy(user->chat_name, user->real_name);
        return 0;
    }
}
void *heart_beat(void *arg) {
    struct ChatMsg msg;
    msg.type = CHAT_HEART;
    while (1) {
        sleep(5);
        for (int i = 1; i <= maxfd; i++) {
            if (users[i].online == 1) {
                send(users[i].fd, (void *)&msg, sizeof(msg), 0);
                if (--users[i].flag == 0) {
                    DBG(PINK"Heart_Beat" NONE " : %s is offline by heart_beat!\n", users[i].name) ;
                    del_event(epollfd, &users[i]);
                    close(users[i].fd);
                    users[i].online = 0;
                    cnt_online--;
                }
            } 
        }
    }
}
