/*************************************************************************
	> File Name: IdSwitch.c
	> Author: 
	> Mail: 
	> Created Time: Sun 11 Oct 2020 02:43:15 PM CST
 ************************************************************************/

#include<head.h>

struct Msg {
    int flag;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct Msg *share_memory = NULL;
int main(int argc, char **argv) {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 209);
    if ((shmid = shmget(key, sizeof(struct Msg), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }
    if ((share_memory = (struct Msg *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&share_memory->mutex, &attr);

    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&share_memory->cond, &cond_attr);

    memset(share_memory, 0, sizeof(struct Msg));
    share_memory->flag = 1;
    int x;
    printf("Please choose your identity\n");
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }

    if (pid == 0) {
        while (1) {
            pthread_mutex_lock(&share_memory->mutex);
            if (share_memory->flag == 1) {
                pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
                DBG(GREEN"<默认状态>" NONE "I'm Master!\n");
            }
            scanf("%d", &share_memory->flag);
            if (share_memory->flag == 1) {
                pthread_mutex_unlock(&share_memory->mutex);
                pthread_cond_signal(&share_memory->cond);
                printf("I'm Master!\n");
            } else {
                printf("I'm Client!\n");
                pthread_mutex_unlock(&share_memory->mutex);
            }
        }
    } else {
        while (1) {
            pthread_mutex_lock(&share_memory->mutex);
            if (share_memory->flag == 1) {
                pthread_cond_wait(&share_memory->cond, &share_memory->mutex);
                DBG(GREEN"默认状态" NONE "I'm Master!\n");
            }
            scanf("%d", &share_memory->flag);
            if (share_memory->flag == 2) {
                pthread_mutex_unlock(&share_memory->mutex);
                pthread_cond_signal(&share_memory->cond);
                printf("I'm Client!\n");
            } else {
                printf("I'm Master!\n");
                pthread_mutex_unlock(&share_memory->mutex);
            }
        }
    }

    return 0;
}
