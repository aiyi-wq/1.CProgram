/*************************************************************************
	> File Name: sub_reactor.c
	> Author: 
	> Mail: 
	> Created Time: Thu 22 Oct 2020 01:53:33 PM CST
 ************************************************************************/
#include"sub_reactor.h"

#define handle_error(msg) \
    do{perror(msg); exit(EXIT_FAILURE);} while(0);

/******************
从反应堆，创建线程，并通过epoll将就绪事件对应的用户加入到反应堆中
*********************/
extern epollfd;
extern struct Player *rteam, *bteam;

void *sub_reactor(void *arg) {
    struct task_queue *taskQueue = (struct task_queue *)arg;
    pthread_t *tid = (pthread_t *)calloc(NTHREAD, sizeof(pthread_t));
    for (int i = 0; i < NTHREAD; ++i) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    };
    DBG(GREEN"INFO" NONE " : sub_reactor created thread successfully...\n");
    struct epoll_event ev, events[MAX];
    sigset_t mask, orig_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    while (1) {
        DBG(L_RED"Sub Reactor" NONE " : Epoll Waiting...\n");
        pthread_sigmask(SIG_SETMASK, &mask, &orig_mask);
        //将返回的事件所关联到的user, push 到相应的任务队列
        int nfds = epoll_wait(taskQueue->epollfd, events, MAX, -1);
        if (nfds < 0) {
            handle_error("epoll_wait()");
        }
        DBG(YELLOW"<Debug>" NONE " : After wait nfds = %d\n", nfds);
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == rteam->fd && (events[i].events & EPOLLIN)) {
                //do something
            } else {
                task_queue_push(&taskQueue, events[i].data.fd);
            }
        }
    }
    return NULL;
}
