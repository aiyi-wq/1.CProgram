/*************************************************************************
	> File Name: 2.thread.c
	> Author: 
	> Mail: 
	> Created Time: Sat 01 Aug 2020 06:42:16 PM CST
 ************************************************************************/

#include<head.h>

struct Arg {
    char nmae[20];
    int age;
}

void *print(void *arg) {
    struct Arg *tmp = (struct Arg *)arg;

    printf("%s is %d years old!\n", tmp->name, tmp->age);
}

int main() {
    pthread_t tid;
    struct Arg arg_in;
    strcpy(arg_in.name, "haizie");
    arg_in.age = 18;
    int age = 18;
    pthread_create(&tid, NULL, print, (void *)&arg_in);

    sleep(1);

    return 0;
}
