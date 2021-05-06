/*************************************************************************
	> File Name: datatype1.h
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 07:00:03 PM CST
 ************************************************************************/

#ifndef _DATATYPE1_H
#define _DATATYPE1_H

//与球相关的结构
struct Bpoint {
    double x;
    double y;
};

//球的速度
struct Speed {
    double x;
    double y;
};

//球的加速度
struct Aspeed {
    double x;
    double y;
};

//球的状态
struct BallStatus {
    struct Speed v;
    struct Aspeed a;
    int by_team;   //which team
    char name[20];
    //pthread_mutex_t mutex;
};

struct Score {
    int red;
    int blue;
};

//与球员有关
struct Point {
    int x;
    int y;
};
#define MAX 11   //每队球员数量
struct Player {
    int team; // 0 RED 1 BLUE
    int fd;  //该玩家对应的连接
    char name[20];
    int online;  //1 在线  0 不在线
    int Score;
    char real_name[20];
    char chat_name[20];
    char id[50];
    char ip[20];
    int test[10];
    int flag; // 未响应次数, 心跳包
    struct Point loc;
};

//数据交互相关

    //登录相关
struct LogRequest {
    char name[20];
    int team;
    char msg[512];
};

struct LogResponse {
    int type;  // 0 OK 1 NO
    char msg[512];
};

struct Map {
    int width;
    int height;
    struct Point start;
    int gate_width;
    int gate_height;
};


//游戏期间交互

#define MAX_MSG 4096
//日常的消息交互, 如聊天信息， 统一长度为512字节

#define ACTION_KICK 0X01
#define ACTION_CARRY 0x02
#define ACTION_STOP 0x04
#define ACTION_DFL 0x08

struct Ctl {
    int action;
    int dirx;
    int diry;
    int strength;
};

//type 的值
#define FT_HEART 0X01
#define FT_ACK 0x02   //允许登陆
#define FT_MSG 0x04   //私聊, 队友之间
#define FT_WALL 0x08   //公告
#define FT_CTL 0x10   //控制信息
#define FT_MAP 0x20   //场地数据
#define FT_FIN 0x40  //下线操作
#define FT_SCORE 0x80  //比分变化
#define FT_GAMEOVER 0x100
//#define FT_NAK 0x200  //拒绝登陆

struct FootBallMsg {
    int type;
    int size;
    int team;
    char name[20];
    char msg[MAX_MSG];
    struct Ctl ctl;
};

#endif
