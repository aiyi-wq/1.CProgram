/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: Sat 08 Aug 2020 02:35:41 PM CST
 ************************************************************************/

/******************************************
static extern, 一个是静态变量，作用相当于全局变量，一个是引入外部的全局变量, 前者只能在当前的文件使用,而后者可以在彼得文件中使用
***************************/

#include"head.h"

//句柄
#define handle_error(msg)\
    do{perror(msg); exit(EXIT_FAILURE);} while(0);

char conf_value_ans[512] = {0};
char conf_ans[512] = {0};

char *get_conf(const char *conf, char *key) {
    FILE *fp = NULL;
    char *line = NULL, *sub = NULL;
    ssize_t nread;
    size_t len;
    //static char conf_value_ans[512] = {0};
    //optarg的实现，静态变量
    if (conf == NULL || key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if ((fp = fopen(conf, "r")) == NULL) {
        return NULL;
    }
    while ((nread = getline(&line, &len, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        if (line[strlen(key)] == '=') {
            strncpy(conf_value_ans, sub + strlen(key) + 1, nread - strlen(key) - 2);//判断最后一行是否有换行
            *(conf_value_ans + nread - strlen(key) - 2)= '\0';
            break;
        }
    }
    free(line);
    fclose(fp);
    //if (!strlen(conf_value_ans)) return NULL;
    if (sub == NULL) return NULL;
    return conf_value_ans;
}

char *get_conf1(const char *conf, char *key) {
    FILE *file;
    if (conf == NULL || key == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if ((file = fopen(conf, "r")) == NULL) {
        perror("fopen()");
        exit(1);
    }
    static char buff[1024] = {0};
    while (fgets(buff, sizeof(buff), file) != NULL) {
        if (strncmp(buff, key, strlen(key)) == 0) {
            return (buff + strlen(key) + 1);
        }
        memset(buff, 0, sizeof(buff));
    }
    return NULL;
}

//footballgame config_get
char *get_conf_value(const char *path, const char *key) {
    FILE *fp = NULL;
    char *line = NULL, *sub = NULL;
    ssize_t nread;
    size_t size;
    if (key == NULL || path == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if ((fp = fopen(path, "r")) == NULL) {
        handle_error("fopen()");
    }
    while ((nread = getline(&line, &size, fp)) != -1) {
        if (strstr(line, key) != NULL) continue;
        if (line[strlen(key)] == ':') {
            strncpy(conf_ans, sub + strlen(key) + 1, nread - strlen(key) - 2); //如果最后一行没有换行
            *(conf_ans + nread - strlen(key) - 2) = '\0';
            break;
        }
    }
    free(line);
    fclose(fp);
    if (sub == NULL) return NULL;
    return conf_ans;
}

int make_non_block(int fd) {
    int flag;
    if (flag == fcntl(fd, F_GETFL) < 0) {
        return -1;
    }
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
    return 0;
}

int make_block(int fd) {
    int flag;
    if (flag == fcntl(fd, F_GETFL) < 0) {
        return -1;
    }
    flag &= (~O_NONBLOCK);
    fcntl(fd, F_SETFL, flag);
    return 0;
}

int socket_create(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    //实现延迟关闭
    struct linger m_linger;
    m_linger.l_onoff = 1;
    m_linger.l_linger = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &m_linger, (socklen_t)sizeof(m_linger)) < 0) {
        return -1;
    }
    int flag = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    if (listen(sockfd, 10) < 0) {
        return -1;
    }
    return sockfd;
}

int socket_connect(char *ip, int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    return sockfd;
}

int socket_connect_timeout(char *ip, int port, long timeout) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    make_non_block(sockfd);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;

    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        int retval, error = -1;
        int len = sizeof(int);
        retval = select(sockfd + 1, NULL, &wfds, NULL, &tv);
        if (retval < 0) {
            close(sockfd);
            return -1;
        } else if (retval) {
            if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len) < 0) {
                close(sockfd);
                return -1;
            } 
            if (error) {
                close(sockfd);
                return -1;
            }
        } else {
            printf("Connect Time Out！\n");
            close(sockfd);
            return -1;
        }
    }
    make_block(sockfd);
    return sockfd;
}

//UDP
int socket_create_udp(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        handle_error("socket()");
    }
    //延时关闭
    struct linger m_linger;
    m_linger.l_onoff = 1;
    m_linger.l_linger = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &m_linger, (socklen_t)sizeof(m_linger)) < 0) {
        return -1;
    }
    int flag = 1;
    //设置地址重用
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
        return -1;
    }
    //设置为非阻塞套接字
    make_non_block(sockfd);
    //绑定INADDR_ANY & port
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }
    return sockfd;
}
//创建一个主动的UDP套接字
int socket_udp() {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        handle_error("socket()");
    }
    return sockfd;
}
