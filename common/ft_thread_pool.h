/*************************************************************************
	> File Name: ft_thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: Sun 25 Oct 2020 02:12:23 PM CST
 ************************************************************************/

#ifndef _FT_THREAD_POOL_H
#define _FT_THREAD_POOL_H
#include<head.h>
struct task_queue {
    int size, total, head, tail;
    int epollfd; //从反应堆获取
    struct Player **team;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd);
void task_queue_push(struct task_queue *taskQueue, struct Player *player);
struct Player *task_queue_pop(struct task_queue *taskQueue, struct Player *player);
void *thread_run(void *arg);
#endif
