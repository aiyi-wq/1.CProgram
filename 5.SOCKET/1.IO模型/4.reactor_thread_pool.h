/*************************************************************************
	> File Name: thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:16:52 PM CST
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include<head.h>

//#define MAX 10

typedef struct task_queue {
    int size, total, head, tail;
    int *fd;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Queue;


Queue *task_queue_init(int size);
void task_queue_push(Queue *taskQueue, int fd);
int task_queue_pop(Queue *taskQueue);
void task_queue_clear(Queue *taskQueue);

//struct epoll_event ev, events[MAX];
void *thread_run(void *arg);
int socket_create(int port);

#endif
