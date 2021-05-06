/*************************************************************************
	> File Name: 3.fork_100.c
	> Author: 
	> Mail: 
	> Created Time: Sat 01 Aug 2020 03:08:54 PM CST
 ************************************************************************/

#include<head.h>

int main() {
    pid_t pid;
    int x = 0;
    for (int i = 0; i < 100; ++i) {
        if ((pid = fork()) < 0) {
            perror("fork()");
            exit(1);
        }
        x = i;
        if (pid == 0) break;
    }

    if (pid == 0) {
        DBG(GREEN"<Debug>" NONE " : 我是 %d 个孩子\n", x);
    } else {
        for (int i = 0; i < 100; i++) {
            wait(NULL);
        }
    }
    return 0;
}
