#include "conf_sink.h"
#include "services.h"
#include "iohandle.h"
#include "mqtt.h"
#include "zlog.h"

mqtt_svr_arg *p_mqtt; //mqtt客户端服务参数

/*
 * MQTT服务初始化, 系统初始化时调用 */
void mqtt_init()
{
    p_mqtt = (mqtt_svr_arg*)malloc(sizeof(mqtt_svr_arg));

    if (!p_mqtt) {
        zlog_error("malloc mqtt srv arg error!");
        return;
    }

    memset(p_mqtt, 0, sizeof(mqtt_svr_arg));

    p_mqtt->p_client = &(SINK.service_config.mqtt);

    pthread_mutex_init(&(p_mqtt->mutex), NULL);		// 互斥
}


/*
 * 重置MQTT服务参数，服务初始化时调用 */
void reset_mqtt_arg(channel_info_t* p_channel)
{
    if (!p_channel)
        return;

    p_mqtt->p_channel = p_channel;
    p_mqtt->p_client = &(SINK.service_config.mqtt);
    p_mqtt->recv_time = time(NULL);
    p_mqtt->send_time = time(NULL);
    p_mqtt->ping_time = 0;
    p_mqtt->connected = false;
    p_mqtt->pkg_id = 1;
}

void mqtt_clear_before_exit()
{
    p_mqtt->connected = false;
}

static _Bool is_mqtt_link_alive(time_t now, int timeout)
{
    if (0 == p_mqtt->ping_time)
        return true;

    return (now - p_mqtt->ping_time <= timeout/1000);
}

/*
 * 启动mqtt客户端服务 */
void* run_mqtt_client(void* args)
{
    int nread = 0;
    unsigned char buf[8192] = {0}; //最好改为循环缓冲区，逐条消息进行处理
    srv_args_t* srv_args = (srv_args_t*) args;
    statisitics_t* p_stat = srv_args->p_stats;

    zlog_srv_info(srv_args, SERVICE_START_PENDING);
    set_running(p_stat, true);

    reset_mqtt_arg(srv_args->_base.p_channel);
    if (srv_args->p_io->open(srv_args) < 0) {
        zlog_srv_warn(srv_args, SERVICE_STOPPED);
        set_running(srv_args->p_stats, false);
        pthread_exit((void*) 0);
    }

    /*
     * 建立MQTT连接并订阅需要的主题信息 */
    pthread_mutex_lock(&(p_mqtt->mutex));
    mqtt_connect_retry(srv_args);
    mqtt_send_and_query(srv_args, SUBSCRIBE, 0, 0, NULL, NULL, 0); //订阅主题
    pthread_mutex_unlock(&(p_mqtt->mutex));

    /*注册子设备*/
    mqtt_register_device(srv_args);

    while (is_running(srv_args->p_stats))
    {
        nread = mqtt_read(srv_args, buf, sizeof(buf));
        if (nread < 0) {
            zlog_info("mqtt read %d data", nread);
            set_running(srv_args->p_stats, false);
            continue;
        } else if (nread > 0) {
            stat_io_read(srv_args->p_stats, nread);
            mqtt_msg_handle(srv_args, buf, nread); 	// 收到的mqtt消息处理
            p_mqtt->recv_time = time(NULL);
        } else {
            if (!is_mqtt_link_alive(time(NULL), p_mqtt->p_channel->timeout)) {
                zlog_warn("recv ping resq timeout, exit mqtt service");
                set_running(srv_args->p_stats, false);
                continue;
            }
        }

        mqtt_publish_msg(srv_args); //发送publish消息			// 周期扫描rtdb是否有需要上传plat的数据，然后publish？
        mqtt_send_pingreq(srv_args); //发送心跳

        usleep(100 * 1000);
    }

    p_mqtt->connected = false;
    mqtt_clear_before_exit();
    zlog_srv_info(srv_args, SERVICE_STOP_PENDING);
    pthread_exit((void*) 0);
}
