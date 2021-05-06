/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Wed 21 Oct 2020 07:22:52 PM CST
 ************************************************************************/

#include<head.h>

int server_port = 0;
char server_ip[20] = {0};
char name[20] = {0};
char logmsg[30] = {0};
int team = -1; // RED 0 BLUE 1
char *conf = "./football.conf";
int sockfd = -1;
struct Map court;
struct Bpoint ball;
struct FootBallMsg chat_msg;
struct FootBallMsg ctl_msg;
int show_name = 0;
int message_num = 0;
char data_stream[20] = {0};
extern WINDOW *Football, *Football_t, *Message, *Help, *Score, *Write;

void logout(int signum) {
    struct FootBallMsg msg;
    msg.type = FT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    close(sockfd);
    DBG(RED"Bye!\n"NONE);
    endwin();
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    int opt;
    struct LogResponse response;
    struct LogRequest logrequest;
    bzero(&logrequest, sizeof(logrequest));
    bzero(&response, sizeof(respsonse));
    pthread_t recv_t, draw_t;
    bzero(&chat_msg, sizeof(struct FootBallMsg));
    bzero(&ctl_msg, sizeof(struct FootBallMsg));
    chat_msg.type = FT_WALL;
    ctl_msg.type = FT_CTL;
    while ((opt = getopt(argc, argv, "p:h:n:t:m::")) != -1) {
        switch (opt) {
            case 'p':
                server_port = atoi(optarg);
            break;
            case 'h':
                strcpy(server_ip, optarg);
            break;
            case 'n':
                strcpy(name, optarg);
            break;
            case 't':
                team = atoi(optarg);
            case 'm':
                strcpy(logmsg, optarg);
            break;
            default:
                fprintf(stderr, "Usage : %s -h host -p port -n [name] -t [team] -m [msg]!\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (!server_port) server_port = atoi(get_conf(conf, "SERVERPORT"));
    if (!strlen(server_ip)) strcpy(server_ip, get_conf(conf, "SERVERIP"));
    if (!strlen(name)) strcpy(name, get_conf(conf, "NAME"));
    if (!strlen(logmsg)) strcpy(logmsg, get_conf(conf, "LOGMSG"));
    if (team == -1) team = atoi(get_conf(conf, "TEAM"));

    //DBG(YELLOW"SERVERPORT" NONE " %d\n", server_port);
    //DBG(YELLOW"SERVERIP" NONE " %s\n", server_ip);
    //DBG(YELLOW"NAME" NONE " %s\n", name);
    //DBG(YELLOW"TEAM_NUM" NONE " %d(0 RED 1 BLUE)\n", team);
    //DBG(YELLOW"LOGMSG" NONE " %s\n", logmsg);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip);
    socklen_t len = sizeof(server);
    if ((sockfd = socket_udp()) < 0) {
        perror("socket_create_udp()");
        exit(EXIT_FAILURE);
    }
    strcpy(logrequest.name, name);
    logrequest.team = team;
    sendto(sockfd, (void *)&logrequest, sizeof(logrequest), 0, (struct sockaddr *)&server, len);

    bzero(&logrequest, sizeof(logrequest));
    DBG(GREEN"INFO" NONE " : create sockfd success...\n");
    make_non_block(sockfd);
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 10000;

    sendto(sockfd, (void *)&respsonse, sizeof(response), 0, (struct sockaddr *)&server, len);
    fd_set client_fds;
    FD_ZERO(&client_fds);
    FD_SET(sockfd, &client_fds);
    int retval, error = -1;
    int len = sizeof(server);
    retval = select(sockfd + 1, NULL, &client_fds, NULL, &tv);
    if (retval < 0) {
        perror("select()");
        exit(EXIT_FAILURE);
    } else if (retval) {
        int ret = recvfrom(sockfd, (void *)&respsonse, sizeof(respsonse), 0, (struct sockaddr *)&server, &len);
        if (ret != sizeof(respsonse) || respsonse.type) {
            printf("The Game Server refused your login.\n\tThis May be helpful:%s\n", respsonse.msg);
            exit(EXIT_FAILURE);
            }
        } else {
            printf("The Game Server is out of service!\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("Server : %s\n", respsonse.msg);
    connect(sockfd, (struct sockaddr *)&server, len);
#ifndef _D
    initfootball();
#endif
    pthread_create(&recv_t, NULL, client_recv, NULL);
    signal(SIGALRM, send_ctl);
    struct itimerval itimer;
    itimer.it_interval.tv_sec = 0;
    itimer.it_interval.tv_usec = 100000;
    itimer.it_value.tv_sec = 0;
    itimer.it_value.tv_usec = 100000;
    setitimer(ITIMER_REAL, &itimer, NULL);

    while (1) {
        int c = getchar();
        switch (c) {
            case 'a':
                ctl_msg.ctl.dirx -= 1;
            break;
            case 'd':
                ctl_msg.ctl.dirx += 1;
            break;
            case 'w':
                ctl_msg.ctl.diry -= 1;
            break;
            case 's':
                ctl_msg.ctl.diry += 1;
            break;
            case 13:
                send_chat();
            break;
            case " ": {
                show_strength();
                break;
            }
            case 'k': {
                show_data_stream('k');
                struct FootBallMsg msg;
                bzero(&msg, sizeof(msg));
                msg.type = FT_CTL;
                msg.ctl.action = ACTION_KICK;
                msg.ctl.strength = 1;
                send(sockfd, (void *)&msg, sizeof(msg), 0);
                break;
            }
        }
    }
    return 0;
}
