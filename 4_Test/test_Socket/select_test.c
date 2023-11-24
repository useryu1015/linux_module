/***************************************************
 * 1、同时处理多个client数据收发
 * 2、recv的多种情况处理： 长数据、
 * 3、
 **************************************************/
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
#include "socket_api.h"
// #define fd server_socket
#define BUFFER_SIZE 1024

void* mes_handle(void *arg);
int mes_process(void);
int usr_main(void);


fd_set tfds, rfds;
socket_client_t cli;        // FD_SETSIZE记录客户端的连接
char buffer[BUFFER_SIZE];
struct timeval tv  = {1, 0};     

int main (int argc, const char * argv[])
{
    int server_socket;
    int count = 0;
    
    server_open("", 11333, &server_socket);     // listen();

    if (1){
        // 启动一个线程跟客户端进行通信
        pthread_t id;           // 不够严谨， try to array WHEN more client??
        if ( pthread_create(&id, NULL, mes_handle, NULL) != 0){
            perror("create thread wrong \n");
            return 1;
        }
        pthread_detach(id);     // 分离线程： 不受控
        //pthread_cancel();     // 取消
    }


    while (1) {
        // 接受一个客户端的tcp连接 创建socket跟客户端通信
        printf("wait client\n");
        
        struct sockaddr_in client_address;
        socklen_t address_len;

        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len);        // par1:fd      par2:RTN client addr        par3:RTN siezof addr
        printf("accept a client %d \n", client_socket);
        if (client_socket == -1) {
            perror("accept error");
            return -1;
        }
        sock_option(client_socket,&tv);         // 结构体参数 传递。。。

        // FD_SET(client_socket, &rfds);
        cli.cliAdd[cli.count].fd = client_socket;       // ...  wrong: close后
        cli.count += 1;         // 每接收一个接连

    } // while

    close(server_socket);    
    return 0;
}


// 处理收发数据
void* mes_handle(void *arg)
{
    while (1)
    {
        mes_process();
        usr_main();
        /* code */
    }
}

// 非阻塞： 遍历所有监听端口的接收数据
int mes_process(void)
{
    int i,rtn;
    int currfd;

    char *num = "123456789111";
    char num2[] = "zhaoxinyu";
    int num3 = 12;

    do {
        FD_ZERO(&rfds);             // 清空
        for (i=0; i<cli.count; i++){
            FD_SET(cli.cliAdd[i].fd, &rfds);
        }

        rtn = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);      // 监视fd是否有数据发生？ 以便处理
        if (rtn < 0){
            printf("select error, %s(%d)", strerror(errno), errno);
            return 0;
        } else if (rtn == 0){
            // printf("超时 \n");
            return 0;
        } else {
            printf("select ok sum: %d  \n", rtn);
        }

        // for (i=0; i<cli.count; i++){
            // if (!FD_ISSET(cli.cliAdd[yes], &rfds))
        for (currfd = 0; currfd < FD_SETSIZE; currfd++) {
            // tips: select()成功返回时会将未准备好的描述符位清零， so: 通过FD_ISSET判断可用的fd
            if (!FD_ISSET(currfd, &rfds))   //检查currfd是否在集合中，直到找到为止，最大1024  
                continue;

            printf("socket fd=%d in select run. FD_SETSIZE=%d\n", currfd, FD_SETSIZE);

            int sta = server_recv(currfd, buffer, sizeof(buffer), 0);        // rcve_main
            if (sta == 0){
                cli.count-=1;
            }
            printf("recv(%d): %s", sta, buffer);
            memset(buffer,0,sizeof(buffer));

            //printf("\n zhao_test :   sizeof=%d . sizeof*=%d . sizeofchar=%d . sizeofchar[]=%d \n", sizeof(buffer), sizeof(&num3), sizeof(num), sizeof(num2));
        }



    } while (0);
        
    // printf(" ");

    return 0;
}


int usr_main(void)
{

    // 根据xx响应xx
    switch (buffer)
    {
    case "hi":
        send(fd,"hello");
        break;
    case "parameter_Get"
        send();
        break;
    default:
        break;
    }


}