/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 09:34:57 AM CST
 ************************************************************************/

#include "chat.h"

struct Message *share_memory = NULL;

int main(int argc, char **argv) {
    struct Message tmp;
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
            case 'n':
               strcpy(tmp.name, optarg);
            break;
            default:
                fprintf(stderr, "Usage : %s -n nmae!\n", argv[0]);
                break;
        }
    }
    int shmid;
    key_t key = ftok(".", 2011);
    if ((shmid = shmget(key, sizeof(struct Message), IPC_CREAT | 0666)) < 0) {
        perror("shmid()");
        exit(1);
    }

    if ((share_memory = shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat()");
        exit(1);
    }

    while (1) {
        scanf("%[^\n]s", tmp.msg);
        getchar();
        if (!strlen(tmp.msg)) continue;
        while (1) {
            if (!strlen(share_memory->msg)) {
                pthread_mutex_lock(&share_memory->mutex);
                break;
            }
        }
        strcpy(share_memory->name, tmp.name);
        strcpy(share_memory->msg, tmp.msg);
        pthread_mutex_unlock(&share_memory->mutex);
        pthread_cond_signal(&share_memory->cond);
        memset(tmp.msg, 0, sizeof(tmp.msg));
        DBG(GREEN"<Debug>" NONE " : After signal!\n");
    }
    return 0;
}
