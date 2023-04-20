#ifndef _MQTT_H_
#define _MQTT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "conf_service.h"
#include "conf_channel.h"
#include "services.h"
#include "MQTTPacket.h"

#define MAX_MQTT_LINKS 16

/*
 * mqtt �ַ��� */
typedef struct {
    int len;
    char str[128];
} mqtt_string;

/* mqtt ���ݻ����� */
typedef struct {
    pthread_mutex_t mutex;
} mqtt_data;

/*
 * mqtt ��·��Ϣ */
typedef struct {
    _Bool is_free; //

    char send_topic[64];
    char recv_topic[64];
    unsigned int send_qos;
    unsigned int recv_qos;
    pthread_mutex_t mutex;

    mqtt_data send;     //��Ҫ���͵�publish��Ϣ
    mqtt_data recv;     //�Ѿ����յ�publish��Ϣ
} mqtt_link_t;

/*
 * mqtt����ṹ�� */
typedef struct {
    channel_info_t *p_channel;	// ͨ����Ϣ��ip���˿�
    mqtt_service_t *p_client;	// �˺š���������

    pthread_mutex_t mutex;
    _Bool connected;
    unsigned short pkg_id;

    time_t recv_time;
    time_t send_time;
    time_t ping_time;
} mqtt_svr_arg;

extern mqtt_svr_arg *p_mqtt;

#ifdef  __cplusplus
extern "C" {
#endif

void mqtt_connect_retry(srv_args_t* srv_args);
_Bool mqtt_subscribe(srv_args_t* srv_args);

int mqtt_read(srv_args_t* p_args, unsigned char* buf, int len);
int mqtt_write(srv_args_t* p_args, const unsigned char *buf, const int len);

/*
 * ��۽ڵ㶩�ĵ���Ϣ����*/
void subscribe_msg_handle(srv_args_t* srv_args, unsigned char *buf, int len);
/*
 * ���յ���MQTT��Ϣ����*/
void mqtt_msg_handle(srv_args_t* srv_args, unsigned char *buf, int len);

void mqtt_send_publish_msg(srv_args_t* srv_args);
void mqtt_send_pingreq(srv_args_t* srv_args);

_Bool mqtt_send_and_query(srv_args_t* srv_args, \
        enum msgTypes type, unsigned short pkgid, \
        int qos, char *topic, char *data, uint32_t datalen);

#ifdef  __cplusplus
}
#endif

#endif /* _MQTT_H_ */
