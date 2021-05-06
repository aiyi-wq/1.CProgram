/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Sun 09 Aug 2020 04:12:06 PM CST
 ************************************************************************/

#include<head.h>

void *do_chat(void *arg) {
    int fd;
    fd = *(int *)arg;
    DBG(PINK"<Debug>" NONE "In thread %d\n", fd);
    char name[50] ={0};
    char ip[20] = {0};
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    socklen_t len = sizeof(client);
    getpeername(fd, (struct sockaddr *)&client, &len);
    strcpy(ip, inet_ntoa(client.sin_addr));
    DBG(PINK"<Debug>" NONE " : ip = %s!\n", inet_ntoa(client.sin_addr));

    pthread_mutex_t mutex;
    //pthread_cond_t cond;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mutex, &attr);

    /*pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&cond, &cond_attr);*/

    while (1) {
        ssize_t nrecv;
        char buff[512] = {0};
        if ((nrecv = recv(fd, buff, sizeof(buff), 0)) <= 0) {
            close(fd);
            DBG(PINK"<Debug>" NONE " : Before exit %d!\n", fd);
            return NULL;
        }
        pthread_mutex_lock(&mutex);
        //pthread_cond_wait(&cond, &mutex);
        printf("Recv : %s\n", buff);
        send(fd, buff, strlen(buff), 0);
        pthread_mutex_unlock(&mutex);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int listener, port;
    port = atoi(argv[1]);
    if ((listener = socket_create(port)) < 0) {
        perror("socket_create()");
        exit(EXIT_FAILURE);
    }
    //开始连接
    while (1) {
        int newfd;
        pthread_t tid;
        if ((newfd = accept(listener, NULL, NULL)) < 0) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }
        pthread_create(&tid, NULL, do_chat, (void *)&newfd);//每个进程共用同一片地址空间
    }
    return 0;
}
