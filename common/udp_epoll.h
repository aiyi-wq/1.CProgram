/*************************************************************************
	> File Name: udp_epoll.h
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 08:55:05 PM CST
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H


//将一个用户注册到epoll实例中
void add_event_ptr(int epollfd, int fd, int events, struct Player *Player);
//将一个用户从epoll实例中注销
void del_event(int epollfd, int fd);
//服务端连接用户
int udp_connect(struct sockaddr_in *client);
//判断是否连接成功
int udp_accept(int fd, struct Player *player);
//将一个加入系统的用户加入到一个主从反应堆
void add_to_sub_reactor(struct Player *player);
//判断用户是否在线，游戏不允许重名用户登录
int check_online(struct LogRequest *request, int fd);
#endif
