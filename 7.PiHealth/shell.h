/*************************************************************************
	> File Name: shell.h
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 05:10:29 PM CST
 ************************************************************************/

#ifndef _SHELL_H
#define _SHELL_H
#define MAX_THREAD 6

struct ShellData {
    int Time;
    char sh[50];
    char conf[50];
};

void init_shell(int n, cJSON *conf, const char *who, const char *config);
void *doing_pthread(void *arg);
void thread_run();

#endif
