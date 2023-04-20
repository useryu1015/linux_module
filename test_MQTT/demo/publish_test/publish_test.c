#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <fcntl.h>
#include <pthread.h>
#include "MQTTPacket.h"

int sock;
pthread_t ping;
void * ping_thread(void * arg);
void TcpConnect(const char *host, int port);

int main(void)
{
	//建立TCP连接
	// TcpConnect("127.0.0.1", 1883);
	TcpConnect("192.168.101.33", 61613);

	
	//初始化连接参数，MQTTPacket_connectData_initializer定义在MQTTConnect.h
	unsigned char buf[256];
	int bufsize = sizeof(buf);
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.clientID.cstring = "zhou002";	//客户端ID
	data.keepAliveInterval = 30;	//存活时间，如果30秒内设备没有向服务器发送ping数据就会短线；
	data.cleansession = 1;	//清除会话
	data.username.cstring = "admin";	//用户名
	// data.password.cstring = "admin";	//密码
	data.password.cstring = "password";	//密码
	
	//打包和发送connect数据
	int len = MQTTSerialize_connect(buf, bufsize, &data);
	send(sock, buf, len, 0);
	printf("Connecting...\n");
	recv(sock, buf, bufsize, 0);	//等待连接
	
	//创建ping线程，25秒ping一次
	// pthread_create(&ping, NULL, ping_thread, NULL);
	
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = "topic1";
	unsigned char *payload = "hello";
	while(1)
	{
		sleep(2);
		len = MQTTSerialize_publish(buf, bufsize, 0, 0, 0, 0, topicString, payload, strlen(payload));
		send(sock, buf, len, 0);
		printf("message send : %s\n", payload);
	}
	
}

void TcpConnect(const char *host, int port)
{
	//创建套接字
	sock = socket(AF_INET, SOCK_STREAM, 0);
	//向服务器（特定IP和端口）发送请求
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(host);
	serv_addr.sin_port = htons(port);
	int res = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(res == 0)
		printf("TCP Connected\n");
	else {
		printf("TCP Connect error ................\n");
	}
}

void * ping_thread(void *arg)
{
	(void)arg;
	uint8_t buf[10];
	int bufsize = sizeof(buf);
	int len = 0;
	printf("Ping thread is running\n");
	while(1)
	{
		sleep(25);
		//打包和发送ping数据
		len = MQTTSerialize_pingreq(buf, bufsize);
		send(sock, buf, len, 0);
		printf("Ping...\n");
	}
}

