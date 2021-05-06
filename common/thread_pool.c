/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:22:16 PM CST
 ************************************************************************/

#include "thread_pool.h"

extern int epollfd;
extern int sockfd;
extern int maxfd;
extern struct User *users;
extern int cnt_online;

void send_all(struct ChatMsg *msg) {
    for (int i = 1; i <= maxfd; i++) {
        if (users[i].online == 1) {
            send(users[i].fd, (void *)&msg, sizeof(struct ChatMsg), 0);
        }
    }
}

void send_to_name(char *to, struct ChatMsg *msg, int fd) {
    int flag = 0;
    for (int i = 1; i <= maxfd; i++) {
        if (!strcmp(to, users[i].chat_name) && users[i].online) {
            send(users[i].fd,(void *)&msg, sizeof(struct ChatMsg) , 0);
            flag = 1;
            break;
        }
    }
    if (!flag) {
        memset(msg->msg, 0, sizeof(msg->msg));
        sprintf(msg->msg, "用户 %s 不在线或者格式错误！", to);
        msg->type = CHAT_SYS;
        send(fd, (void *)msg, sizeof(struct ChatMsg), 0);
    }
}

void send_to(int fd, struct ChatMsg *msg) {
    char name[20] = {0};
    int sub = 1;
    for (; sub <= 21; sub++) {
        if (msg->msg[sub] == ' ') break; 
    }  
    if (msg->msg[0] != '@' || msg->msg[sub] != ' ') {
        memset(msg, 0, sizeof(struct ChatMsg));
        msg->type = CHAT_SYS;
        sprintf(msg->msg, "私聊信息格式错误！");
        send(fd, (void *)msg, sizeof(struct ChatMsg), 0);
    } else {
        strncpy(name, msg->msg + 1, sub - 1);
        send_to_name(name, msg, fd);
    }
}
//#1:查询在线人数，并打印在线人数表
void get_name(int fd) {
    struct ChatMsg msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = CHAT_SYS;
    int cnt = 0;
    for (int i = 1; i <= maxfd; i++) {
        if (cnt > 10) break;
        if (users[i].online == 1) {
            cnt++;
            strcat(msg.msg, users[i].chat_name);
            strcat(msg.msg, ",");
        }
    }
    msg.msg[strlen(msg.msg) - 1] = ' ';
    char tmp[100];
    sprintf(tmp, "等 %d 位好友在线!", cnt_online);
    strcat(msg.msg, tmp);
    send(fd, (void *)&msg, sizeof(msg), 0);
}


void do_work(struct User *user) {
    struct ChatMsg msg;
    /*if (recv(user->fd, &msg, sizeof(struct ChatMsg), 0) <= 0) {
        DBG(L_PINK"thread_pool.c" NONE " : %s closed connection abnormally!\n", user->name);
        del_event(epollfd, user);
        cnt_online--;
        close(user->fd);
        return ;
    }*/
    recv(user->fd, &msg, sizeof(struct ChatMsg), 0);
    user->flag = 10;
    if (msg.type & CHAT_SYN) {
        do_login(user, &msg);
    } else if (msg.type & CHAT_PUB) {
        strcpy(msg.name, user->chat_name);
        printf(BLUE"%s" NONE " : %s\n", user->chat_name, msg.msg);
        send_all(&msg);
        DBG(GREEN"<PUB>" NONE " : has send public message!\n");
    } else if (msg.type & CHAT_PRI) { 
        strcpy(msg.name, user->chat_name);
        printf(PINK"%s" NONE " : %s *\n", user->chat_name, msg.msg);
        send_to(user->fd, &msg);
        DBG(GREEN"<PRI>" NONE " : has send private message!\n");
    } else if (msg.type & CHAT_FUNC) {
        if (msg.msg[0] != '#') {
            sprintf(msg.msg, "请使用#+数字的形式使用聊天室功能, #1:查询在线人数, #2:切换匿名！");
            msg.type = CHAT_SYS;
            send(user->fd, (void *)&msg, sizeof(struct ChatMsg), 0);
        } else if (msg.msg[1] == '1'){
            get_name(user->fd);
        } else if (msg.msg[1] == '2') {
            strcpy(user->chat_name, user->name);
        }
        DBG(GREEN"<FUNC>" NONE " : has send server message!\n");
    } else if (msg.type & CHAT_FIN) {
        user->flag = 10;
        DBG(L_PINK"Chat Fin" NONE " recv a CHAT_FIN from %s\n", user->chat_name);
        msg.type = CHAT_FIN_1;
        send(user->fd, (void *)&msg, sizeof(msg), 0);
        DBG(L_PINK"Chat Fin" NONE " :sent a CHAT_FIN from %s\n", user->chat_name);
        close(user->fd);
        user->online = 0;
        del_event(epollfd, user);
        cnt_online--;
    }
}

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd) {
    taskQueue->size = size;
    taskQueue->epollfd = epollfd;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->users = calloc(size, sizeof(struct User));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return ;
}

void task_queue_push(struct task_queue *taskQueue, struct User *user) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        DBG(YELLOW"<taskQueue>"NONE" : taskQueue is full!\n");
        return;
    }
    taskQueue->users[taskQueue->tail] = user;
    taskQueue->total++;
    DBG(GREEN"<Push>"NONE" : %s\n", user->name);
    if (++taskQueue->tail == taskQueue->size) {
        DBG(RED"<taskQueue>"NONE" : End\n");
        taskQueue->tail = 0;
    }
    //sleep(1);
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct User *task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    //惊群效应
    while (taskQueue->total == 0) {
        DBG(PINK"<Debug>"NONE" : taskQueue is empty!\n ");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    struct User *user = taskQueue->users[taskQueue->head];
    taskQueue->total--;
    DBG(BLUE"<taskQueue>"NONE" : pop %s!\n", user->name);
    if (++taskQueue->head == taskQueue->size) {
        taskQueue->head = 0;
    }
   // sleep(1);
    pthread_mutex_unlock(&taskQueue->mutex);
    return user;
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct User *user = task_queue_pop(taskQueue);
        do_work(user);
    }
}
