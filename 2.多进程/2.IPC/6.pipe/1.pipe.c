/*************************************************************************
	> File Name: 1.pipe.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 11:48:09 AM CST
 ************************************************************************/

#include<head.h>

int main(int argc, char **argv) {
    pid_t pid;
    int pipefd[2];
    pipe(pipefd);
    //memset(pipefd, 0, sizeof(pipefd));
    char buff[1024];
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }

    if (pid == 0) {
        close(pipefd[0]);
        while (1) {
            scanf("%s", buff);
            write(pipefd[1], buff, strlen(buff));
            memset(buff, 0, sizeof(buff));
        }
    } else {
        close(pipefd[1]);
        while (1) {
            read(pipefd[0], buff, sizeof(buff));
            printf("%s\n", buff);
            memset(buff, 0, sizeof(buff));
        }
    }

    return 0;
}
