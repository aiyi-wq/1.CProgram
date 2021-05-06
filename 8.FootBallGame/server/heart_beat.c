/*************************************************************************
	> File Name: heart_beat.c
	> Author: 
	> Mail: 
	> Created Time: Mon 26 Oct 2020 10:13:04 PM CST
 ************************************************************************/

#include<head.h>

extern struct Player *rteam, *bteam;
extern int repollfd, bepollfd;
void heart_beat_team(struct Player *team);
void *heart_beat(void *arg);

void heart_beat_team(struct Player *team) {
    for (int i = 0; i < strlen(team); ++i) {
        if (team->online == 1) {
            team->flag = FT_HEART;
            team->flag--;
            //sendto();
        }
        if (team->flag == 0) {
            team->online = 0;
            del_event(team->fd);
        }
    }
}

void *heart_beat(void *arg) {
    while (1) {
        //固定时间调用hear_beat_team
    }
}
