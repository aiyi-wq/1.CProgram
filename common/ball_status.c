/*************************************************************************
	> File Name: ../common/ball_status.c
	> Author: Aiiii
	> Mail: 2646967818@qq.com
	> Created Time: Sun 01 Nov 2020 04:57:04 PM CST
 ************************************************************************/

#include"ball_status.h"
#define PI 3.1415926

extern WINDOW *Message;
extern struct Bpoint ball;
extern struct BallStatus ball_status;

int can_kick(struct Point *loc, int strength) {
    char tmp[512] = {0};
    sprintf(tmp, "loc->x = %d, (int)ball.x = %d, loc->y = %d, (int)ball.y = %d", loc->x - 2, (int)ball.x, loc->y, (int)ball.y);
    //Show_Message
    if (abs(loc->x - 2 - (int)ball.x) <= 2 && abs(-loc->y + 1 + (int)ball.y) <= 2) {
        if (loc->x - 2 == (int)ball.x && loc->y - 1 == (int)ball.y) return 0;
        double v_tmp = (40.0 * strength) * 0.2;
        sprintf(tmp, "v_tmp = %lf", v_tmp);
        //Show_Message
        double a;
        if (loc->x - 2 == (int)ball.x) {
            a = PI / 2;
        } else {
            a = atan(fabs(-loc->y + 1 + ball.y) / abs(loc->x - 2 - ball.x));
        }
        if (loc->x - 2 > ball.x) {
            ball_status.v.x = -cos(a) * v_tmp;
            ball_status.a.x = cos(a) * 3;
        } else {
            ball_status.v.x = cos(a) * v_tmp;
            ball_status.a.x = -cos(a) * 3;
        }
        if (-loc->y + 1 > -ball.y) {
            ball_status.v.y = sin(a) * v_tmp;
            ball_status.a.y = sin(a) * 3;
        } else {
            ball_status.v.y = -sin(a) * v_tmp;
            ball_status.a.y = sin(a) * 3;
        }
        return 1;
    } else return 0;
}

int can_access(struct Point *loc) {
    char tmp[512] = {0};
    sprintf(tmp, "Player loc: <x = %d, y = %d>, Ball loc : <x = %lf, y = %lf>", loc->x - 2, loc->y - 1, ball.x, ball.y);
}
