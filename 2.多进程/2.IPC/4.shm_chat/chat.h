/*************************************************************************
	> File Name: chat.h
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 09:30:03 AM CST
 ************************************************************************/

#ifndef _CHAT_H
#define _CHAT_H

#include<head.h>

struct Message {
    char name[20];
    char msg[1024];
    pid_t pid;//server进程pid，可以使用kill向server进程发送信号
    pthread_mutex_t mutex;//至少有两个client进程会飞同步访问msg，所以需要上锁
};

#endif
