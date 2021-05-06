/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: Sat 08 Aug 2020 02:32:14 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
extern char conf_ans[512];
char *get_conf(const char *conf, char *key);
char *get_conf1(const char *conf, char *key);
//footballgame config_get
char *get_conf_value(const char *path, const char *key);
int make_non_block(int fd);
int make_block(int fd);
extern char conf_value_ans[512];
int socket_create(int port);
int socket_connect(char *ip, int port);
int socket_connect_timeout(char *ip, int port, long timeout);
int socket_create_udp(int port);
int socket_udp();
#endif
