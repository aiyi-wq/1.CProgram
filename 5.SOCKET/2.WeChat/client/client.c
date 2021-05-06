/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Wed 19 Aug 2020 03:19:39 PM CST
 ************************************************************************/

#include <head.h>
char *conf = "./chat.conf";
char name[20]= {0};
char server_ip[20] = {0};
int server_port = 0;
int sockfd, msg_num = 0;
WINDOW *msg_win, *sub_msg_win, *input_win, *sub_input_win, *help_win, *sub_help_win;

void logout(int signum) {
    struct ChatMsg msg;
    msg.type = CHAT_FIN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    if (recv(sockfd, (void *)&msg, sizeof(msg), 0) <= 0) {
        sprintf(msg.msg,"Bye 1");
        print_message(sub_msg_win, &msg, 1);
        sleep(1);
        endwin();
        exit(EXIT_FAILURE);
    }
    /*if (msg.type & CHAT_FIN_1) {
        sprintf(msg.msg, GREEN"Bye 2" NONE);
        print_message(sub_msg_win, &msg, 1);
        sleep(1);
        endwin();
        exit(EXIT_SUCCESS);
    }*/
}

int main(int argc, char **argv) {
    int opt;
    pthread_t recv_t;
    while ((opt = getopt(argc, argv, "p:h:n::")) != -1) {
        switch (opt) {
            case 'p':
                //server_port= atoi(argv[4]);
                server_port= atoi(optarg);
            break;
            case 'n':
                //strcpy(name, argv[6]);
                strcpy(name, optarg);
            break;
            case 'h':
                //strcpy(server_ip, argv[2]);
                strcpy(server_ip, optarg);
            break;
            default:
                fprintf(stderr, "Usage : %s -h host -p port -n [name]!\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!server_port) server_port = atoi(get_conf(conf, "SERVERPORT"));
    if (!strlen(server_ip)) strcpy(server_ip, get_conf(conf, "SERVERIP"));
    if (!strlen(name)) strcpy(name, get_conf(conf, "NAME"));
    DBG(GREEN"INFO" NONE " : server_ip = %s, server_port = %d, client_name = %s\n", server_ip, server_port, name);

    setlocale(LC_ALL, "");
    if ((sockfd = socket_connect_timeout(server_ip, server_port, 100000)) < 0) {
        perror("socket_connect_timeout()");
        exit(EXIT_FAILURE);
    }
    DBG(GREEN"INFO" NONE " : connected to server via TCP!\n");
    struct ChatMsg msg;
    bzero(&msg, sizeof(msg));
    strcpy(msg.name, name);
    msg.type = CHAT_SYN;
    send(sockfd, (void *)&msg, sizeof(msg), 0);
    DBG(GREEN"INFO" NONE " : sent CHAT_SYN to server\n");
    bzero(&msg, sizeof(struct ChatMsg));

    recv(sockfd, (void *)&msg, sizeof(msg), 0);
    DBG(GREEN"INFO" NONE " : recved packet from server\n");
    if (msg.type & CHAT_NAK) {
        DBG(RED"Server returned Error" NONE " : Login failed!:%s\n", msg.msg);
        exit(EXIT_FAILURE);
    }

    DBG(RED"Login success" NONE " : login success : %s\n", msg.msg);
    signal(SIGINT, logout);
    pthread_create(&recv_t, NULL, client_recv, NULL);

    init_ui();
    strcpy(msg.name, name);
    echo();//打开ui界面
    nocbreak();//行缓冲
    while (1) {
        msg.type = CHAT_PUB;
        bzero(msg.msg, sizeof(msg.msg));
        w_gotoxy_puts(sub_input_win, 1, 1, "Input :");
        mvwscanw(sub_input_win, 2, 2, "%[^\n]s", msg.msg);
        //scanf("%[^\n]s", msg.msg);
        //getchar();
        if (!strlen(msg.msg)) continue;
        if (msg.msg[0] == '@') msg.type = CHAT_PRI;
        if (msg.msg[0] == '#') msg.type = CHAT_FUNC;
        int retval = send(sockfd, (void *)&msg, sizeof(msg), 0);
        DBG(RED"Send" NONE " :%d bytes of message has send to server! : %s\n", retval, msg.msg);
        wclear(input_win);
        box(input_win, 0, 0);
        wrefresh(input_win);
    }
    return 0;
}
