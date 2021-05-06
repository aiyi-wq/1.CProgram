/*************************************************************************
	> File Name: 0.one_thread.c
	> Author: 
	> Mail: 
	> Created Time: Wed 12 Aug 2020 02:37:36 PM CST
 ************************************************************************/

#include<head.h>

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

    while (1) {
        struct sockaddr_in client;
        char buff[512] = {0};
        socklen_t len = sizeof(client);
        if ((sockfd = accept(server_listen, (struct sockaddr *)&client, &len)) < 0) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }
        read(sockfd, buff, sizeof(buff));
        printf(BLUE"<name>" NONE " : %s\n", buff);
        close(sockfd);
    }
    return 0;
}
