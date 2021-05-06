/*************************************************************************
	> File Name: 2.popen.c
	> Author: 
	> Mail: 
	> Created Time: Tue 04 Aug 2020 02:16:37 PM CST
 ************************************************************************/

#include<head.h>

int main(int argc, char **argv) {
    FILE *fp, *fw;
    fw = fopen("./buffer", "w");
    char buff[1024];
    size_t nread;
    fp = popen("ls /etc", "r");

    if (fp == NULL) {
        perror("fp NULL");
        exit(1);
    }
    while ((nread = fread(buff, sizeof(buff), 1, fp)) > 0) {
        fwrite(buff, strlen(buff), 1, fw);
        memset(buff, 0, sizeof(buff));
        if (feof(fp)) break;
    }
    fclose(fw);
    pclose(fp);

    return 0;
}
