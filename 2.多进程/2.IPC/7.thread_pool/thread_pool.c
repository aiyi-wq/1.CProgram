/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:22:16 PM CST
 ************************************************************************/

#include "head.h"
#include "thread_pool.h"

extern int epollfd;

void do_work(int fd) {
    char buff[512] = {0};
    if (recv(fd, buff, sizeof(buff), 0) <= 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        DBG(RED"<Reactor>"NONE" : Del from reactor!\n");
        close(fd);
        return ;
    } 
    printf("Recv : %s", buff);
}

void task_queue_init(struct task_queue *taskQueue, int size) {
    taskQueue->size = size;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->fd = calloc(size, sizeof(int));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return ;
}

void task_queue_push(struct task_queue *taskQueue, int fd) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        DBG(YELLOW"<taskQueue>"NONE" : taskQueue is full!\n");
        return;
    }
    taskQueue->fd[taskQueue->tail] = fd;
    taskQueue->total++;
    DBG(GREEN"<Push>"NONE" : %d\n", fd);
    if (++taskQueue->tail == taskQueue->size) {
        DBG(RED"<taskQueue>"NONE" : End\n");
        taskQueue->tail = 0;
    }
    //sleep(1);
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

int task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    //惊群效应
    while (taskQueue->total == 0) {
        DBG(PINK"<Debug>"NONE" : taskQueue is empty!\n ");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    int fd = taskQueue->fd[taskQueue->head];
    taskQueue->total--;
    DBG(BLUE"<taskQueue>"NONE" : pop %d!\n", fd);
    if (++taskQueue->head == taskQueue->size) {
        taskQueue->head = 0;
    }
   // sleep(1);
    pthread_mutex_unlock(&taskQueue->mutex);
    return fd;
}



void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        int fd = task_queue_pop(taskQueue);
        do_work(fd);
    }
}
