/*************************************************************************
	> File Name: 1.shm_add.c
	> Author: 
	> Mail: 
	> Created Time: Sun 02 Aug 2020 04:01:48 PM CST
 ************************************************************************/

#include<head.h>

#define MAXSIZE 1024

struct Num {
    char buff[MAXSIZE];
    pthread_mutex_t mutex;
};


struct Num *share_memory = NULL;

void do_print() {
    while (1) {
        if (strlen(share_memory->buff)) {
            pthread_mutex_lock(&share_memory->mutex);//lock
            DBG(GREEN"<Debug>" NONE " : locked in parent\n");
            printf(RED"<parent>" NONE " : %s\n", share_memory->buff);
            memset(share_memory->buff, 0, sizeof(share_memory));
            pthread_mutex_unlock(&share_memory->mutex);//unlock
        }
    }
}


int main(int argc, char **argv) {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 2009);
    if ((shmid = shmget(key, sizeof(struct Num), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }

    if ((share_memory = (struct Num*)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    }
    
    memset(share_memory, 0, sizeof(struct Num));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&share_memory->mutex, &attr);

    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) {
        while (1) {
            if (strlen(share_memory->buff)) continue;
            pthread_mutex_lock(&share_memory->mutex);
            DBG(GREEN"Debug" NONE " : locked in child\n");
            scanf("%[^\n]s", share_memory->buff);
            getchar();
            printf(PINK"<child>" NONE "%s\n", share_memory->buff);
            pthread_mutex_unlock(&share_memory->mutex);
        }
    } else {
        do_print();
        wait(NULL);
    }
    return 0;
}
