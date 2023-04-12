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
	data.clientID.cstring = "zhou001";	//客户端ID
	data.keepAliveInterval = 30;	//存活时间，如果30秒内设备没有向服务器发送ping数据就会短线；
	data.cleansession = 1;	//清除会话
	data.username.cstring = "admin";	//用户名
	// data.password.cstring = "admin";	//密码
	data.password.cstring = "password";	//密码

	
	//打包和发送connect数据
	int len = MQTTSerialize_connect(buf, bufsize, &data);			// 1. 打包向mqtt服务端连接的请求
	send(sock, buf, len, 0);										// 2. 通过socket连接
	printf("Connecting...\n");
	recv(sock, buf, bufsize, 0);	//等待连接
	
	//创建ping线程, 25秒ping一次
	pthread_create(&ping, NULL, ping_thread, NULL);
	
	//订阅主题
	MQTTString topicString = MQTTString_initializer;				// 1. 创建 mqtt主题 数据结构
	int msgid = 1;	//消息id
	int req_qos = 0;	//消息质量
	topicString.cstring = "topic1";	//主题名称
	len = MQTTSerialize_subscribe(buf, bufsize, 0, msgid, 1, &topicString, &req_qos);	// 2. 打包subscribe数据， 告知mqtt-server我方分订阅的主题
	send(sock, buf, len, 0);	// 3. 发送subscribe请求数据
	printf("Subcribing...\n");
	
	while(1)
	{
		len = recv(sock, buf, bufsize, 0);
		if(len > 0)
		{
			if(buf[0] == 0x30)	//判断接收到的消息是否是publish				// enum 自定义
			{
				//解包publish数据
				unsigned char dup;
				int qos;
				unsigned char retained;
				unsigned short msgid;
				int payloadlen_in;
				unsigned char* payload_in;	//指向payload
				int rc;
				MQTTString receivedTopic;
				MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic,
						&payload_in, &payloadlen_in, buf, bufsize);					// 解析订阅的publish数据包。
				printf("\nmessage arrived %*s\n", payloadlen_in, payload_in);
				printf(" \n\nmessage arrived %.*s\n", payloadlen_in, payload_in);
				printf(" \n\nmessage arrived %s \n\n", payload_in);
			}
		}
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
