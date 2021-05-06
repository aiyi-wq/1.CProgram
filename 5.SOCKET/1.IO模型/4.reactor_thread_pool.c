/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:22:16 PM CST
 ************************************************************************/

#include "4.reactor_thread_pool.h"

extern int epollfd;

void do_work(int fd) {
    char buff[512] = {0};
    if (recv(fd, buff, sizeof(buff), 0) <= 0) {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        DBG(RED"<Reactor>" NONE " : Del from reactor!\n");
        close(fd);
        return ;
    }
    DBG(GREEN"<Debug>" NONE " : do_work()\n");
    printf("Recv : %s\n", buff);
    bzero(&buff, sizeof(buff));
}

Queue *task_queue_init(int size) {
    Queue *taskQueue = (Queue *)malloc(sizeof(Queue));
    taskQueue->size = size;
    taskQueue->fd = calloc(size, sizeof(int));
    taskQueue->head = taskQueue->tail = taskQueue->total = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return taskQueue;
}

void task_queue_push(Queue *taskQueue, int fd) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        DBG(YELLOW"<taskQueue>" NONE " : taskQueue is full!\n");
        return ;
    }
    taskQueue->fd[taskQueue->tail++] = fd;
    DBG(BLUE"<QueuePush>" NONE " : fd = %d\n", fd);
    if (taskQueue->tail == taskQueue->size) {
        DBG(BLUE"<Debug>" NONE " : queue is full!\n");
        taskQueue->tail = 0;
        //taskQueue->tail -= taskQueue->size;
    }
    taskQueue->total++;
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
    return ;
}

int task_queue_pop(Queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    while (taskQueue->total == 0) {
        //DBG(YELLOW"<Debug>" NONE " : queue is empty!\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    int fd = taskQueue->fd[taskQueue->head++];
    DBG(BLUE"<QueuePop>" NONE " : fd = %d\n", fd);
    if (taskQueue->head == taskQueue->size) {
        printf(RED"<PopEND>" NONE " : fd = %d\n", fd);
        taskQueue->head = 0;
        //taskQueue->head -= taskQueue->size;
    }
    taskQueue->total--;
    pthread_mutex_unlock(&taskQueue->mutex);
    return fd; 
}

void task_queue_clear(Queue *taskQueue) {
   if (taskQueue == NULL) return ;
    free(taskQueue->fd);
    free(taskQueue);
    return ;
}

int socket_create(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    int val = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    if (listen(sockfd, 10) < 0) {
        return -1;
    }
    return sockfd;
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    //线程的分离, pthread_self用来获取子线程的id
    Queue *taskQueue = (Queue *)arg;
    while (1) {
        int fd = task_queue_pop(taskQueue);
        do_work(fd);
    }
}
