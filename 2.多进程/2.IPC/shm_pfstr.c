/*************************************************************************
	> File Name: shm_pfstr.c
	> Author: 
	> Mail: 
	> Created Time: Sun 02 Aug 2020 04:34:02 PM CST
 ************************************************************************/

#include<head.h>

#define MAXSIZE 1024

struct Str {
    char buff[MAXSIZE];
    pthread_mutex_t mutex;
};

struct Str *share_memory = NULL;

void output(struct Str *str) {
    while (1) {
        pthread_mutex_lock(&str->mutex);
        DBG(GREEN"<Debug>" NONE " after lock in child process\n");
        for (int i = 0; i < strlen(str->buff) - 1; i++) {
            if (str->buff[i] == '\0') {
                break;
            }    
            DBG(GREEN"<Debug>" NONE " for()");
            printf("%c", str->buff[i]);
        }
        DBG(GREEN"<Debug>" NONE ": str = %s ", str->buff);
        memset(str->buff, 0, MAXSIZE);
        pthread_mutex_unlock(&str->mutex);
    }
}

int main() {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 2009);
    if ((shmid = shmget(key, sizeof(struct Str), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }

    if ((share_memory = (struct Str *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    } 

    memset(share_memory, 0, sizeof(struct Str));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&share_memory->mutex, &attr);
    
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid > 0) {
    while (1) {
        pthread_mutex_lock(&share_memory->mutex);
        DBG(GREEN"<Debug>" NONE " after lock in parent process\n");
        scanf("%s", share_memory->buff);
        pthread_mutex_unlock(&share_memory->mutex);
        usleep(20);
    }
    }
    if (pid == 0) {
        DBG(GREEN"<Debug>" NONE " : pid = %d", pid);
        output(share_memory);
        exit(0);
    }
    return 0;
}
