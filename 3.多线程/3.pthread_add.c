/*************************************************************************
	> File Name: 3.pthread_add.c
	> Author: 
	> Mail: 
	> Created Time: Sat 01 Aug 2020 07:24:05 PM CST
 ************************************************************************/

#include<head.h>

#define INS 5
int now = 0, sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *add(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);//互斥锁
        if (now >= 1000) {
            pthread_mutex_unlock(&mutex);
            break;//跳出while循环
        }
        now++;
        sum += now;
        pthread_mutex_unlock(&mutex);
        printf("<%ld>%d\n", pthread_self(), now);
        usleep(20);
    }
}

int main() {
    
    pthread_t tid[INS];

    for (int i = 0; i < INS; i++) {
        pthread_create(&tid[i], NULL, add, NULL);
    }

    for (int i = 0; i < INS; i++) {
        pthread_join(tid[i], NULL);
    }//保证主线程在所有线程结束后结束，如果不加这三行代码，程序会直接执行41行代码，并且不再输出，因为主线程结束后程序就结束了

    printf("sum = %d\n", sum);
    return 0;
}
