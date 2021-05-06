/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 09:19:51 AM CST
 ************************************************************************/

#include"chat.h"

struct Message *share_memory = NULL;

void print(int signum) {
    pthread_mutex_lock(&share_memory->mutex);
    printf("<%s> : %s\n", share_memory->name, share_memory->msg);
    memset(share_memory->msg, 0, sizeof(share_memory->msg));
    memset(share_memory->name, 0, sizeof(share_memory->name));
    pthread_mutex_unlock(&share_memory->mutex);
}

int main(int argc, char **argv) {
    int shmid;
    key_t key = ftok(".", 2011);
    if ((shmid = shmget(key, sizeof(struct Message), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }

    if ((share_memory = (struct Message *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    }

    memset(share_memory, 0, sizeof(struct Message));
    
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&share_memory->mutex, &attr);

    share_memory->pid = getpid();
    while (1) {
        signal(SIGUSR2, print);
        pause();    
    }

    return 0;
}
