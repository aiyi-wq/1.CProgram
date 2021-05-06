/*************************************************************************
	> File Name: head.h
	> Author: 
	> Mail: 
	> Created Time: Sun 26 Jul 2020 06:38:03 PM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include"color.h"
#include<grp.h>
#include<pwd.h>
#include<time.h>
#include<math.h>
#include<sys/ioctl.h>
#include<ctype.h>
#include<sys/wait.h>
#include<pthread.h>
#include<sys/file.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<signal.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include"common.h"
#include"chat.h"
//#include"thread_pool.h"
#include"client_recv.h"
#include"datatype.h"
#include"datatype1.h"
#include<sys/time.h>
#include<ncurses.h>
#include<locale.h>
#include<cjson/cJSON.h>
#include"setscreen.h"
#include"file_transfer.h"
#include<sys/sem.h>
#include<parse_conf.h>
#include"ft_thread_pool.h"

#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...) 
#endif

#endif
