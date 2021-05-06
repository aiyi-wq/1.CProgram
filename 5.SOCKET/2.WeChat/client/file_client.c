/*************************************************************************
	> File Name: file_client.c
	> Author: 
	> Mail: 
	> Created Time: Sun 20 Sep 2020 07:29:30 PM CST
 ************************************************************************/

#include"head.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ip port path_of_file!\n", argv[0]);
        return 1;
    }
    int sockfd, port;
    port = atoi(argv[2]);
    char buff[1024] = {0}, name[1024] = {0};
    if ((socket_connect(argv[1], port)) < 0) {
        perror("sockfd_connect()");
        return 1;
    }
    while (1) {
        scanf("%[^\n]s", buff);
        getchar();
        if (!strcmp("send", buff, 5)) {
            strcpy(name, buff + 5);
        } else {
            fprintf(stderr, "invalid commad!");
        }
        send_file(name, sockfd);
    }
    close(sockfd);
    return 0;
}
