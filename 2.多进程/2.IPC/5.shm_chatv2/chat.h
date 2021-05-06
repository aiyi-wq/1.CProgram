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
    pid_t pid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

#endif
