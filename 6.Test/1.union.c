/*************************************************************************
	> File Name: 1.union.c
	> Author: 
	> Mail: 
	> Created Time: Sun 20 Sep 2020 05:32:27 PM CST
 ************************************************************************/
//大端机/小端机
#include<stdio.h>

struct Data_1 {
    int a;
    char b[4];
};

//取决于最大的成员
union Data {
    int a;
    //char b[4];
    char b;
};

int main() {
    union Data data;
    data.a = 1;
    //printf("%d %d %d %d\n",data.b[0], data.b[1], data.b[2], data.b[3]);
    if (data.b == 1) {
        printf("小端机\n");
    } else {
        printf("大端机\n");
    }
    return 0;
}
