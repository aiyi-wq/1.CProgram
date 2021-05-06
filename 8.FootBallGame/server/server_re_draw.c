/*************************************************************************
	> File Name: server_re_draw.c
	> Author: 
	> Mail: 
	> Created Time: Wed 28 Oct 2020 04:45:39 PM CST
 ************************************************************************/
#include<head.h>
#include"server_re_draw.h"


extern struct Player *rteam, *bteam;
extern WINDOW *Football, *Football_t;
extern struct BallStatus ball_status;
extern struct Bpoint ball;

void re_draw_ball() {
    //根据ball_status里记录的加速度，和上次re_drew时的速度，算出本次球应该移动的时间
    int time_move;
    ball_status.a.x + ball_status.a.y
    //加速度保持不变，速度更新
    //需要注意的是，当判断到速度减为0，ball_status里的速度和加速度都清空
    //同样需要注意的时，球如果超过球场边界，则判定为出界，球停止到边界即可
}

void re_draw_player(int team, char *name, struct Point *loc) {
	//根据team，切换颜色
    if (team == 0) {
        
    } else {
        
    }
  	//在loc位置打印player，并显示姓名
    
}

void re_draw_team(struct Player *team) {
    if (team->team == 0) {
        for (int i = 1; i <= 11; ++i) {
            re_draw_player(0, team->name, team->loc);
        }
    } else {
        for (int i = 1; i <= 11; ++i) {
            re_draw_player(1, bteam->name, bteam->loc);
        }
    }
}

void re_draw() {
    char tmp[512] = {0};
    werase(Football_t);
    box(Football, 0, 0);
    box(Football_t, 0, 0);
    re_draw_ball();
    re_draw_team(bteam);
    re_draw_team(rteam);
    re_drew_gate();
    wrefresh(Football_t);
}
