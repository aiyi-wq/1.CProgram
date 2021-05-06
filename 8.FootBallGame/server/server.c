/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 07:21:10 PM CST
 ************************************************************************/

#include<head.h>

int port = 0, message_num = 0; //服务端等待客户接入的端口， 设置为全局
char *conf = "./football.conf";
int repollfd, bepollfd;
struct Player *rteam, *bteam;
struct Map court;
struct Bpoint ball;
struct Score score;
struct Ball_Status ball_status;
char data_stream[20] = {0};
pthread_mutex_t rmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bmutex = PTHREAD_MUTEX_INITIALIZER;

//WINDOW *Football, *Football_t, *Message, *Help, *Score, *Write;

int main(int argc, char **argv) {
    int opt, epollfd, server_listen;
    pthread_t red_t, blue_t, heart_t;
    //memset(court, 0, sizeof(struct Map));
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch(opt) {
            case 'p':
                port = atoi(optarg);
            break;
            default:
                fprintf(stderr, "Usage: %s -p port!\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    //setlocale(LC_ALL, "");
    
    if (!port) port = atoi(get_conf(conf, "PORT"));
    //从配置文件中获取球场的额长宽，球初始位于中央
    
    if ((server_listen = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(EXIT_FAILURE);
    }
    make_non_block(server_listen);
    DBG(GREEN"INFO" NONE " : server started on port %d\n", port);
    DBG(GREEN"INFO" NONE " : calloced memcpy for storing users!\n");

#ifndef _D
    initfootball();
#endif

    rteam = (struct Player *)calloc(MAX, sizeof(struct Player));
    bteam = (struct Player *)calloc(MAX, sizeof(struct Player));
    epollfd = epoll_create(MAX * 2);
    repollfd = epoll_create(MAX);
    bepollfd = epoll_create(MAX);

    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        perror("epoll_create()");
        exit(EXIT_FAILURE);
    }

    struct task_queue redQueue;
    struct task_queue blueQueue;
    task_queue_init(&redQueue, MAX, repollfd);
    task_queue_init(&blueQueue, MAX, bepollfd);

    pthread_create(&red_t, NULL, sub_reactor, (void *)&redQueue);
    pthread_create(&blue_t, NULL, sub_reactor, (void *)&blueQueue);
    pthread_create(&heart_t, NULL, heart_beat, NULL);

    signal(SIGINT, server_exit);

    struct epoll_event ev, events[MAX * 2];
    ev.events = EPOLLIN;
    ev.data.fd = server_listen;

    signal(SIGALRM, re_draw);

    //计时器
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 5;
    itimer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itimer, NULL);

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
        perror("epoll_ctl()");
        exit(EXIT_FAILURE);
    }
    DBG(GREEN"INFO" NONE " : main reactor Created!\n");

    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    socklen_t len = sizeof(client);

    //Show_Message
    //信号掩码
    sigset_t origmask, sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);
    //主反应堆用来判断玩家发送的报文是否符合规定
    while (1) {
        DBG(YELLOW"Main Reactor" NONE " : waiting for client.\n");
        pthread_sigmask(SIG_SETMASK, &sigmask, &origmask);
        int nfds = epoll_wait(epollfd, events, MAX * 2, -1);
        pthread_sigmask(SIG_SETMASK, &origmask, NULL);
        if (nfds < 0) {
            perror("main reactor: epollfd_wait()");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; ++i) {
            char buff[512] = {0};
            struct Player player;
            bzero(&player, sizeof(player));
            if (events[i].data.fd == server_listen) {
                int new_fd = udp_accept(server_listen, &player);
                if (new_fd > 0) {
                    sprintf(buff, "%s login the game", player.name);
                    //Show_Message;
                    show_data_stream("l");
                    add_to_sub_reactor(&player);
                }
            }

        }
    }
    return 0;
}
