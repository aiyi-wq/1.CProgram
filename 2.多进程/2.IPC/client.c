/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Sun 02 Aug 2020 08:09:06 PM CST
 ************************************************************************/

#include<head.h>

struct Message {
    char name[20];
    char msg[1024];
    pid_t pid;
    pthread_mutex_t mutex;
};

struct Message *client = NULL;

int main(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
                break;
            default:
            fprintf(stderr, "Usage : %s [-n name]\n", argv[0]);
            break;
        }
    }
    argv += optind;
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

    while (1) {
        pthread_mutex_lock(&client->mutex);
        strcpy(client->name, *argv);
        scanf("%[^\n]s", client->msg);
        getchar();
        DBG(PINK"<%s>" NONE "%s\n", client->name, client->msg);
        kill(client->pid, SIGUSR1);
        pthread_mutex_unlock(&client->mutex);
        usleep(20);
    }
    return 0;
}
