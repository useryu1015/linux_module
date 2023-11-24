#include <ctype.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
// #include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "zlog.h"
#include "crc.h"
#define fd server_socket
#define BUFFER_SIZE 1024

// void* msgHandle(void *);
// void* test_thread(void *);

char recvBuf[BUFFER_SIZE]; 
bool response_signal = false;
static int send_times = 0, rands;       // 随机向设备发送乱码


/*****     demo process         ****/
void sleep_ms(unsigned int secs)
{
	struct timeval tval;    
	
	tval.tv_sec=secs/1000;
	tval.tv_usec=(secs*1000)%1000000;
	select(0,NULL,NULL,NULL,&tval);
}

void CutSpace1(char* Res, char* Dec)
{
	for (; *Res != '\0'; Res++) {
		if (*Res != ' ')
			*Dec++ = *Res;//'++' 优先级比 '*'高，但是这里'++'作为后缀，先进行*Dec，再自加
	}

	*Dec = '\0';
	return;
}

int string2hex(char *buf, char *val)
{
	int i = 0;

	for(; i<strlen(buf)/2 /*&& i<sizeof(val)*/; i++) {               // BUG：2022年6月8日  sizo(val) 为4， 而非1024      use二级指针？
		sscanf(buf+i*2, "%2x", val+i);
	}

    return strlen(buf)/2;
}
/*****     end demo process         ****/







/* 
* 定义响应报文的数据帧 
* tips:  根据请求报文第一个OI的值，响应对应data的数据帧
*        OI： 2001  响应：data[0]
*        OI： 2101  响应：data[1]
*        OI:  2201  响应：data[2]
*/
char data[][256] = {
    // 点表所有标准信息
    "81 20 01 20 01 88 \
        20 02 20 01 66 \
        20 03 20 01 77 \
        20 04 40 07 e6 07 01 02 03 04 05 \
        21 01 05 06 e6 b5 8b e8 af 95 \
        21 02 04 06 11 22 33 44 55 66 \
        21 03 20 01 77 ",
    "81 21 03 20 01 31",             // 21
    // md66_sf6_density 所有点信息
    "81 22 01 04 02 55 66 22 02 26 04 11 44 22 44 22 03 26 04 22 33 44 11 22 04 26 04 11 22 67 44 22 05 26 04 33 33 33 44 22 06 26 04 11 22 33 44 22 07 26 04 11 22 33 44 22 08 26 04 11 22 33 44 22 09 26 04 55 66 77 88",
    "81 23 01 04 02 77 77 23 02 20 01 aa 23 03 26 04 11 22 33 44 23 04 26 04 11 22 33 44",
};
// 分帧响应报文 （乱序且重复）
char data_retrans[][128] = {
    "C1 20 02 20 01 16",                  // 分帧
    "C1 20 03 20 01 17",                  // 分帧2    
    "81 20 01 20 01 88 20 02 20 01 66 20 03 20 01 77 20 04 40 07 e6 07 01 02 03 04 05",     // 结束帧
    "81 20 00 41 0A 01 02 00 E6 07 01 02 03 04 05"                                          // struct 类型
};
// 异常响应报文
char data_error[][128] = {
    "81 1221313131232132132158439",
    "81 sadljajslkdjlkasjldalkjdl",
};


int rand_data_frame(int len)
{
    time_t tv;

    time(&tv);              // 1669020080




}


/* 根据接收的请求报文，组响应报文 */
int response_pack(char * pack, const char * recv_mes)
{
    uint8_t head_OI;
    int _data, mesLen;
    char frame_buf[1024], data_frame[512];
    char *ps = pack;

    memmove(pack, recv_mes, 2);         // 1. 帧头
    pack += 2;
    head_OI = *(recv_mes+4);

    // zlog_info("OI:  %02x  %02x", head_OI, *(recv_mes+5));
    switch (head_OI)
    {
        case 0x20:
            _data = 0;
            break;
        case 0x21:
            _data = 1;
            break;       
        default:
            _data = head_OI & 0x0F;           // 取低位
            // zlog_info(" _data: %d ", (head_OI & 0x0F));
            break;
    }
    
    /* data frame */
    send_times++;
    CutSpace1(data[_data], frame_buf);
    // if (_data == 0) {
    //     CutSpace1(data_retrans[send_times%4], frame_buf);             // 伪随机分帧响应
    // }
    uint8_t len = string2hex(frame_buf, data_frame);

    *pack = len;                        // 2. 帧长度
    pack += 1;

    memmove(pack, data_frame, len);     // 3. 数据帧
    pack += len;

    /* crc frame */
    uint16_t crc = MD66_CRC16_CALC(ps, pack - ps);
    memcpy(pack, &crc, sizeof(crc));    // 4. 校验帧

    mesLen = 2 + 1 + len + 2;
    return mesLen;
}


// 响应线程
void* test_thread(void *arg) {
    char sendBuf[1024];
    int slen;
           
    while (1){

        if (response_signal != true) {
            usleep(100);
            continue;
        }

        // fgets(sendBuf,sizeof(sendBuf),stdin);       // 阻塞等待用户输入；
        // printf("接收标准输入: %s", sendBuf);

        slen = response_pack(sendBuf, recvBuf);                // 根据recv数据， 组响应数据包
        hzlog_info("响应报文:", sendBuf, slen); printf("\n");
        send(*(int*)arg,sendBuf,slen,0);

        response_signal = false;
    }

}


// 接收线程
void* msgHandle(void *arg){
    int id = *(int*)arg;
    char sendBuf[1024];
    int slen;
 
    // 创建线程： 处理用户输入
    // pthread_t thread_id;
    // pthread_create(&thread_id, NULL, test_thread, &id);       // create
    // pthread_detach(thread_id);     // 分离线程： 不受控

    while (1){
        int buflen = recv(id,recvBuf,sizeof(recvBuf),0);      
        if (buflen <= 0) {
            if (buflen == -1){
                zlog_info("recv WRONG \n");
                break;
            }
            zlog_info("recv len <= 0 ");
            break;
            continue;
        }

        hzlog_info("接收报文:", recvBuf, buflen);
        /* 校验接收数据crc */
        if (!MD66_CRC16_CHECK(recvBuf, recvBuf + buflen - 2))
        {
            zlog_info("\n\n\n");
        }

        slen = response_pack(sendBuf, recvBuf);                // 根据recv数据， 组响应数据包
        hzlog_info("响应报文:", sendBuf, slen); printf("\n");
        send(*(int*)arg,sendBuf,slen,0);
        response_signal = true;

        sleep_ms(100);
    }

    fflush(stdout); // 流控： 刷新缓冲区
    close(id);      // 关闭通道
    
    return 0;
}


int main (int argc, const char * argv[])
{
    int32_t option = 1;
    char tcp_ip[256] = "0.0.0.0";
    uint16_t tcp_port = 512;
    struct sockaddr_in server_addr;

    printf("Usage: ./process ip port \n");

    if (argc >= 3 ) {
        memmove(tcp_ip, argv[1], strlen(argv[1]));
        tcp_port = atoi(argv[2]);        
    } else
        printf("default: ./process 0.0.0.0 512 \n");

    bzero(&server_addr,sizeof(server_addr));        // bzero TEST;
    server_addr.sin_family = AF_INET;//Address families AF_INET互联网地址簇
    server_addr.sin_port = htons(tcp_port);              // 指定端口号
    server_addr.sin_addr.s_addr = inet_addr(tcp_ip);  // 指定IP
    bzero(&(server_addr.sin_zero),8);               // 将前8个变量清零     和memset同类

    // printf ("  test  %d %d    \n",htons(16),server_addr.sin_addr.s_addr);
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

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(int32_t));

    //绑定socket：将创建的socket绑定到本地的IP地址和端口，此socket是半相关的，只是负责侦听客户端的连接请求，并不能用于和客户端通信
    int bind_result = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));    // step2：绑定（struct）配置数据
    if (bind_result == -1) {
        perror("bind error");
        return 1;
    }

    // printf("Test block \n");
    
    //listen侦听 第一个参数是套接字，第二个参数为等待接受的连接的队列的大小，在connect请求过来的时候,
    //完成三次握手后先将连接放到这个队列中，直到被accept处理。如果这个队列满了，且有新的连接的时候，对方可能会收到出错信息。
    if (listen(server_socket, 5) == -1) {                                           // step3：tell kernel is listen mode, kernel set TCP/TP WAIT_LISTEN
        perror("listen error");
        return 1;
    }
    // system("netstat -an | grep 11333");	// 查看连接状态
    // printf("\n 监听服务 listen(); \n");

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
         end test */

        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &address_len);        // par1:fd      par2:RTN client addr        par3:RTN siezof addr
        // printf(" \n Test:  accept is %d ", client_socket);
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

        // printf("ID = %d xx %d\n",id,&id);
//        pthread_join(id, NULL);
        pthread_detach(id);     // 分离线程： 不受控
        //pthread_cancel();
        sleep(1);

        //close(server_socket);
    }
    
    return 0;
}





