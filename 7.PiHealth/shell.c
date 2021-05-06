/*************************************************************************
	> File Name: shell.c
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 04:59:51 PM CST
 ************************************************************************/
#include<head.h>
#include"shell.h"

extern struct ShellData arr[MAX_THREAD];

pthread_mutex mutex = PTHREAD_MUTEX_INITIALIZER;
static char *shell_conf[8] = {"Proc", "Disk", "Mem", "Cpu", "User", "Sysinfo"};

static char *shell_sh[6] = {"ProcessLog.sh", "DiskLog.sh", "MemLog.sh", "CpuLog.sh", "UserLog.sh", "SystemLog.sh"};

void init_shell(int n, cJSON *conf, const char *who, const char *config) {
    for (int i = 0; i < MAX_THREAD; ++i) {
        strcpy(arr[i].conf, shell_conf[i]);
        strcpy(arr[i].sh, shell_sh[i]);
        arr[i].time = get_json_array_valueint(conf, who, config, arr[i].conf);
    } 
    return ;
}

void *doing_pthread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        int ind = *(int *)arg;
        char Filename[512] = {0};
        sprintf(Filename, "../../shell/%s >> ./log/%s", arr[ind].sh, arr[ind].conf);
        DBG(RED"%s\n" NONE, Filename);
        system(Filename);
            pthread_mutex_unlock(&mutex);
        sleep(arr[ind].time);
    }
    return NULL;
}

void thread_run() {
    int tmp_arg[MAX_THREAD] = {0};
    pthread_t tid[MAX_THREAD];
    for (int i = 0; i < MAX_THREAD; ++i) {
        tmp_arg[i] = i;
        pthread_create(&tid[i], NULL, doing_pthread, (void *)&tmp_arg[i]);
    }
    for (int i = 0; i < MAX_THREAD; ++i) {
        pthread_join(tid[i], NULL);
    }
}
