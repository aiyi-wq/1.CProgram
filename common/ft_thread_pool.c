/*************************************************************************
	> File Name: thread_pool.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 04:22:16 PM CST
 ************************************************************************/

#include "thread_pool.h"

extern int repollfd, bepollfd;
extern struct Player *bteam, *rteam;

/*void send_all(struct ChatMsg *msg) {
    for (int i = 1; i <= maxfd; i++) {
        if (users[i].online == 1) {
            send(users[i].fd, (void *)&msg, sizeof(struct ChatMsg), 0);
        }
    }
}*/

void do_with(struct Player *player) {
    struct FootBallMsg msg;
    bzero(&msg, sizeof(msg));
    char tmp[512] = {0};
    int size = recv(player->fd, (void)&msg, sizeof(msg), 0);
    player->flag = 10;
    if (msg.type & FT_ACK) {
        check(4, player, 9);
        show_data_stream('n');
        if (player->team) {
            DBG(L_BLUE" %s " NONE "❤\n", player->name);
        } else {
            DBG(L_RED" %s " NONE "❤\n", player->name);
        }
    } else if (msg.type & (FT_WALL | FT_MSG)) {
        check(6, player, 9);
        if (player->team) {
            DBG(L_BLUE" %s : %s\n" NONE, player->name, msg.msg);
        } else {
            DBG(L_RED" %s : %s\n" NONE, player->name, msg.msg);
        }
        strcpy(msg.name, player->name);
        msg.team = player->team;
        //Show_Message
        send_all(&msg);
    } else if (msg.type & FT_FIN) {
        check(7, player, 9);
        show_data_stream('n');
        DBG(RED"%s logout.\n", player->name);
        sprintf(tmp, "%s logout.", player->name);
        //Show_Message
        player->online = 0;
        int epollfd_tmp = (player->team ? bepollfd : repollfd);
        del_event(epollfd_tmp, player->fd);
    } else if (msg.type = FT_CTL) {
        sprintf(tmp, "Ctrl Message kick = %d", msg.ctl.strength);
        //Show_Message
        if (msg.ctl.action & ACTION_DFT) {
            check(8, player, 9);
            show_data_stream('n');
            player->loc.x += msg.ctl.dirx;
            player->loc.y += msg.ctl.diry;
            if (player->loc.x <= 1) player->loc.x = 1;
            if (player->loc.x >= court.width + 2) player->loc.x = court.width + 2;
            if (player->loc.y <= 0) player->loc.y = 0;
            if (player->loc.y >= court.height + 1) player->loc.y = court.height + 1;
        } else if (msg.ctl.action & ACTION_KICK) {
            check(9, player, 9);
            show_data_stream('k');
            sprintf(tmp, "bx = %lf, by = %lf, px = %d, py = %d", ball.x, ball.y, player->loc.x, player->loc.y);
            //Show_Message
            if (can_kick(&Player->loc, msg.ctl.strength)) {
                ball_status.by_team = player->team;
                strcpy(ball_status.team, player->name);
                sprintf(tmp, "vx = %f, vy = %f, ax = %f, ay = %f", ball_status.v.x, ball_status.v.y, ball_status.a.x, ball_status.a.y);
                //Show_Message
            }
        } else if (msg.ctl.action & ACTION_STOP) {
            check(2, player, 9);
            show_data_stream('s') ;
            if (can_access(&player->loc)) {
                bzero(&ball_status.v, sizeof(ball_status.v));
                bzero(&ball_status.a, sizeof(ball_status.a));
                sprintf(tmp, "Stop the Ball");
                //Show_Message
            }
        } else if (msg.ctl.action & ACTION_CARRY) {
            show_data_stream('c');
            sprintf(tmp, "Try Carry the Ball");
            //Show_Message
        }
    }
}

void task_queue_init(struct task_queue *taskQueue, int size, int epollfd) {
    taskQueue->size = size;
    taskQueue->epollfd = epollfd;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    taskQueue->team = calloc(size, sizeof(struct Player));
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    return ;
}

void task_queue_push(struct task_queue *taskQueue, struct Player *player) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        pthread_mutex_unlock(&taskQueue->mutex);
        DBG(YELLOW"<taskQueue>"NONE" : taskQueue is full!\n");
        return;
    }
    taskQueue->team[taskQueue->tail] = player;
    taskQueue->total++;
    DBG(GREEN"<Push>"NONE" : %s\n", player->name);
    if (++taskQueue->tail == taskQueue->size) {
        DBG(RED"<taskQueue>"NONE" : End\n");
        taskQueue->tail = 0;
    }
    //sleep(1);
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct Player *task_queue_pop(struct task_queue *taskQueue, struct Player *player) {
    pthread_mutex_lock(&taskQueue->mutex);
    //惊群效应
    while (taskQueue->total == 0) {
        DBG(PINK"<Debug>"NONE" : taskQueue is empty!\n ");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
    }
    player = taskQueue->team[taskQueue->head];
    taskQueue->total--;
    DBG(BLUE"<taskQueue>"NONE" : pop %s!\n", player->name);
    if (++taskQueue->head == taskQueue->size) {
        taskQueue->head = 0;
    }
   // sleep(1);
    pthread_mutex_unlock(&taskQueue->mutex);
    return player;
}

void *thread_run(void *arg) {
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct Player *player = task_queue_pop(taskQueue);
        do_work(player);
    }
}
