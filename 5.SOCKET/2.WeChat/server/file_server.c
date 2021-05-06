/*************************************************************************
	> File Name: file_server.c
	> Author: 
	> Mail: 
	> Created Time: Sun 20 Sep 2020 07:28:07 PM CST
 ************************************************************************/

#include"head.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int server_listen, port, fd;
    port = atoi(argv[1]);
    if ((server_listen = sockfd_create(port)) < 0) {
        perror("sockfd_create");
        exit(EXIT_FAILURE);
    }
    while (1) {
        if (fd = accept(server_listen, NULL, NULL) < 0) {
            perror("Accept");
            exit(EXIT_FAILURE);
        }
        if ((pid = fork() < 0)) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            close(server_listen);
            recv_file(fd);
            exit(EXIT_SUCCESS);
        } else {
            close(fd);
        }
    }
    return 0;
}
