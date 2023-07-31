#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <zlog.h>

/* */
#define LED_RUN_BLUE    "led1"
#define LED_ERR_RED     "led2"
#define LED_WIFI_BLUE   "led1"
// #define LED_WIFI_BLUE   "led3"
#define LED_SUM 3

#define LED_SPRINTF_STR(S,X,Y,MOD) {sprintf(S, "echo %s > /sys/class/leds/%s/%s", X, Y,MOD);}
#define LED_SPRINTF_INT(S,X,Y,MOD) {sprintf(S, "echo %d > /sys/class/leds/%s/%s", X, Y,MOD);}

typedef enum _led_busy_priority
{
	LED_BUSY_FALSE = 0,     // 空闲
	LED_BUSY_TRUE,          // 使用中
	LED_BUSY_PRI5,    
	LED_BUSY_PRI4,
	LED_BUSY_PRI3,
	LED_BUSY_PRI2,
	LED_BUSY_PRI1,
    LED_BUSY_HIGH           // 最高使用权
} led_busy_priority;

// void led_set_on(char *led);
// void led_set_off(char *led);
// void led_set_blink(char *led, int on_time, int off_time);
// void led_gpio_switch(char *ledfd, _Bool light);

/**
 * LED CONFIG
*/
void _led_set_light(char *ledfd, _Bool light);
void _led_set_blink_mode(char *ledfd, uint16_t delay_on, uint16_t delay_off, uint16_t delay_sleep, uint16_t delay_times);
/* 配置LED每周期频闪次数 及优先级*/
void _led_set_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri);
void _led_dis_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri);
/* 使能LED闪烁一次（如网口、串口有数据交互时）*/
void _led_set_blink_once(char *ledfd, _Bool reversal);

/**
 * LED INIT
*/
void led_heartbeat();
void led_blink_init();
void led_delete();
void led_timer_create(char *ledfd, void *func, int freq);
void *run_led_scheduler(void *args);
