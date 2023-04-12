#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include "socket_api.h"
#define fd server_socket
#define BUFFER_SIZE 20

void* msgHandle(void *);
void* test_thread(void *);

char buffer[BUFFER_SIZE]; 


struct timeval timev  = {1, 0};        // 时限

int usr_main (int argc, const char * argv[])
{
    int server_socket;      // 网络套接字

    server_open("", 11333, &server_socket);     // listen();

    while (1) {
        // 接受一个客户端的tcp连接 创建socket跟客户端通信
        printf("wait client\n");
        
        struct sockaddr_in client_address;
        socklen_t address_len;


        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len);        // par1:fd      par2:RTN client addr        par3:RTN siezof addr
        // printf(" \n Test:  accept is %d ", client_socket);
        // printf("accept fd %d state is %d\n", client_socket, get_sock_state(client_socket));
        sock_option(client_socket,&timev);

        if (client_socket == -1) {
            perror("accept error");
            return -1;
        }
        printf("accept a client %d \n", client_socket);
        
        // 启动一个线程跟客户端进行通信
        pthread_t id;           // 不够严谨， try to array WHEN more client??   
        int rtn_pthr;
        rtn_pthr = pthread_create(&id, NULL, msgHandle, &client_socket);
        if (rtn_pthr != 0){
            perror("create thread wrong \n");
        }

        printf("ID = %d xx %d\n",id,&id);
        pthread_detach(id);     // 分离线程： 不受控
        //pthread_cancel();     // 取消
        
    }

    close(server_socket);
    
    return 0;
}


void* msgHandle(void *arg){         // 线程 函数
    int id = *(int*)arg;
 
    printf("client socket id:%d id:%d \n", id, arg);

    // 创建线程： 处理用户输入
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, test_thread, &id);       // create
    pthread_detach(thread_id);     // 分离线程： 不受控


    while (1){
        int buflen = recv(id,buffer,sizeof(buffer),MSG_DONTWAIT);       // 非阻塞    0也是阻塞？
        
        printf("阻塞测试 \n");  
        if (buflen > 0){
            printf("receive rtn:%d string:    %s \n", buflen, buffer);
            memset(buffer,0,sizeof(buffer));
        }

        if (buflen == 0){
            printf("对端socket 正常关闭 \n");
            break;
        }
        
        if (buflen == -1){
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue;
            
            perror("recv WRONG \n");
            break;
        }
    }

    printf("socket close \n");
    fflush(stdout); // 流控： 刷新缓冲区
    close(id);      // 关闭通道
    
    return 0;
}


void* test_thread(void *arg){
    char sendBuf[1024] = "  test ";


    while (1){
        sleep(1);
        fgets(sendBuf,sizeof(sendBuf),stdin);       // 阻塞等待用户输入；
        printf("接收标准输入: %s", sendBuf);

        send(*(int*)arg,sendBuf,strlen(sendBuf),0);     // 
        memset(sendBuf,0,sizeof(sendBuf));          // 清空发送缓存
        // printf("send done \n");
    }

}




