/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Sun 09 Aug 2020 04:23:26 PM CST
 ************************************************************************/

#include<head.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int sockfd, port;
    char ip[20] = {0};
    strcpy(ip, argv[1]);
    port = atoi(argv[2]);

    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char buff[512] = {0};
        DBG(PINK"<Debug>" NONE "Please input:\n");
        scanf("%[^\n]s", buff);
        getchar();
        send(sockfd, buff, strlen(buff), 0);
        recv(sockfd, buff, strlen(buff), 0);
        printf("Server : %s\n", buff);
    }
}
