/*************************************************************************
	> File Name: thread_pool.h
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:16:52 PM CST
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#define CHAT_SYN 0x01
#define CHAT_FIN 0x02
#define CHAT_ACK 0x04
#define CHAT_HEART 0x08
#define CHAT_PRI 0x10
#define CHAT_PUB 0x20
#define CHAT_FUNC 0x40
#define CHAT_SYS 0x80
#define CHAT_NAK 0x100
#define CHAT_FIN1 0x200

#define FUNC_CHECK_ONLINE 0x01
#define FUNC_CHANGE_NAME 0x02

#define MAXUSER 100
struct User {
    char name[20];
    char chat_name[20];
    char real_name[20];
    int flag;//可心跳的次数，默认设置为10
    char ip[20];
    int fd;
    int online;//1在线，0不在线
};

//struct ChatMsg chatmsg;
//struct User user;

struct ChatMsg {
    int type;
    int opt;
    char name[20];
    char msg[1024];
};
struct task_queue{
    int size;
    int total;
    int head;
    int tail;
    int *fd;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int size);
void task_queue_push(struct task_queue *taskQueue, int fd);
int task_queue_pop(struct task_queue *taskQueue);
void *thread_run(void *arg);
#endif
