/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Sun 02 Aug 2020 08:09:06 PM CST
 ************************************************************************/
//server端
#include<head.h>

struct Message {
    char name[20];
    char msg[1024];
    pid_t pid;
    pthread_mutex_t mutex;
};

struct Message *client = NULL;

void *do_print(int signum) {
    while (1) {
        while (strlen(client->name) && strlen(client->msg)) {
            pthread_mutex_lock(&client->mutex);
            DBG(GREEN"<Debug>" NONE " : in server!\n");
            printf(GREEN"<%s>" NONE " : %s\n", client->name, client->msg);
            memset(client->name, 0, sizeof(client->name));
            memset(client->msg, 0, sizeof(client->msg));
            pthread_mutex_unlock(&client->mutex);
        }
    }
}

int main(int argc, char **argv) {
    int shmid;
    key_t key = ftok(".", 2010);
    if ((shmid = shmget(key, sizeof(struct Message), IPC_CREAT | 0666)) < 0) {
        perror("shmget()");
        exit(1);
    }
    if ((client = (struct Message*)shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&client->mutex, &attr);

    memset(client->name, 0, sizeof(client->name));
    memset(client->msg, 0, sizeof(client->msg));
    client->pid = getpid();
    DBG(GREEN"<Debug>" NONE " : pid = %d\n", client->pid);
    signal(SIGUSR1, (void *)do_print);

    while (1) {
        sleep(1);
    }

    return 0;
}
