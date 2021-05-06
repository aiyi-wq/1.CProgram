/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:25:45 PM CST
 ************************************************************************/

#include "thread_pool.h"
#define SIZE 50
#define THREADNUM 5
#define BUFFSIZE 512

int main() {
    pthread_t tid[THREADNUM];
    Queue *taskQueue = task_queue_init(SIZE);

    for (int i = 0; i < THREADNUM; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }

    char buff[BUFFSIZE];
    memset(buff, 0, sizeof(buff));
    while (1) {
        scanf("%[^\n]s", buff);
        getchar();
        DBG(GREEN"<Debug>" NONE " : start to do tasks!\n");
        task_queue_push(taskQueue, buff);
    }
    return 0;
}
