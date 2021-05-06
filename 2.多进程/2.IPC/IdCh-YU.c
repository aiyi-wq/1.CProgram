/*************************************************************************
	> File Name: IdCh-YU.c
	> Author: 
	> Mail: 
	> Created Time: Wed 14 Oct 2020 08:28:03 PM CST
 ************************************************************************/

#include "head.h"

struct Num {
    int status;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct Num *share_memory = NULL;

int main() {
    pid_t pid;


    make_non_block(1);

    int shmid;
    key_t key = ftok(".", 2025);
    if ((shmid = shmget(key, sizeof(struct Num), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }

    if ((share_memory = (struct Num*)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    }

    share_memory->status = 2;


    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&share_memory->mutex, &attr);

    pthread_condattr_t c_attr;
    pthread_condattr_init(&c_attr);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&share_memory->cond, &c_attr);

    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }

    if (pid == 0) {
        while (1) {
            pthread_mutex_lock(&share_memory->mutex);
            if (share_memory->status == 1) {
                pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
                printf("Cli!\n");
            }
            scanf("%d", &share_memory->status);
            sleep(1);
            if (share_memory->status == 1) {
                printf("unlock, to master\n");
                pthread_mutex_unlock(&share_memory->mutex);
                pthread_cond_signal(&share_memory->cond);
            } else {
                printf("I'm Client\n");
                pthread_mutex_unlock(&share_memory->mutex);
            }
        }
    } else {
        while (1) {
            pthread_mutex_lock(&share_memory->mutex);
            if (share_memory->status == 2) {
                pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
                printf("Mas!\n");
            }
            scanf("%d", &share_memory->status);
            sleep(1);
            if (share_memory->status == 2) {
                printf("unlock, to client\n");
                pthread_mutex_unlock(&share_memory->mutex);
                pthread_cond_signal(&share_memory->cond);
            } else {
                printf("I'm Master\n");
                pthread_mutex_unlock(&share_memory->mutex);
            }
        }
        wait(NULL);
    }


    return 0;
}
