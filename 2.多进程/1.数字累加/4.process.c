/*************************************************************************
	> File Name: 4.process.c
	> Author: 
	> Mail: 
	> Created Time: Sat 01 Aug 2020 08:15:26 PM CST
 ************************************************************************/

#include<head.h>

#define INS 5
int now = 0, sum = 0;

int main() {
    pid_t pid;
    FILE *fp;
    int x = 0;
    for (int i = 0; i < INS; i++) {
        if ((pid = fork()) < 0) {
            perror("fork()");
            exit(1);
        }
        x = i;
        if (pid == 0) break;
    }

    if (pid == 0) {
        while (1) {
            if (now >= 100) break;
            now++;
            sum += now;
            flock(now, sum);
        }
        //DBG(GREEN"<Debug>" NONE " : 我是 %d 个子进程\n", x);
        while ((fp = fopen(, "w")) != NULL) {
            
            flock(now, sum);
        }
    } else {
        for (int i = 0; i < INS; i++) {
            wait(NULL);
        }
        flock(now, sum);
    }
    printf("sum = %d\n", sum);
    flose(fp);
    return 0;
}
