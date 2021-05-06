/*************************************************************************
	> File Name: 3.one_thread_reactor.c
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Aug 2020 04:10:38 PM CST
 ************************************************************************/

#include<head.h>
#define MAX 10
#define MAXUSER 1024

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int server_listen, sockfd, port, epollfd;
    int fd[MAXUSER] = {0};//初始化
    port = atoi(argv[1]);
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(EXIT_FAILURE);
    }
    //创建反应堆
    if ((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create()");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev, events[MAX];
    ev.data.fd = server_listen;
    ev.events = EPOLLIN;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX, -1);//- 1 无限期阻塞
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_listen && (events[i].events && EPOLLIN)) {
                if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
                    perror("accept()");
                    exit(EXIT_FAILURE);
                }   
                fd[sockfd] = sockfd;
                ev.events = EPOLLIN;
                ev.data.fd = sockfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
                    perror("epoll_ctl()");
                    exit(EXIT_FAILURE);
                }
            } else {
                if (events[i].events & EPOLLIN) {
                    char buff[512] = {0};
                    memset(buff, 0, sizeof(buff));
                    int nrecv = recv(events[i].data.fd, buff, sizeof(buff), 0);
                    if (nrecv <= 0) break;
                    printf("fd = %d, recv : %s\n", events[i].data.fd, buff);
                    //exit(EXIT_SUCCESS);
                    //break;
                }

                //sleep(1);
                if (events[i].events & EPOLLRDHUP) {
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    DBG(GREEN"Debug" NONE " : fd = %d\n", events.data.fd);
                    close(events[i].data.fd);
                    printf("Logout!\n");
                }
                
            }
        }
    }
    return 0;
}
