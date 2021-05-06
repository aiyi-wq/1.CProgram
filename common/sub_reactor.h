/*************************************************************************
	> File Name: sub_reactor.h
	> Author: 
	> Mail: 
	> Created Time: Thu 22 Oct 2020 01:53:59 PM CST
 ************************************************************************/

#ifndef _SUB_REACTOR_H
#define _SUB_REACTOR_H
#include"head.h"
#define NTHREAD 10  //略小于CPU的数量
void *sub_reactor(void *arg);
#endif
