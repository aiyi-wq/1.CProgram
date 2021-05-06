/*************************************************************************
	> File Name: 1.fork_io_worker.c
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Aug 2020 02:36:06 PM CST
 ************************************************************************/
#include<head.h>

void worker(int fd) {
    while (1) {
        char buff[512] = {0};
        if ((recv(fd, buff, sizeof(buff), 0)) <= 0) {
            break;
        }
    }
    close(fd);
    DBG("Logout!\n");
} 

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_listen, sockfd, port;
    port = atoi(argv[1]);
    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        if ((sockfd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }
        
        if (pid = fork() < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            worker(sockfd);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
