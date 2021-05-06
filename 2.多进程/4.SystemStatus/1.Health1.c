/*************************************************************************
	> File Name: 1.Health1.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Oct 2020 12:15:30 PM CST
 ************************************************************************/
#include<health.h>

struct Share *Share_memory = NULL;

void doing_process(int who) {
    char msg[512] = {0};
    strcpy(msg, who ? "Slave" : "Master");
    printf("I'm %s\n", msg);
}

int main(int argc, char **argv) {
    int who = 0, shmid;
    DBG(GREEN"Start..\n" NONE);
    key_t key = ftok(".", 209);
    if ((shmid = shmget(key, sizeof(struct Share), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }
    DBG(GREEN"Shmget success..\n" NONE);
    if ((Share_memory = (struct Share *)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }
    DBG(GREEN"Shmat success..\n" NONE);

    memset(Share_memory, 0, sizeof(struct Share));

    DBG(GREEN"Share_memory memset success..\n" NONE);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&Share_memory->mutex, &attr);
    DBG(GREEN"Init mutex..\n" NONE);

    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&Share_memory->cond, &cond_attr);

    DBG(GREEN"Init cond..\n" NONE);
    Share_memory->type = 0;
    DBG(GREEN"Program set to be Master default..\n" NONE);
    DBG(GREEN"Forking..\n" NONE);
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        int pnum = 0;
        DBG(RED"The %dth Process act as a Controler!\n" NONE, pnum);
        while (1) {
            int cmd;
            DBG(YELLOW"Please Input Your Cmd(0 for Master, 1 for Slave):\n" NONE);
            scanf("%d", &cmd);
            if (cmd == 0) {
                Share_memory->type = 0;
            } else {
                Share_memory->type = 1;
            }
            pthread_cond_signal(&Share_memory->cond);
        }
    } else {
        pid_t pid1;
        if ((pid1 = fork()) < 0) {
            perror("fork()");
            exit(EXIT_FAILURE);
        }
        if (pid1 == 0) {
            int pnum = 1;
            DBG(PINK"The %dth Process act as a Controler!\n" NONE, pnum);
            while (1) {
                if (Share_memory->type == 1) {
                    pthread_mutex_lock(&Share_memory->mutex);
                    DBG(BLUE"Master Process Waiting for signal..\n" NONE);
                    pthread_cond_wait(&Share_memory->cond, &Share_memory->mutex);
                    pthread_mutex_unlock(&Share_memory->mutex);
                    //考虑了惊群效应
                    while (1) {
                        if (Share_memory->type == 1) {
                            break;
                        }
                        sleep(2);
                        doing_process(0);
                    }
                } else {
                    while (1) {
                        if (Share_memory->type == 1) break;
                        sleep(2);
                        doing_process(0);
                    }
                }
            }
        } else {
            int pnum = 2;
            DBG(BLUE"The %dth Process act as a Slave!\n" NONE, pnum);
            while (1) {
                if (Share_memory->type == 0) {
                    pthread_mutex_lock(&Share_memory->mutex);
                    DBG(BLUE"Slave Process Waiting for signal..\n" NONE);
                    pthread_cond_wait(&Share_memory->cond, &Share_memory->mutex);
                    pthread_mutex_unlock(&Share_memory->mutex);
                    while (1) {
                        if (Share_memory->type == 0) break;
                        sleep(2);
                        doing_process(1);
                    }
                } else {
                    while (1) {
                        if (Share_memory->type == 0) break;
                        sleep(2);
                        doing_process(1);
                    }
                }
            }
        }
    }
}

