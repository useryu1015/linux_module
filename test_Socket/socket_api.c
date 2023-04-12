#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "socket_api.h"

// TCP.listen();
int server_open(const char *ip, int port, int32_t *fd)
{
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));        // bzero TEST;
    // server_addr.sin_len = sizeof(struct sockaddr_in);        // 为了适配xxx
    server_addr.sin_family = AF_INET;//Address families AF_INET互联网地址簇
    server_addr.sin_port = htons(11333);            // 指定端口号：111332
    server_addr.sin_addr.s_addr = inet_addr("192.168.101.73");  // 指定IP
    bzero(&(server_addr.sin_zero),8);               // 将前8个变量清零？     和memst同类

    // 创建socket
    /*
     * fd 文件描述符    同文件IO
     * para1：AF_INET：IPV4协议族？     AF_INET6：      
     *          AF_UNIX：多用于本地、进程间通信。   把原始数据直接从内核交互，而不经过TCP/IP协议； so 不能通过网络发送数据？
     * para2：SOCK_STREAM(面向连接的TCP套接字   SOCK_DGRAM(无连接、离散的UDP)
     * para3：指定协议类型？        
     */
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);//SOCK_STREAM 有连接        // step1: 启动xx服务        
    if (server_socket == -1) {
        perror("socket error");
        return 1;
    }
    system("netstat -an | grep 11333");	// 查看连接状态
    //绑定socket：将创建的socket绑定到本地的IP地址和端口，此socket是半相关的，只是负责侦听客户端的连接请求，并不能用于和客户端通信
    int bind_result = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));    // step2：绑定（struct）配置数据
    if (bind_result == -1) {
        perror("bind error");
        return 1;
    }
    system("netstat -an | grep 11333");	// 查看连接状态
    
    //listen侦听 第一个参数是套接字，第二个参数为等待接受的连接的队列的大小，在connect请求过来的时候,
    //完成三次握手后先将连接放到这个队列中，直到被accept处理。如果这个队列满了，且有新的连接的时候，对方可能会收到出错信息。
    if (listen(server_socket, 5) == -1) {                                           // step3：tell kernel is listen mode, kernel set TCP/TP WAIT_LISTEN
        perror("listen error");
        return 1;
    }

    printf("启动监听服务； ip:port \n");
    system("netstat -an | grep 11333");	// 查看连接状态

    *fd = server_socket;
    return 1;
}



// 非阻塞 接收tcp data
int server_recv(int32_t fd, char *buf,int32_t len, int32_t flags)
{
    int buflen=0,rs;
    while (rs)
    {
        buflen = recv(fd, buf, len, MSG_DONTWAIT);       // 非阻塞
        printf("buflen = %d \n", len);
        //printf("buflen = %d \n", sizeof(&buf));        // wrong: sizeof(buf) == char; 用二级指针？
        if(buflen < 0)
        {
            // 由于是非阻塞的模式,所以当buflen为EAGAIN时,表示当前缓冲区已无数据可读
            // 在这里就当作是该次事件已处理
            if(errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
                return buflen;
        }
        else if(buflen == 0)
        {
            // 这里表示对端的socket已正常关闭.
            printf("对端的socket已正常关闭. \n");
            close(fd);              // 关闭该连接，不在CLOSE_WAIT
            system("netstat -an | grep 11333");	// 查看连接状态
            return buflen;
        }
    
        if(buflen != len)
            rs = 0;
        else        // select 模式下，用不着怎么处理
            rs = 1; // 通常没recv完对端数据，需要再次读取
    }
    
    return buflen;
}



void sock_option(int fd, void *value)
{
   /*
    * param2:TCP_NODELAY是唯一使用IPPROTO_TCP层的选项，其他所有选项都使用SOL_SOCKET层
    * param3: option
    * param4: option`s value
    * param5: value`s size
    */
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)value, sizeof(*value)); // 设置接收超时时限     
    system("netstat -an | grep 11333");	// 查看连接状态
}