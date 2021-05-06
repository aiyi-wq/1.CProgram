/*************************************************************************
	> File Name: 1.add.c
	> Author: 
	> Mail: 
	> Created Time: Sun 02 Aug 2020 02:19:46 PM CST
 ************************************************************************/

#include<head.h>

#define INS 5


char num_file[] = "./.num";
char lock_file[] = "./.lock";//系统生成的文件

struct Num {
    int now;
    int sum;
};

//save in file
size_t set_num(struct Num *num) {
    FILE *f = fopen(num_file, "w");
    size_t nwrite = fwrite(num, sizeof(struct Num), 1, f);
    fclose(f);
    return nwrite;
}

//get from file
size_t get_num(struct Num *num) {
    FILE *f = fopen(num_file, "r");
    if (f == NULL) {
        fclose(f);
        perror("fopen_r()");
        return -1;
    }
    size_t nread = fread(num, sizeof(struct Num), 1, f);
    if (nread < 0) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return nread;
}

void do_add(int max, int x) {
    struct Num num;
    while (1) {
        FILE *lock = fopen(lock_file, "w");
        flock(lock->_fileno, LOCK_EX);
        if (get_num(&num) < 0) {
            fclose(lock);//关闭文件时锁也被关闭
            continue;
        }
        DBG(GREEN"<Debug>" NONE " : <%d> now = %d, sum = %d\n", x, num.now, num.sum);
        if (num.now > max) {
            fclose(lock);
            break;
        }
        num.sum += num.now;
        num.now++;
        set_num(&num);
        //flock(lock->_fileno, LOCK_UN);
        fclose(lock);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s max ins \n", argv[0]);
        exit(1);
    }   
    int max = atoi(argv[1]);
    int ins = atoi(argv[2]);
    struct Num num;
    int x = 0;
    pid_t pid;
    num.now = 0;
    num.sum = 0;
    set_num(&num);
    for (int i = 0; i < INS; i++) {
        if ((pid = fork()) < 0) {
            perror("fork()");
            exit(1);
        }
        x = i;
        if (pid == 0) break;
    }
    if (pid == 0) {
        do_add(max, x);
        exit(0);
    }
    while (ins) {
        wait(NULL);
        ins--;
    }
    get_num(&num);
    printf("sum = %d\n", num.sum);
    return 0;
}
