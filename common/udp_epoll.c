/*************************************************************************
	> File Name: udp_epoll.c
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 08:55:02 PM CST
 ************************************************************************/
#include"udp_epoll.h"
#include"head.h"

#define handle_error(msg)\
    do{perror(msg); exit(EXIT_FAILURE);} while (0);

extern int port;
extern struct Player *bteam, *rteam;
pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
extern int repollfd, bepollfd;

void add_event_ptr(int epollfd, int fd, int events, struct Player *player) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    //调用epoll_ctl将fd加到epollfd中
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        handle_error("epoll_ctl()");
    }
    ev.data.ptr = player;
    return ;
}

void del_event(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    return ;
}

int find_sub(struct Player *team) {
    for (int i = 0; i < MAX; ++i) {
        if (!team[i].online) return i;
    }
    return -1;
}

//与服务器创建连接
int udp_connect(struct sockaddr_in *client) {
    int sockfd;
    if ((sockfd = socket_create_udp(port)) < 0) {
        handle_error("socket_create_udp()");
    }
    if (connect(sockfd, (struct sockaddr *)client, sizeof(struct sockaddr)) < 0) {
        return -1;
    }
    return sockfd;
}

//判断用户是否重复登录
int check_online(struct LogRequest *request, int fd) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(server);
    for (int i = 0; i < MAX; ++i) {
        //判断是否有重名用户,有的话返回1
        if (request->team == 0) {
            if (strcmp(rteam[i]->name, request->name) == 0) {
                sprintf(request->msg, "this name [:%s] has been used!\n", rteam[i]->name);
                sendto(fd, request, sizeof(request), 0, (struct sockaddr *)&server, len);
                return 1;
            } else {
                sprintf(request->msg, "name has registered successfully..\n");
                sendto(fd, request, sizeof(request), 0, (struct sockaddr *)&server, len);
                break;
            }
        } else {
            if (strcmp(bteam[i]->name, request->name) == 0) {
                sprintf(request->msg, "this name : has been used!\n", bteam[i]->name);
                sendto(fd, request, sizeof(request), 0, (struct sockaddr *)&server, len);
                return 1;
            } else {
                sprintf(request->msg, "name has registered successfully..\n");
                sendto(fd, request, sizeof(request), 0, (struct sockaddr *)&server, len);
                break;
            }
        }
    }
    return 0;
}

int udp_accept(int fd, struct Player *player) {
    int new_fd, ret;
    struct sockaddr_in client;
    struct LogRequest request;
    struct LogResponse response;
    socklen_t len = sizeof(client);
    memset(&request, 0, sizeof(struct LogRequest));
    memset(&response, 0, sizeof(struct LogResponse));
    ret = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);

    if (ret != sizeof(request)) {
        response.type = 1;
        strcpy(response.msg, "Login failed with Data errors!");
        sento(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }
    if (check_online(&request)) {
        response.type = 1;
        strcpy(response.msg, "You are already login!\n");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len)
        return -1;
    }

    if (request.team) {
        DBG(GREEN"Info" NONE " : "BLUE"%s Login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    } else {
        DBG(GREEN"Info" NONE " : "RED"%s Login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    }
    strcpy(player->ip, inet_ntoa(client.sin_addr));
    strcpy(player->name, request.name);
    if (new_fd == -1) return -1;
    player->team = request.team;
    player->score = 0;
    bzero(player->test, sizeof(player->test));

    new_fd = udp_connect(&client);
    player->fd = new_fd;

    response.type = 0;
    send(new_fd, (void *)&response, sizeof(response), 0);
    char buff[512] = {0};
    sprintf(buff, "%s<%s>Login success, enjoy yourself! You have get %d for this game!\n", player->real_name, player->id, player->score);
    strcpy(response.msg, buff);
    check(1, player, 9);
    return new_fd;
}


void add_to_sub_reactor(struct Player *player) {
    struct Player *team = (player->team ? bteam : rteam);
    DBG(BLUE"Main thread :" NONE " Add to sub_reactor\n");
    if (player->team) {
        pthread_mutex_lock(&bmutex);
    } else {
        pthread_mutex_lock(&rmutex);
    }
    int sub = find_sub(team);
    team[sub] = *player;
    team[sub].online = 1;
    team[sub].flag = 10;
    team[sub].loc.y = court.height + 1;
    if (player->team) {
        team[sub].loc.x = (court.width + 4) / 2 + 2 + 10 + sub;
    } else {
        team[sub].loc.x = (court.width + 4) / 2 + 2 - 10 - sub;
    }
    strcpy(team[sub].chat_name, team[sub].real_name);
    struct FootBallMsg r_msg;
    bzero(r_msg.msg, sizeof(r_msg.msg));
    sprintf(r_msg.msg, "你的好友 %s 上线了，快打个招呼吧!\n", player->real_name);
    r_msg.type = FT_WALL;
    send_all(&r_msg);
    check(1, player, 4);
    if (player->team) {
        pthread_mutex_unlock(&bmutex);
    } else {
        pthread_mutex_unlock(&rmutex);
    }
    DBG(L_RED"sub = %d, name = %s\n" NONE, sub, team[sub].name)
    if (player->team) {
        add_event_ptr(bepollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
    } else {
        add_event_ptr(repollfd, team[sub].fd, EPOLLIN | EPOLLET, &team[sub]);
    }
    int epollfd;
    struct epoll_event ev;
    ev.data.fd = player->fd;
    ev.events = EPOLLIN | EPOLLET; //边缘触发
    //创建反应堆
    if ((epollfd = epoll_create(1)) < 0) {
        handle_error("epoll_create()");
    }
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, player->fd, &ev) < 0) {
        handle_error("epoll_ctl()");
    }
}

