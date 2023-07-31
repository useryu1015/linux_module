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

led_components led[LED_SUM];
led_components led_null[1];
pthread_mutex_t led_lock = PTHREAD_MUTEX_INITIALIZER;

#define LED_CMD_CREAT(CMD,FD,CONF,VAL) \
        {sprintf(CMD, "echo %s > /sys/class/leds/%s/%s", VAL, FD, CONF);}

/*关闭系统LED控制*/
static void _led_none_trigger_init(char *ledfd)
{
    if (!ledfd) {
        return;
    }
    char cmd[128] = { 0 };
    LED_CMD_CREAT(cmd, ledfd, "trigger", "none");
    system_result(cmd, NULL, 0);
}

/*设置LED亮*/
static void _led_set_on(char *ledfd)
{
    if (!ledfd) {
        return;
    }
    char cmd[128] = { 0 };
    LED_CMD_CREAT(cmd, ledfd, "brightness", "1");
    system_result(cmd, NULL, 0);
}

/*设置LED灭*/
static void _led_set_off(char *ledfd)
{
    if (!ledfd) {
        return;
    }
    char cmd[128] = { 0 };
    LED_CMD_CREAT(cmd, ledfd, "brightness", "0");
    system_result(cmd, NULL, 0);
}

/*设置LED亮或灭*/
void led_gpio_switch(char *ledfd, _Bool light)
{
    if (!ledfd) {
        return;
    }
    (light) ? _led_set_on(ledfd) : _led_set_off(ledfd);
}

uint16_t get_led_delay_time(led_components *conf)
{
    switch (conf->led_state) {
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

    for (i = 0; i < LED_SUM; i++) {
        if (0 == strcmp(ledfd, led[i].led_fd))
            return &led[i];
    }

    return led_null;
}

/* */
int led_blink_scheduler(char *ledfd, led_components *conf)
{
    int rc;
    uint32_t now_time;
    pthread_mutex_lock(&led_lock);
    if (!ledfd || !conf) {
        rc = LED_SCHE_ERR;
        goto END;
    }

    // LED无任务或继续当前xx
    if (conf->busy == LED_BUSY_FALSE || conf->led_state == LED_STEP_KEEP) {
        rc = LED_SCHE_IDLE;
        goto END;
    }

    get_timeofday_ms(&now_time);
    // LED当前工作状态未结束
    if (now_time - conf->led_clock < get_led_delay_time(conf)) {
        rc = LED_SCHE_BUSY;
        goto END;
    }


    /* 只闪一次*/
    if (conf->led_state == LED_STEP_ONCE) {
        conf->led_light = !conf->led_light;
        conf->busy = LED_BUSY_FALSE;
        rc = LED_SCHE_TURN;
        goto END;
    }

    /* 周期闪烁*/
    conf->led_light = !conf->led_light;         // 亮灭
    conf->led_state = (conf->led_light) ? LED_STEP_ON : LED_STEP_OFF;
    get_timeofday_ms(&conf->led_clock);         // 更新当前状态xx时间

    conf->led_times++;
    if (conf->led_times >= conf->delay_times && conf->delay_times) {
        conf->led_times = 0;
        conf->led_state = LED_STEP_SLEEP;
        conf->led_light = false;                // 休眠周期默认长灭
    }

    rc = LED_SCHE_TURN;
END:
    pthread_mutex_unlock(&led_lock);
    return rc;
}


void led_blink_init()
{
    int i;

    strcpy(led[0].led_fd, LED_RUN_BLUE);       // fixme: enum
    strcpy(led[1].led_fd, LED_ERR_RED);
    strcpy(led[2].led_fd, LED_WIFI_BLUE);

    for (i = 0; i < LED_SUM; i++) {
        led[i].busy = LED_BUSY_FALSE;
        _led_none_trigger_init(led[i].led_fd);
        led_gpio_switch(led[i].led_fd, true);  // 设备启动时，所有灯长亮一秒
    }
    sleep(1);
    for (i = 0; i < LED_SUM; i++) {
        led_gpio_switch(led[i].led_fd, false);
    }

    /* 配置RUN指示灯每秒闪一次*/
    led_set_blink_mode(LED_RUN_BLUE, 500, 500, 0, 0);
}

void led_set_blink_mode(char *ledfd, uint16_t delay_on, uint16_t delay_off, uint16_t delay_sleep, uint16_t delay_times)
{
    led_components *led = get_led_conf_by_fd(ledfd);
    pthread_mutex_lock(&led_lock);
    led->delay_on = delay_on;
    led->delay_off = delay_off;
    led->delay_sleep = delay_sleep;
    led->delay_times = delay_times;

    led->led_state = LED_STEP_OFF;
    led->led_times = 0;
    led->busy = LED_BUSY_TRUE;
    get_timeofday_ms(&led->led_clock);
    pthread_mutex_unlock(&led_lock);
}

/* 配置LED每周期频闪次数 及优先级*/
void led_set_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    if (led->busy > pri)     // 有更高优先级灯控任务，则略过xx （fix：备份？）
        return;

    led_set_blink_mode(ledfd, 300, 200, 2000, delay_times * 2);
    led->busy = pri;
}

/* 失能指定频闪和优先级任务*/
void led_dis_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    if (led->busy == pri && led->delay_times == delay_times * 2) {
        led_gpio_switch(ledfd, false);
        led->busy = LED_BUSY_FALSE;
    }
}

/**
 * 闪烁一次
 *  reversal：true：状态反转 false：先亮后灭
 * */
void led_set_blink_once(char *ledfd, _Bool reversal)
{
    led_components *led = get_led_conf_by_fd(ledfd);

    led_set_blink_mode(ledfd, 0, 0, 0, 0);
    led->busy = LED_BUSY_TRUE;
    led->led_state = LED_STEP_ONCE;
    get_timeofday_ms(&led->led_clock);

    led->led_light = reversal ? !led->led_light : true;
    led_gpio_switch(led->led_fd, led->led_light);
}

/**
 * 设置在scheduler中常亮或长灭
 * */
void led_set_light(char *ledfd, _Bool light)
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

