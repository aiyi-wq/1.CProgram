/*************************************************************************
	> File Name: file_transfer.h
	> Author: 
	> Mail: 
	> Created Time: Sun 20 Sep 2020 06:33:55 PM CST
 ************************************************************************/

#ifndef _FILE_TRANSFER_H
#define _FILE_TRANSFER_H
void send_file(const char *filename, int sockfd);
void recv_file(int sockfd); 
#endif
