/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:25:45 PM CST
 ************************************************************************/

#include "4.reactor_thread_pool.h"
#define SIZE 50
#define THREADNUM 20
#define MAXUSER 1024
#define MAXEVENTS 10

int epollfd;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port, server_listen, sockfd;
    int fd[MAXUSER]= {0};
    port = atoi(argv[1]);

    if (server_listen = socket_create(port) < 0) {
        perror("socket_create()");
        exit(EXIT_FAILURE);
    }
    pthread_t *tid = (pthread_t *)calloc(THREADNUM, sizeof(pthread_t));
    Queue *taskQueue = task_queue_init(SIZE);
    for (int i = 0; i < THREADNUM; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }

    if ((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create()");
        exit(EXIT_FAILURE);
    }
    struct epoll_event ev, events[MAXEVENTS];

    ev.data.fd = server_listen;
    ev.events = EPOLLIN;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    while (1) {
        //scanf("%[^\n]s", buff);
        //getchar();
        int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(EXIT_FAILURE);
        }
        DBG(YELLOW"<Debug>" NONE " : After wait nfds = %d\n", nfds);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_listen && (events[i].events && EPOLLIN)) {
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept()");
                    exit(EXIT_FAILURE);
                }   
                fd[sockfd] = sockfd;
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = sockfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                    perror("epoll_ctl()");
                    exit(EXIT_FAILURE);
                }
            } else {
                if (events[i].events & EPOLLIN) {
                    task_queue_push(taskQueue, events[i].data.fd);
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
