/*************************************************************************
	> File Name: health.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Oct 2020 12:13:13 PM CST
 ************************************************************************/
//存放共享内存的结构体
#ifndef _HEALTH_H
#define _HEALTH_H
#include<head.h>
struct Share {
    int type;// 0 Master启动 1Slave启动
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};
#endif
