/*************************************************************************
	> File Name: ChID.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 14 Oct 2020 05:40:24 PM CST
 ************************************************************************/

#include"head.h"

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

union semun arg;
struct sembuf sops1, sops2;

int p(int semid) {
    if (semop(semid, &sops2, 1) == -1) {
        perror("semop2()");
        exit(EXIT_FAILURE);                
    }
    return 0;
}

int v(int semid) {
    if (semop(semid, &sops1, 1) == -1) {
        perror("semop2()");
        exit(EXIT_FAILURE);                
    }
    return 0;
}

int main () {
    int key1 = ftok(".",2016);
    int key2 = ftok(".",2017);
    int key[4],semid[4];
    for (int i = 0; i < 4; i++) {
        key[i] = ftok(".",(i + 2016));
    }
    for (int i = 0; i < 4; i++) {
        if ((semid[i] = semget(key[i], 1, IPC_CREAT | 0777)) == -1) {
            perror("semget()");
            exit(1);
        }
        if (i < 2) {
            arg.val = 1;
        } else {
            arg.val = 0;
        }
        if (semctl(semid[i], 0, SETVAL, arg) == -1) {
            perror("semctl()");
            exit(1);
        }
    }
    sops1.sem_op = 1;
    sops2.sem_op = -1;
    int pid[2];
    int x = 0;
    for (int i = 0; i < 2; i++) {
        if ((pid[i] = fork()) < 0) {
            perror("fork()");
            exit(1);
        }
        x = i;
        if (pid[x] == 0) break;
    }
    if (pid[x] == 0) {
        if (x == 0) {
            while (1) {
                //p(semid[2]);
                p(semid[0]);
                printf("i am master\n");
                sleep(2);
                v(semid[0]);
                //v(semid[2]);
                //v(semid[2]);
            }
        }else {
            while (1) {
                //p(semid[3]);
                //p(semid[1]);
                printf("i am client\n");
                sleep(2);
                //v(semid[1]);
                //v(semid[3]);
            }
        }    
    }else {
        int status = 0;
        while (1) {
            int a;
            scanf("%d",&a);
            if(a == 1 && status != 1) {
                status = 1;
                //p(semid[2]);
                v(semid[0]);
                //v(semid[2]);
                //p(semid[3]);
                //v(semid[1]);
                //v(semid[3]);
            }else if (a == 2 && status != 2) {
                status = 2;
                //p(semid[2]);
                //p(semid[1]);
                p(semid[0]);
                //v(semid[2]);
            }else if (a == 2 | a == 1){
                printf("您现在已经处于该状态\n");
            }else {
                printf("请输入1或2\n");
            }
        }
    }
}
