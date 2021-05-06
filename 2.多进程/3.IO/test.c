/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: Sat 08 Aug 2020 02:33:32 PM CST
 ************************************************************************/

#include<head.h>

int main() {
    char buff[512] = {0};
    int fd;
   // make_non_block(0);
    make_block(0);
    scanf("%s", buff);
    printf("%s\n", buff);
    return 0;
}
