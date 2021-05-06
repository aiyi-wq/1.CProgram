/*************************************************************************
	> File Name: 1.thread.c
	> Author: 
	> Mail: 
	> Created Time: Sat 01 Aug 2020 06:19:49 PM CST
 ************************************************************************/

#include<head.h>

void *print(void *arg) {
    //int tmp = *(int *)arg;
    //DBG(GREEN"<Debug>" NONE " : 我是第  %d   个子线程！\n", tmp);
    DBG(GREEN"<Debug>" NONE " : 我是第  %d   个子线程！\n", *(int *)arg);
}

int main(int argc, char **argv) {
    int tmp_arg[100] = {0};
    for (int i = 0; i < 100; i++) {
        pthread_t tid;
        tmp_arg[i] = i;
        pthread_create(&tid[i], NULL, print, (void *)&tmp_arg[i]);
    }
    
    for (int i = 0; i < 100; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
