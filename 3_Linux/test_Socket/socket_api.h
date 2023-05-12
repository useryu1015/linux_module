#ifndef __SOCK_H_
#define __SOCK_H_

#ifdef DEBUG
#define DEBUG_S if(0){      // 如果编译器不优化，会占用 垃圾内存
#define DEBUG_E }
#endif

typedef struct fd_client
{
    int fd;
} fd_client_t;

// socket的连接
typedef struct _socket_client {
    int count;
    fd_client_t cliAdd[1024];
} socket_client_t;




int server_open(const char *ip, int port, int *fd);       // 启动一条tcp监听通道；
int server_recv(int32_t fd, char *buf,int32_t len, int32_t flags);
void sock_option(int fd, void *value);

#endif