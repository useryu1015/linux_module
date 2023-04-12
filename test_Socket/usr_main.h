#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
// #include <iostream>
#include <stdio.h>
#define fd server_socket
#define BUFFER_SIZE 1024

void* msgHandle(void *);
void* test_thread(void *);

char buffer[BUFFER_SIZE]; 

int main (int argc, const char * argv[])
{
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));        // bzero TEST;
    // server_addr.sin_len = sizeof(struct sockaddr_in);        // 为了适配xxx
    server_addr.sin_family = AF_INET;//Address families AF_INET互联网地址簇
    server_addr.sin_port = htons(11333);            // 指定端口号：111332
    server_addr.sin_addr.s_addr = inet_addr("192.168.101.73");  // 指定IP
    bzero(&(server_addr.sin_zero),8);               // 将前8个变量清零？     和memst同类

    printf ("  test  %d %d    \n",htons(16),server_addr.sin_addr.s_addr);
    //创建socket
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
    printf("Test block \n");
    
    //listen侦听 第一个参数是套接字，第二个参数为等待接受的连接的队列的大小，在connect请求过来的时候,
    //完成三次握手后先将连接放到这个队列中，直到被accept处理。如果这个队列满了，且有新的连接的时候，对方可能会收到出错信息。
    if (listen(server_socket, 5) == -1) {                                           // step3：tell kernel is listen mode, kernel set TCP/TP WAIT_LISTEN
        perror("listen error");
        return 1;
    }
    system("netstat -an | grep 11333");	// 查看连接状态
    printf("\n 监听服务 listen(); \n");






    
    
    while (1) {
        // 接受一个客户端的tcp连接 创建socket跟客户端通信
        printf("wait client\n");
        
        struct sockaddr_in client_address;
        socklen_t address_len;

        /* 多个client test 
        // client_address++;
        // MQTT topic mini_test             
            client_sub -> connect
                topic: struct{ ip,port,topic};
            publish -> send(date.topic);
            QOS:
                if(signal)
        // HTTP:    UDP/TCP
            request： clinet -> connect
            responce: server -> send(.html);
        /* end test */

        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len);        // par1:fd      par2:RTN client addr        par3:RTN siezof addr
        printf(" \n Test:  accept is %d ", client_socket);
        /*
         * param2:TCP_NODELAY是唯一使用IPPROTO_TCP层的选项，其他所有选项都使用SOL_SOCKET层
         * param3: option
         * param4: option`s value
         * param5: value`s size
         */
        setsockopt(client_socket, SOL_SOCKET, SO_BROADCAST, NULL, sizeof(int)); // 函数用于任意类型、任意状态套接口的设置选项值   // 定义了最高的“套接口”层次上的选项。     
        system("netstat -an | grep 11333");	// 查看连接状态

        if (client_socket == -1) {
            perror("accept error");
            return -1;
        }
        printf("accept client\n");
        
        // 启动一个线程跟客户端进行通信
        pthread_t id;           // 不够严谨， try to array WHEN more client??   
        int rtn_pthr;
        rtn_pthr = pthread_create(&id, NULL, msgHandle, &client_socket);
        if (rtn_pthr != 0){
            perror("create thread wrong \n");
        }

        printf("ID = %d xx %d\n",id,&id);
//        pthread_join(id, NULL);
        pthread_detach(id);     // 分离线程： 不受控
        //pthread_cancel();
        sleep(1);

        //close(server_socket);
    }
    
    return 0;
}


void* msgHandle(void *arg){         // 线程 函数
    int id = *(int*)arg;
 
    printf("client socket id:%d id:%d \n", id, arg);

    // 创建线程： 处理用户输入
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, test_thread, &id);       // create
    pthread_detach(thread_id);     // 分离线程： 不受控

    char *buf = "recv OK";
    while (1){
        int buflen = recv(id,buffer,sizeof(buffer),0);      

        if (buflen > 0){
            printf("receive rtn:%d sting: %s \n", buflen, buffer);
            memset(buffer,0,sizeof(buffer));
        }

        // sleep(1);
        // int send_rtn = send(id,buf,strlen(buf),0);
        // printf("send_rtn: %d  sendSum: %d \n", send_rtn, strlen(buf));
        
        if (buflen == -1){
            perror("recv WRONG \n");
            break;
        }
    }

    fflush(stdout); // 流控： 刷新缓冲区
    close(id);      // 关闭通道
    
    return 0;
}


void* test_thread(void *arg){
    char sendBuf[1024];
           
    while (1){
        fgets(sendBuf,sizeof(sendBuf),stdin);       // 阻塞等待用户输入；
        printf("接收标准输入: %s", sendBuf);

        send(*(int*)arg,sendBuf,strlen(sendBuf),0);
        memset(sendBuf,0,sizeof(sendBuf));          // 清空发送缓存
        // printf("send done \n");
    }

}




