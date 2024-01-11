#ifndef __SYS_LED_H__
#define __SYS_LED_H__

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <zlog.h>

#define LED_RUN_BLUE    "led1"
#define LED_ERR_RED     "led2"
#define LED_WIFI_BLUE   "led2"
#define LED_SUM 3

#define LED_BLINK_ONCE_TIME (100)

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

typedef struct _led_components
{
    char led_fd[32];
    led_busy_priority busy;

    /* 配置信息*/
    uint16_t delay_on;      // 亮time
    uint16_t delay_off;     // 灭time
    uint16_t delay_times;   // 周期内闪烁次数（0：无限制）
    uint16_t delay_sleep;   // 闪烁周期间隔

    /* 状态信息*/
    _Bool led_light;        // 亮灭状态
    uint16_t led_times;     // 已闪烁次数
    uint32_t led_clock;     // 上次状态起始时间，世纪毫秒ms
    led_step led_state;     // 当前blink状态

} led_components;

extern led_components led[LED_SUM];
extern led_components led_null[1];

/* */
int led_blink_scheduler(char *ledfd, led_components *conf);
/*设置LED亮或灭*/
void led_gpio_switch(char *ledfd, _Bool light);


/*设置在scheduler中常亮或长灭*/
void led_set_light(char *ledfd, _Bool light);
void led_set_blink_mode(char *ledfd, uint16_t delay_on, uint16_t delay_off, uint16_t delay_sleep, uint16_t delay_times);
/* 配置LED每周期频闪次数 及优先级*/
void led_set_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri);
void led_dis_blink_times(char *ledfd, uint16_t delay_times, led_busy_priority pri);
/* 使能LED闪烁一次（如网口、串口有数据交互时）*/
void led_set_blink_once(char *ledfd, _Bool reversal);
void led_blink_init();
void led_delete();


#endif