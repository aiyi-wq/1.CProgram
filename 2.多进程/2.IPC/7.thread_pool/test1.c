/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:25:45 PM CST
 ************************************************************************/

#include "thread_pool.h"
#define SIZE 50
#define THREADNUM 20
#define BUFFSIZE 512

int main() {
    pthread_t tid[THREADNUM];
    Queue *taskQueue = task_queue_init(SIZE);

    for (int i = 0; i < THREADNUM; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }

    char buff[BUFFSIZE][BUFFSIZE] = {0};
    int sub = 0;
    while (1) {
        FILE *fp;
        fp = fopen("./thread_pool.c", "r");
        //scanf("%[^\n]s", buff[sub]);
        //getchar();
        //DBG(GREEN"<Debug>" NONE " : start to do tasks!\n");
        if (fp == NULL) {
            perror("fopen()");
            exit(1);
        }//压力测
        while (fgets(buff[sub], BUFFSIZE, fp) != NULL) {
            //sleep(1);
            task_queue_push(taskQueue, buff[sub]);
            if (++sub >= SIZE) {
                sub = 0;
            }
        }
        fclose(fp);
    }
    return 0;
}
