/*************************************************************************
	> File Name: ../common/send_ctl.c
	> Author: Aiiii
	> Mail: 2646967818@qq.com
	> Created Time: Sun 01 Nov 2020 08:33:58 PM CST
 ************************************************************************/

#include"head.h"
extern int sockfd;
extern struct FootBallMsg ctl_msg;

void send_ctl() {
    if (ctl_msg.ctl.dirx || ctl_msg.ctl.diry) {
        ctl_msg.ctl.action = ACTION_DFT;
        send(sockfd, (void *)&ctl_msg, sizeof(ctl_msg), 0);
        ctl_msg.ctl.dirx = ctl_msg.ctl.diry = 0;
        show_data_stream('n');
    }
}
