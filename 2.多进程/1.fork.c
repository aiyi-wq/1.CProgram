/*************************************************************************
	> File Name: 1.fork.c
	> Author: 
	> Mail: 
	> Created Time: Thu 30 Jul 2020 08:15:36 PM CST
 ************************************************************************/

#include <head.h>

int main() {
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }
    
    if (pid == 0) {
        printf("In Child Process!\n");
        execlp("/bin/ls", "ls", "/etc", ".", NULL);
        printf("Bye!\n");//不会执行,子进程已经变成ls
    } else {
        printf("In Parent Process!\n");
    }

    return 0;
}
