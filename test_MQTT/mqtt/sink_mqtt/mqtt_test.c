/*
 * mqtt_test.c
 *
 *  Created on: 2016Äê7ÔÂ11ÈÕ
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"

int mqtt_send(const int fd, const unsigned char *buf, const int len);
int mqtt_recv(const int fd, unsigned char* buf, int len);


void print_usage() {
	printf("-print\n");
	printf("-host [192.168.100.2] -port [12345]\n");
}

int socket_open(const char* host, uint16_t port) {
	int s = -1;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_port = htons(port);

	if (-1 == (s = socket(AF_INET, SOCK_STREAM, 0))) {
		return -1;
	}

	return (connect(s, (struct sockaddr*) &addr, sizeof(addr)) == 0) ? s : -1;
}

const char* con_ack_str[] = {
		"Connection Accepted",
		"Connection Refused, unacceptable protocol version",
		"Connection Refused, identifier rejected",
		"Connection Refused, Server unavailable",
		"Connection Refused, bad user name or password",
		"Connection Refused, not authorized",
		"Connection Refused, other reason"};

_Bool mqtt_connect(const int fd) {
	int len = 0;
	unsigned char buf[2048] = {0}, sessionPresent, ack;
	_Bool ret = false;
	MQTTPacket_connectData args = MQTTPacket_connectData_initializer;

	len = MQTTSerialize_connect(buf, sizeof(buf), &args);
	if (len <= 0)
		return false;

	if (len != mqtt_send(fd, (const char*)buf, len)){
		printf("send connect data failed!\n");
		return false;
	}

	if ((len = mqtt_recv(fd, (const char*)buf, sizeof(buf))) < 0) {
		printf("mqtt recv len:%d \n", len);
		return false;
	}

	if (0 != MQTTDeserialize_connack(&sessionPresent, &ack, buf, len)) {
		ret = (0 == ack);
		printf("%s", (ack>5) ? con_ack_str[6] : con_ack_str[ack]);
	}

	if (!ret) {
		close(fd);
	}

	return ret;
}

_Bool mqtt_subscribe(const int fd) {
	_Bool ret = false;
	unsigned char buf[2048] = {0};
	MQTTString topicStrings[2] = { MQTTString_initializer, MQTTString_initializer };
	int req_qoss[2] = {2, 2}, len = 0, count = 0, i = 0;
	unsigned short packet_id = 0;
	int req_qoss2[2] = {0, 0};

	topicStrings[0].cstring = "iec104";
	topicStrings[1].cstring = "gdw376";

	len = MQTTSerialize_subscribe(buf, sizeof(buf), 0, 1, 2, topicStrings, req_qoss);
	if (len <= 0)
		return false;

	if (len != mqtt_send(fd, (const char*)buf, len)){
		printf("send subscribe data failed!\n");
		return false;
	}

	if ((len = mqtt_recv(fd, (const char*)buf, sizeof(buf))) < 0) {
		printf("mqtt recv len:%d \n", len);
		return false;
	}

	ret = (1 == MQTTDeserialize_suback(&packet_id, 2, &count, req_qoss2, buf, len));

	if (ret) {
		printf("packet_id = %d, count = %d\n", packet_id, count);
		for (i = 0; i < count; ++i)
			printf("req_qos[%d] = %d\n", i, req_qoss2[i]);
	}

	return ret;
}


void response_publish_message(const unsigned char *buf, const int len) {
	unsigned char dup, retained, *payload = NULL;
	unsigned char send_buf[2048] = {0};
	unsigned short packetid;
	int payload_len = 0, qos = 0, resp_len = 0;
	MQTTString topic = MQTTString_initializer;

	if (0 == MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topic,\
			&payload, &payload_len, buf, len))
		return;

	if (1 == qos) {
		resp_len = MQTTSerialize_ack(send_buf, sizeof(send_buf), PUBACK, packetid, 1, 0);
	} else if (2 == qos) {
		resp_len = MQTTSerialize_ack(send_buf, sizeof(send_buf), PUBREC, packetid, 2, dup);
	}
}


int main(int argc, char* argv[]) {
	// char host[32] = "192.168.100.115";
	char host[32] = "192.168.101.33";
	unsigned char buf[2048] = {0};
	int i = 0, port = 1883, len = 0, fd = -1;
	port = 61613;

	if ((fd = socket_open(host, port)) < 0) {
		printf("connect to %s:%d error\n", host, port);
		return EXIT_FAILURE;
	}

	if (!mqtt_connect(fd)) {
		printf("mqtt connect failed\n");
		return EXIT_FAILURE;
	}

	if (!mqtt_subscribe(fd)) {
		printf("mqtt subscribe failed\n");
		return EXIT_FAILURE;
	}

	while (true) {
		len = mqtt_recv(fd, buf, sizeof(buf));

		if(len > 0) {
			printf("recv %d hex data: ", len);
			for (i = 0; i < len; ++i) {
				printf("0x%02X ", buf[i]);
			}
			printf("\n");
		}

		switch (buf[0]) {
		case CONNACK:
		case PUBLISH:
		case PUBACK:
		case PUBREC:
		case PUBREL:
		case PUBCOMP:
		case SUBSCRIBE:
		case SUBACK:
		case UNSUBSCRIBE:
		case UNSUBACK:
		case PINGREQ:
		case PINGRESP:
		case DISCONNECT:
			break;
		default:
			break;
		}

	}

	return EXIT_SUCCESS;
}

