#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "MQTTPublish.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"

#include "mqtt.h"
#include "zlog.h"
#include "services.h"
#include "pdiot.h"
#include "conf_sink.h"

const char* con_ack_str[] = {
        "Connection Accepted",
        "Connection Refused, unacceptable protocol version",
        "Connection Refused, identifier rejected",
        "Connection Refused, Server unavailable",
        "Connection Refused, bad user name or password",
        "Connection Refused, not authorized",
        "Connection Refused, other reason"};

typedef struct {
    enum msgTypes type;
    int (*make_packge)(unsigned char *buf, int len, \
            unsigned short sid, int qos, char *topic, \
            char *data, int datalen);
    _Bool (*judge_func)(unsigned char* rp, int len, \
            int qos, unsigned short sid);
    int retry, mdelay;
} mqtt_type_op;

int make_connect_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_publish_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_puback_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_pubrec_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_pubrel_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_pubcomp_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_subscribe_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);
int make_pingreq_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen);

_Bool connect_reply_judge(unsigned char *rp, int len, int qos, unsigned short sid);
_Bool publish_reply_judge(unsigned char *rp, int len, int qos, unsigned short sid);
_Bool pubrec_reply_judge(unsigned char *rp, int len, int qos, unsigned short sid);
_Bool pubrel_reply_judge(unsigned char *rp, int len, int qos, unsigned short sid);
_Bool pingreq_reply_judge(unsigned char *rp, int len, int qos, unsigned short sid);

mqtt_type_op type_ops[] = {
        {CONNECT, make_connect_packge, connect_reply_judge, 30, 200},
        {PUBLISH, make_publish_packge, publish_reply_judge, 10, 200},
        {PUBACK, make_puback_packge, NULL, 0, 0},
        {PUBREC, make_pubrec_packge, pubrec_reply_judge, 10, 200},
        {PUBREL, make_pubrel_packge, pubrel_reply_judge, 10, 200},
        {PUBCOMP, make_pubcomp_packge, NULL, 0, 0},
        {SUBSCRIBE, make_subscribe_packge, NULL, 0, 0},
        {PINGREQ, make_pingreq_packge, pingreq_reply_judge, 10, 200}};

static void increase_pkgid()
{
    if (0xffff == ++p_mqtt->pkg_id)
        p_mqtt->pkg_id = 1;
}

mqtt_type_op* get_mqtt_type_op(enum msgTypes type)
{
    int i = 0;

    for (i = 0; i < sizeof(type_ops)/sizeof(type_ops[0]); ++i) {
        if (type != type_ops[i].type)
            continue;

        return &(type_ops[i]);
    }

    return NULL;
}


_Bool mqtt_send_and_query(srv_args_t* srv_args, \
        enum msgTypes type, unsigned short pkgid, \
        int qos, char *topic, char *data, uint32_t datalen)
{
    mqtt_type_op *p_op = NULL;
    unsigned char *pbuf = NULL, reply[2048] = {0};
    int nread = 0, nwrite = 0, i = 0;
    int base_len = 2048, buf_len = 2048, pkg_len = 0;

    if (!(p_op = get_mqtt_type_op(type)))           // 连接、推送、订阅等mqtt消息类型 功能块函数
        return false;

    pbuf = malloc(buf_len);
    while(true) {
        pkg_len = p_op->make_packge(pbuf, buf_len, pkgid, qos, topic, data, datalen);

        if (MQTTPACKET_BUFFER_TOO_SHORT != pkg_len)
            break;

        buf_len += base_len;
        free(pbuf);
        pbuf = malloc(buf_len);
    }

    zlog_info("mqtt pre send %d msg", pkg_len);
    hlog_info(pbuf, pkg_len);
    nwrite = mqtt_write(srv_args, pbuf, pkg_len);
    free(pbuf);

    if (nwrite < 0) {
        set_running(srv_args->p_stats, false);
        return false;
    }

    if (pkg_len != nwrite)
        return false;

    p_mqtt->send_time = time(NULL);
    stat_io_write(srv_args->p_stats, nwrite);
    if (PINGREQ == type && 0 == p_mqtt->ping_time) {
        p_mqtt->ping_time = p_mqtt->send_time;
    }

    if (!p_op->judge_func)
        return true;

    //qos为0的发布消息不需要确认
    if ((PUBLISH == type) && (0 == qos))
        return true;

    for (i = 0; i < p_op->retry+1; ++i) {
        usleep(p_op->mdelay * 1000);

        nread = mqtt_read(srv_args, reply, sizeof(reply));
        if (0 > nread) {
            return false;
        } else if (0 == nread) {
            continue;
        } else {
            p_mqtt->recv_time = time(NULL);
            stat_io_read(srv_args->p_stats, nread);
        }

        mqtt_msg_handle(srv_args, reply, nread);
        if (!p_op->judge_func(reply, nread, (PUBLISH==type) ? qos : 0, pkgid))
            continue;

        return true;
    }

    return false;
}

/* mqtt与主站链接，并在失败后重试 */
void mqtt_connect_retry(srv_args_t* srv_args)
{
    while (true) {
        zlog_info("connect to mqtt service");
        p_mqtt->connected = mqtt_send_and_query(\
                srv_args, CONNECT, 0, 0, NULL, NULL, 0);

        if (p_mqtt->connected)
            break;

        usleep(1000 * 100);
    }
}


void response_publish_msg(srv_args_t* srv_args, int qos, \
        unsigned short pkg_id, unsigned char dup)
{
    if (1 == qos) {
        mqtt_send_and_query(srv_args, PUBACK, \
                pkg_id, qos, NULL, NULL, 0);
    } else if (2 == qos) {
        if (!mqtt_send_and_query(srv_args, PUBREC, \
                pkg_id, qos, NULL, NULL, 0))
            return;

        mqtt_send_and_query(srv_args, PUBCOMP, \
                pkg_id, qos, NULL, NULL, 0);
    }
}

void mqtt_send_pingreq(srv_args_t* srv_args)
{
    int escape_time = time(NULL) - p_mqtt->send_time;

    /*每隔半个超时时间就发送一个心跳*/
    /*zlog_info("last:%d, timeout:%d, escape:%d", p_mqtt->send_time,\
            p_mqtt->p_datalink->timeout, escape_time);*/
    if (escape_time < p_mqtt->p_channel->timeout / 2000)
        return;

    if (mqtt_send_and_query(srv_args, PINGREQ, 0, 0, NULL, NULL, 0))
        p_mqtt->ping_time = 0;
    zlog_debug("mqtt send pingreq");
}


int make_connect_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    MQTTPacket_connectData args = MQTTPacket_connectData_initializer;

    args.clientID.cstring = p_mqtt->p_client->client_id;
    args.username.cstring = p_mqtt->p_client->username;
    args.password.cstring = p_mqtt->p_client->password;
    args.keepAliveInterval = p_mqtt->p_channel->timeout / 1000; //ms to s

    return MQTTSerialize_connect(buf, len, &args);
}

int make_subscribe_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    MQTTString *p_topic = NULL;
    int i = 0, *p_wqos = NULL, ntopic = p_mqtt->p_client->ntopic;

    p_topic = calloc(ntopic, sizeof(MQTTString));
    p_wqos = calloc(ntopic, sizeof(int));

    if (!p_topic || !p_wqos)
        return 0;

    for (i = 0; i < ntopic; ++i) {
        p_topic[i].cstring = p_mqtt->p_client->pp_topic[i]->name;
        p_wqos[i] = p_mqtt->p_client->pp_topic[i]->qos;
    }

    return MQTTSerialize_subscribe(buf, len, 0, 1, ntopic, p_topic, p_wqos);
}

int make_publish_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    MQTTString _topic = MQTTString_initializer;

    _topic.cstring = topic;
    return MQTTSerialize_publish(buf, len, 0, qos, \
            1, sid, _topic, data, datalen);
}

int make_puback_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    return MQTTSerialize_puback(buf, len, sid);
}

int make_pubrec_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    return MQTTSerialize_ack(buf, len, PUBREC, sid, 0, 0);
}

int make_pubrel_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    return MQTTSerialize_ack(buf, len, PUBREL, sid, 1, 0);
}

int make_pubcomp_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    return MQTTSerialize_ack(buf, len, PUBCOMP, sid, 0, 0);
}

int make_pingreq_packge(unsigned char *buf, int len, \
        unsigned short sid, int qos, char *topic, \
        char *data, int datalen)
{
    return MQTTSerialize_pingreq(buf, len);
}

_Bool connect_reply_judge(unsigned char *rp, int len, \
        int qos, unsigned short sid)
{
    unsigned char session = 0xff, ack = 0xff;

    if (0 != MQTTDeserialize_connack(&session, &ack, rp, len)) {
        zlog_info("%s", (ack>5) ? con_ack_str[6] : con_ack_str[ack]);
    }

    return (0 == ack);
}

_Bool publish_reply_judge(unsigned char *rp, int len, \
        int qos, unsigned short sid)
{
    _Bool ret = false;
    unsigned short rid = 0;

    if (len < 4)
        return false;

    rid = rp[2]*256 + rp[3];
    if (1 == qos) {
        ret = (((rp[0]>>4) == PUBACK) && (sid == rid));
    } else if (2 == qos) {
        ret = (((rp[0]>>4) == PUBREC) && (sid == rid));
    }

    return ret;
}

_Bool pubrec_reply_judge(unsigned char *rp, int len, \
        int qos, unsigned short sid)
{
    unsigned short rid = 0;

    if (len < 4)
        return false;

    rid = rp[2] * 256 + rp[3];
    return (((rp[0] >> 4) == PUBREL) && (sid == rid));
}

_Bool pubrel_reply_judge(unsigned char *rp, int len, \
        int qos, unsigned short sid)
{
    unsigned short rid = 0;

    if (len < 4)
        return false;

    rid = rp[2] * 256 + rp[3];
    return (((rp[0] >> 4) == PUBCOMP) && (sid == rid));
}

_Bool pingreq_reply_judge(unsigned char *rp, int len, \
        int qos, unsigned short sid)
{
    return ((rp[0] >> 4) == PINGRESP);
}


#include "pdiot.h"
#include "cJSON.h"

char* format_topic(char *buf, char *topic)
{
    sprintf(buf, "v1/devices/%s/%s", \
            SINK.service_config.mqtt.client_id, \
            topic);
    return buf;
}

typedef _Bool (*json_handler)(cJSON *json);

typedef struct _topic_handler_t
{
    char topic[128];
    json_handler handler;
} topic_handler_t;

/*
 * 汇聚节点订阅的消息处理函数*/
topic_handler_t subscribe_handler[] = {
        {"topo/addResponse",    Pdiot_ParseAddResponse},
        {"topo/deleteResponse", Pdiot_ParseDeleteResponse},
        {"topo/updateResponse", Pdiot_ParseUpdateResponse},
        {"command",             Pdiot_ParseCommand}
};

/*
 * 汇聚节点推送的消息处理函数*/
topic_handler_t publish_handler[] = {
        {"addResponse",     Pdiot_ParseAddResponse},
        {"deleteResponse",  Pdiot_ParseDeleteResponse},
        {"updateResponse",  Pdiot_ParseUpdateResponse},
        {"command",         Pdiot_ParseCommand}
};

// get主题对应的功能块
static json_handler get_subscribe_handler(MQTTString *p_topic)
{
    int i = 0;

    for (i = 0; i < sizeof(subscribe_handler) \
            / sizeof(subscribe_handler[0]); ++i)
    {
        char buf[256] = {0};
        format_topic(buf, subscribe_handler[i].topic);

        printf("topic is %s\n", buf);
        if (MQTTPacket_equals(p_topic, buf)) {
            printf("%s topic matched!\n", buf);
            return subscribe_handler[i].handler;        // topic对应的 程序
        }
    }

    return NULL;
}


/*
 * 汇聚节点订阅的消息处理*/
void subscribe_msg_handle(srv_args_t* srv_args, unsigned char *buf, int len)
{
    unsigned char dup, retained, *msg = NULL;
    unsigned short packetid;
    int nmsg = 0, qos = 0;
    MQTTString topic = MQTTString_initializer;

    if (0 == MQTTDeserialize_publish(&dup, &qos, &retained, \
            &packetid, &topic, &msg, &nmsg, buf, len))
        return;

    zlog_info("msg: %s", msg);

    json_handler handler = get_subscribe_handler(&topic);       // 任务句柄：topic对应功能块
    if (!handler)
        return;

    cJSON *json = cJSON_Parse(msg);
    handler(json);                      					// 1. 处理plat的消息
    response_publish_msg(srv_args, qos, packetid, dup);		// 2. 回复plat接收状态？
}

/*
 * 接收到的MQTT消息处理*/
void mqtt_msg_handle(srv_args_t* srv_args, unsigned char *buf, int len)
{
    if (!buf || len <= 0)
        return;

    zlog_info("RECV mqtt data:");
    hlog_info(buf, len);

    switch (buf[0] >> 4) {			// 根据接收到的mqtt消息类型xxx
	    case PUBLISH:
	        subscribe_msg_handle(srv_args, buf, len);
	        break;
	    case PINGRESP:
	        p_mqtt->ping_time = 0;
	        break;
	    default:
	        break;
    }
}

void mqtt_register_device(srv_args_t* srv_args)
{
    cJSON *json = cJSON_CreateObject();
    Pdiot_PrintAddDeviceInfo(json);
    char* json_str = cJSON_Print(json), topic[64] = {0};
    cJSON_Delete(json);

    if (mqtt_send_and_query(srv_args, PUBLISH, \
            p_mqtt->pkg_id, 0, format_topic(topic, "topo/add"), \
            json_str, strlen(json_str)))
    {
        zlog_info("send publish msg success, %s", json_str);
    }

    free(json_str);
}


/*
 * 设备在线状态更新*/
void online_status_handle(srv_args_t* srv_args)
{
    int qos = 0;
    char *resp = NULL, topic[256] = {0};

    format_topic(topic, "commandResponse");
    while (true)
    {
        if (!get_command_result(&resp, &qos))
            break;

        if (mqtt_send_and_query(srv_args, PUBLISH, \
                p_mqtt->pkg_id, qos, topic, resp, strlen(resp)))
        {
            if (2 == qos) {
                if (mqtt_send_and_query(srv_args, PUBREL, \
                        p_mqtt->pkg_id, qos, NULL, NULL, 0))
                    zlog_info("send publish msg success, %s", resp);
                else
                    zlog_info("send publish msg failed, %s", resp);
            } else {
                zlog_info("send publish msg success, %s", resp);
            }
        }

        increase_pkgid();
        free(resp);
    }
}


/*
 * 物联网平台下发的命令的响应结果处理*/
void command_resp_handle(srv_args_t* srv_args)
{
    int qos = 0;
    char *resp = NULL, topic[256] = {0};

    format_topic(topic, "commandResponse");									// 获取topic
    while (true)
    {
        if (!get_command_result(&resp, &qos))								// 1. get数据     Response等			resp： json文本数据
            break;

        if (mqtt_send_and_query(srv_args, PUBLISH, \
                p_mqtt->pkg_id, qos, topic, resp, strlen(resp)))			// 2. 发送到plat 
        {
            if (2 == qos) {
                if (mqtt_send_and_query(srv_args, PUBREL, \
                        p_mqtt->pkg_id, qos, NULL, NULL, 0))
                    zlog_info("send publish msg success, %s", resp);
                else
                    zlog_info("send publish msg failed, %s", resp);
            } else {
                zlog_info("send publish msg success, %s", resp);
            }
        }

        increase_pkgid();
        free(resp);
    }
}

/*
 * MQTT发送PUBLISH消息*/
void mqtt_publish_msg(srv_args_t* srv_args)
{
    /*
     * 设备在线状态更新*/

    /*
     * 物联网平台下发的命令的响应结果处理*/
    command_resp_handle(srv_args);
}

