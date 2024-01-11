#include <unistd.h>
#include "sys_led.h"
#include "sys_cmd.h"

/*
* 获取ws连接状态*/
_Bool _ws_get_connect_status();

/* ERR指示灯*/
void *err_led_ctrl(void *args)
{
    char ret[128];

    system_result("netstat -natp | grep ssl_proxy | grep EST -c", ret, 128);
    if (0 == strncmp(ret, "0", 1))
    {
        _led_set_blink_times(LED_ERR_RED, 3, LED_BUSY_PRI3);     // SSL连接失败，闪烁3次
        zlog_warn("SSL连接失败");
    } else 
    {
        _led_dis_blink_times(LED_ERR_RED, 3, LED_BUSY_PRI3);
        // zlog_warn("SSL虚拟网口建立成功");
    }

    system_result("ip a | grep tun -c", ret, 128);
    // system_result("ip a | grep -c ens", ret, 128);
    if (0 == strncmp(ret, "0", 1))
    {
        _led_set_blink_times(LED_ERR_RED, 2, LED_BUSY_PRI4);     // SSL虚拟网口建立失败，闪烁2次
        zlog_warn("SSL虚拟网口建立失败");
    } else 
        _led_dis_blink_times(LED_ERR_RED, 2, LED_BUSY_PRI4);
    
    if (!_ws_get_connect_status())
        _led_set_blink_times(LED_ERR_RED, 1, LED_BUSY_PRI5);     // 连接上位机失败，闪烁1次
    else
        _led_dis_blink_times(LED_ERR_RED, 1, LED_BUSY_PRI5);

    return;
}

/*
 * wifi状态监控， ERR灯提示：
 *  wifi连接正常：灭
 *  wifi无连接：5HZ闪烁
 *  信号质量：
 * */
void *wifi_led_ctrl(void *args)
{
    char ret[128];

    // _led_set_blink_once(LED_WIFI_BLUE, true);       // 闪烁一次

    /* WIFI连接状态*/
    system_result("/xlian/script/wifi_get_status.sh", ret, 128);
    if (0 == strncmp(ret, "COMPLETED", strlen("COMPLETED"))) {
        _led_set_light(LED_WIFI_BLUE, true);        // 连接成功：常亮
    } else if (0 == strncmp(ret, "SCANNING", strlen("SCANNING"))) {
        _led_set_light(LED_WIFI_BLUE, false);
    } else {
        _led_set_light(LED_WIFI_BLUE, false);
    }
}

void *wifi_led_test(void *args)
{
    zlog_info("test start");
    sleep(2);
    _led_set_light(LED_WIFI_BLUE, true);
    
    sleep(1);
    _led_set_blink_once(LED_WIFI_BLUE, true);
    sleep(1);
    _led_set_blink_once(LED_WIFI_BLUE, true);
    sleep(1);
    _led_set_blink_once(LED_WIFI_BLUE, true);
    zlog_info("test done");
}

/*
 * */
void *run_led_scheduler(void *args)
{
    led_blink_init();
    zlog_info("LED Scheduler Start");

    /* Add call back*/
    led_timer_create(LED_WIFI_BLUE, wifi_led_ctrl, 1000);
    led_timer_create(LED_ERR_RED, err_led_ctrl, 5000);

    /* 配置RUN指示灯每秒闪一次*/
    _led_set_blink_mode(LED_RUN_BLUE, 200, 800, 0, 0);

    /* Task*/
    while (true)
    {
        led_heartbeat();
        usleep(10 * 1000);
    }

    led_delete();
    zlog_warn("LED Completed");
    return 0;
}
