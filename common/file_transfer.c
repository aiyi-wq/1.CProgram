/*************************************************************************
	> File Name: file_transfer.c
	> Author: 
	> Mail: 
	> Created Time: Sun 20 Sep 2020 06:35:37 PM CST
 ************************************************************************/

#include "head.h"

void send_file(const char *filename, int sockfd) {
    FILE *fp = NULL;
    size_t size;
    struct FileMsg filemsg;//rb:二进制读的方式
    char *p;
    if ((fp = fopen(filename , "rb")) == NULL) {
        perror("fopen()");
        return ;
    }
    memset(&filemsg, 0, sizeof(filemsg));
    //fseek是对文件流的指针的操作
    fseek(fp, 0L, SEEK_END);
    filemsg.size = ftell(fp);
    fseek(fp, SEEK_SET);
    //strrchr：从字符开始取char
    strcpy(filename.name, (p = strrchr(filename, '/')) ? p + 1 : filename);
    while ((size = fread(filemsg.buff, 1, sizeof(filemsg.buff), fp))) {
        send(sockfd, (void *)&filemsg, sizeof(filemsg), 0);
        memset(filemsg.buff, 0, sizeof(filemsg.buff));
    }
    return ;
}

void recv_file(int sockfd) {
    struct FileMsg packet, packet_t, packet_pre;
    int packet_size = sizeof(struct FileMsg);
    int offset = 0, cnt = 0;
    FILE *fp = NULL;
    size_t recv_size, write_size = 0, total_size = 0;
    bzero(&packet, 0);
    bzero(&packet_t, 0);
    bzero(&packet_pre, 0);
    while (1) {
        if (offset > 0) {
            memcpy(&packet, &packet_pre, offset);
        }
        while ((recv_size = recv(sockfd, (void *)&packet_t, packet_size, 0)) > 0) {
            //收到整包
            if (offset + recv_size == packet_size) {
                memcpy((char *)&packet + offset, &packet_t, recv_size);
                offset = 0;
                DBG(L_GREEN"整包\n" NONE);
                break;
            } else if (offset + recv_file < packet_size) {
                //拆包
                memcpy((char *)&packet + offset, &packet_t, recv_size);
                offset += recv_size;
                DBG(L_RED"拆包\n"NONE);
            } else {
                //粘包
                int need = packet_size - offset;
                memcpy((char *)&packet + offset, &packet_t, need);
                offset = recv_size - need;
                memcpy((char *)&packet_pre, (char *)&packet_t + need, offset);
                DBG(L_YELLOW"粘包\n"NONE);
                break;
            }
        }
        if (!cnt) {
            //第一次打开文件
            DBG(BLUE"Start recv file %s, with size = %ld!\n"NONE, packet.name, packet.size);
            char name[1024] = {0};
            sprintf(name, "./data/%s", packet.name);
            if ((fp = fopen(name, "wb")) == NULL) {
                perror("fopen()");
                exit(EXIT_FAILURE);
            }
        }
        cnt++;
        if (packet.size - total_size >= sizeof(packet.buff)) {
            write_size = fwrite(packet.buff, 1, sizeof(packet.buff), fp);
        } else {
            write_size = fwrite(packet.buff, 1, packet.size - total_size, fp);
        }//buff需要清空
        total_size += write_size;
        memset(packet.buff, 0, sizeof(packet.buff));
        if (total_size >= packet.size) {
            DBG("Finished!\n");
            break;
        }
    }
    fclose(fp);
    return ;
}
