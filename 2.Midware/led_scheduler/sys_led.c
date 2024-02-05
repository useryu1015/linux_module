#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>
#include <linux/input.h>
#include "sys_led.h"
#include "sys_cmd.h"

#ifdef USE_FD
/****************
 *  LED FD
*****************/
static led_fd_t fd_led[LED_SUM];
static led_fd_t *led1;      // RUN 浅蓝色
static led_fd_t *led2;      // ERR 红色

int led_init()
{
    led1 = &fd_led[0];
    led2 = &fd_led[1];

    led1->bright = open("/sys/class/leds/led1/brightness", O_RDWR);
    led1->maxbrt = open("/sys/class/leds/led1/max_brightness", O_RDWR);
    led1->trigger = open("/sys/class/leds/led1/trigger", O_RDWR);

    led2->bright = open("/sys/class/leds/led2/brightness", O_RDWR);
    led2->maxbrt = open("/sys/class/leds/led2/max_brightness", O_RDWR);
    led2->trigger = open("/sys/class/leds/led2/trigger", O_RDWR);

    /* LED状态初始化*/
    // led_set_off();

    // 添加异常校验
    return 0;
}

inline void led_write(int fd, char *buf)
{
    write(fd, buf, strlen(buf));
}

/*
 * 设置闪烁频率： 亮on_time毫秒，灭off_time毫秒*/
void led_set_blink(char *led_fd, int on_time, int off_time)
{
    char cmd[128];
    char ret[128];
    
    LED_SPRINTF_STR(cmd, "timer", led_fd, "trigger");
    system_result(cmd, ret, 128);
    // system_result("echo timer > /sys/class/leds/led2/trigger", ret, 128);

    LED_SPRINTF_INT(cmd, on_time, led_fd, "delay_on");
    system_result(cmd, ret, 128);    
    // sprintf(cmd, "echo %d > /sys/class/leds/led2/delay_on", on_time);

    LED_SPRINTF_INT(cmd, off_time, led_fd, "delay_off");
    system_result(cmd, ret, 128);    
    // sprintf(cmd, "echo %d > /sys/class/leds/led2/delay_off", off_time);
}
#endif

/****************
 * LED SYS CONFIF
*****************/
void led_none_trigger_init(char *ledfd)
{
    char ret[128];
    char cmd[128];

    LED_SPRINTF_STR(cmd, "none", ledfd, "trigger");            // 优化
    system_result(cmd, ret, 128);
    // system_result("echo none > /sys/class/leds/led2/trigger", ret, 128);
    // system_result("echo 1 > /sys/class/leds/led2/brightness", ret, 128);    
}

void led_set_on(char *ledfd)
{
    char ret[128];
    char cmd[128];

    LED_SPRINTF_STR(cmd, "1", ledfd, "brightness");
    system_result(cmd, ret, 128);
}

void led_set_off(char *ledfd)
{
    char ret[128];
    char cmd[128];

    LED_SPRINTF_STR(cmd, "0", ledfd, "brightness");
    system_result(cmd, ret, 128);
}

/****************
 * LED TIMER
*****************/
#define LED_BLINK_ONCE_TIME 100

typedef enum _led_step
{
	LED_STEP_OFF,      //长灭
	LED_STEP_ON,       //常亮
	LED_STEP_SLEEP,    //周期休眠
	LED_STEP_ONCE,     //闪烁1次
    LED_STEP_KEEP      //保持
} led_step;

typedef enum _led_scheduler_status
{
	LED_SCHE_ERR,       //
	LED_SCHE_BUSY,
	LED_SCHE_IDLE,      //不用处理
	LED_SCHE_TURN       //状态切换
} led_scheduler_status;

typedef struct _led_timer_cb
{
    int freq_ms;
    uint32_t trigger_time;
    void (*func_cb)(void *);
} led_timer_cb;

typedef struct _led_components
{
    char led_fd[32];
    led_busy_priority busy;

    /* 配置信息*/
    uint16_t delay_on;      // 亮time
    uint16_t delay_off;     // 灭time
    uint16_t delay_sleep;   // 闪烁周期间隔
    uint16_t delay_times;   // 周期内闪烁次数（0：无限制）

    /* 状态信息*/
    _Bool led_light;        // 亮灭状态
    uint16_t led_times;     // 已闪烁次数
    uint32_t led_clock;     // 上次状态起始时间，世纪毫秒ms
    led_step led_state;     // 当前blink状态

    led_timer_cb *timer;
} led_components;

led_components led[LED_SUM];
led_components led_null[1];

static void get_timeofday_ms(uint32_t *time_ms)
{
    struct timeval tv;
    
    // if (!time_ms)
    //     return;

    gettimeofday(&tv, NULL);
    *time_ms = (tv.tv_sec*1000 + tv.tv_usec/1000);       // 世纪毫秒
}

uint16_t get_led_delay_time(led_components *conf)
{
    switch (conf->led_state)
    {
        case LED_STEP_ON: return conf->delay_on;
        case LED_STEP_OFF: return conf->delay_off;
        case LED_STEP_SLEEP: return conf->delay_sleep;
        case LED_STEP_ONCE: return LED_BLINK_ONCE_TIME;        // 只亮一次，define 100ms
        default: return 0;
    }
}

led_components *get_led_conf_by_fd(char *ledfd)
{
    int i;

    for (i = 0; i < LED_SUM; i++) 
    {
        if (0 == strcmp(ledfd, led[i].led_fd))
            return &led[i];
    }

    return led_null;
}

/* */
int led_blink_scheduler(char *ledfd, led_components *conf)
{
    uint32_t now_time;

    if (!ledfd || !conf)
        return LED_SCHE_ERR;

    if (conf->busy == LED_BUSY_FALSE || conf->led_state == LED_STEP_KEEP)      // LED无任务或继续当前xx
        return LED_SCHE_IDLE;

    get_timeofday_ms(&now_time);
    if (now_time - conf->led_clock < get_led_delay_time(conf))                  // LED当前工作状态未结束
        return LED_SCHE_BUSY;

    /* 只闪一次*/
    if (conf->led_state == LED_STEP_ONCE) {
        conf->led_light = !conf->led_light;
        conf->busy = LED_BUSY_FALSE;
        // zlog_info("LED light once done");
        return LED_SCHE_TURN;
    }

    /* 周期闪烁*/
    conf->led_light = !conf->led_light;         // 亮灭
    conf->led_state = (conf->led_light)? LED_STEP_ON : LED_STEP_OFF;
    get_timeofday_ms(&conf->led_clock);         // 更新当前状态xx时间

    conf->led_times++;
    if (conf->led_times >= conf->delay_times && conf->delay_times)
    {
        conf->led_times = 0;
        conf->led_state = LED_STEP_SLEEP;
        conf->led_light = false;                // 休眠周期默认长灭
        // conf->busy = LED_BUSY_FALSE;            // 只触发一周期
    }

    // zlog_info("LED_FD:%s", conf->led_fd); 
    // zlog_info("LED_LIGHT:%s, led_clock:%d", (conf->led_light)? "on":"off", conf->led_clock);
    // zlog_info("led_times:%d, led_state:%d", conf->led_times, conf->led_state);

    return LED_SCHE_TURN;
}

/* LED硬件接口：*/
void led_gpio_switch(char *ledfd, _Bool light)
{
    (light) ? led_set_on(ledfd) : led_set_off(ledfd);
    // zlog_info(">> %s turn %s", ledfd, (light)? "on":"off");
}

static void led_timer_hb(led_components *pled)
{
    led_timer_cb *cb = pled->timer;
    uint32_t now_time;

    if (!pled || !cb)
        return -1;

    get_timeofday_ms(&now_time);
    if ((now_time - cb->trigger_time) > cb->freq_ms)
    {
        cb->func_cb(NULL);
        get_timeofday_ms(&cb->trigger_time);
    }
}

/* 监控并使能LED当前状态*/
void led_heartbeat()
{
    int i;

    for (i = 0; i < LED_SUM; i++)
    {
        led_components *pled = get_led_conf_by_fd(led[i].led_fd);

        led_timer_hb(pled);

        if (led_blink_scheduler(pled->led_fd, pled) == LED_SCHE_TURN)         // xxxx
        {
            led_gpio_switch(pled->led_fd, pled->led_light);
        }
    }

}

void led_blink_init()
{
    int i;

    strcpy(&led[0].led_fd, LED_RUN_BLUE);       // fixme: list data struct
    strcpy(&led[1].led_fd, LED_ERR_RED);
    strcpy(&led[2].led_fd, LED_WIFI_BLUE);

    for (i = 0; i < LED_SUM; i++) {
        led[i].busy = LED_BUSY_FALSE;
        led[i].timer = NULL;
        led_none_trigger_init(led[i].led_fd);
        led_gpio_switch(led[i].led_fd, true);  // 设备启动时，所有灯长亮一秒
    }

    sleep(1);
    for (i = 0; i < LED_SUM; i++)
        led_gpio_switch(led[i].led_fd, false);
}

void _led_set_blink_mode(char *ledfd, uint16_t delay_on, uint16_t delay_off, uint16_t delay_sleep, uint16_t delay_times)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    led->delay_on = delay_on;
    led->delay_off = delay_off;
    led->delay_sleep = delay_sleep;
    led->delay_times = delay_times;

    led->led_state = LED_STEP_OFF;
    led->led_times = 0;
    led->busy = LED_BUSY_TRUE;
    get_timeofday_ms(&led->led_clock);
}

/* 配置LED每周期频闪次数 及优先级*/
void _led_set_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    if (led->busy > pri)     // 有更高优先级灯控任务，则略过xx （fix：备份？）
        return;

    _led_set_blink_mode(ledfd, 100, 200, 1000, delay_times * 2);    
    led->busy = pri;
}

/* 失能指定频闪和优先级任务*/
void _led_dis_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    if (led->busy == pri && led->delay_times == delay_times * 2)
    {
        led_gpio_switch(ledfd, false);
        led->busy = LED_BUSY_FALSE;
    }
}
 
/**
 * 闪烁一次
 *  reversal：true：状态反转 false：先亮后灭
 * */
void _led_set_blink_once(char *ledfd, _Bool reversal)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    _led_set_blink_mode(ledfd, 0, 0, 0, 0);
    led->busy = LED_BUSY_TRUE;
    led->led_state = LED_STEP_ONCE;
    get_timeofday_ms(&led->led_clock);
    
    led->led_light = reversal ? !led->led_light : true;
    led_gpio_switch(led->led_fd, led->led_light);
}

/**
 * 设置在scheduler中常亮或长灭
 * */
void _led_set_light(char *ledfd, _Bool light)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    led->busy = LED_BUSY_TRUE;
    led->led_state = LED_STEP_KEEP;
    led->led_light = light;
    led_gpio_switch(led->led_fd, light);
}

/* 系统正常退出，长灭LED*/
void led_delete()
{
    int i;

    for (i = 0; i < LED_SUM; i++) {
        led_gpio_switch(led[i].led_fd, false);
        // free();
    }
}

/* DEBUG：堵塞占用主线程调度*/
void led_timer_create(char *ledfd, void *func, int freq)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    if (!led)
        return;

    if (led->timer)
        zlog_warn("LED Used Task");

    led->timer = (led_timer_cb *)malloc(sizeof(led_timer_cb));
    if (!led->timer)
        return;

    led->timer->freq_ms = freq;
    led->timer->func_cb = func;
    led->timer->trigger_time = 0;
}