/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Wed 19 Aug 2020 02:23:44 PM CST
 ************************************************************************/

#include<head.h>

char *conf = "./chat.conf";
int epollfd;
int maxfd = 0;
struct User *users;
int cnt_online = 0;

int main(int argc, char **argv) {
    int opt, port = 0, server_listen, sockfd;
    pthread_t heart_t;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(argv[2]);
                break;
            default:
                fprintf(stderr, "Usage :%s -p port!\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (!port) port = atoi(get_conf(conf, "PORT"));
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(EXIT_FAILURE);
    }
    make_non_block(sockfd);
    DBG(GREEN"INFO" NONE " : server started on port %d\n", port);

    DBG(GREEN"INFO" NONE " : calloced memory for storing users!\n");
    if ((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create()");
        exit(1);
    }
    DBG(GREEN"INFO" NONE " : main reactor created!\n");

    struct task_queue taskQueue;
    task_queue_init(&taskQueue, MAXUSER, epollfd);
    pthread_t *tid = (pthread_t *)calloc(THREADNUM, sizeof(pthread_t));
    for (int i = 0; i < THREADNUM; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)&taskQueue);
    }
    DBG(GREEN"INFO" NONE " : Thread pool Created for worker\n");
    users = (struct User *)calloc(MAXUSER, sizeof(struct User));

    users[0].fd = server_listen;//不存别的信息
    strcpy(users[0].name, "server_listen");
    strcpy(users[0].real_name, "server_listen");
    add_event(epollfd, server_listen, EPOLLIN, &users[0]);

    pthread_create(&heart_t, NULL, heart_beat, NULL);
    struct epoll_event events[MAXEVENTS];
    while (1) {
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; i++) {
            struct User *user = (struct User *)events[i].data.ptr;
            if (user->fd == server_listen && ((events[i].events & EPOLLIN))) {
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept()");
                    exit(EXIT_FAILURE);
                }
                make_non_block(sockfd);
                if (sockfd > maxfd) maxfd = sockfd;
                DBG(L_GREEN"Acceptor" NONE " : Accept a new client!\n");
                struct User newuser;//新用户处于半登陆状态
                memset(&newuser, 0, sizeof(newuser));
                newuser.fd = sockfd;
                newuser.online = 2;
                newuser.flag = 10;
                strcpy(newuser.name, "New Client");
                int sub = find_sub(users, MAXUSER);
                if (sub < 0) {
                    DBG(YELLOW"Warning" NONE " : Too many clients!\n");
                }
                users[sub] = newuser;
                add_event(epollfd, sockfd, EPOLLIN | EPOLLET, &users[sub]);
                task_queue_push(&taskQueue, &users[sub]);
            } else {
                if (events[i].events & EPOLLIN) {
                    task_queue_push(&taskQueue, user);
                }
            }
        }
    }
    return 0;
}
