/**
 * @file zwdog.h
 * @author your name (you@domain.com)
 * @brief  窗口看门狗驱动程序， 用于监控窗口是否正常运行，并在超时后执行回调函数。
 * @博文
 *      用百行代码(几个函数)实现 窗口看门狗驱动程序
 *      xx 使用成本
 * @version 0.1
 * @date 2024-05-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _ZWDOG_H_
#define _ZWDOG_H_

#define ZWDOG_MAX_FEED_TIME (60 *1000)          /* 默认喂狗超时时间 60s */

/* 初始化一个回调函数 */
typedef void (*__unfeed_cback)(int);

typedef struct _zwdog_list_t
{
    /* config */
    int                     fd;                 /* 监控的文件描述符 id */
    uint64_t                timeout;            /* 喂狗超时时间 */
    __unfeed_cback          unfeed_cback;       /* 超时回调函数 */
    char                    name[64];           /* 看门狗名称 调试 */

    /* status */
    volatile uint64_t       fdog_time;          /* 喂狗时间 */
    pthread_mutex_t         mutex;
    
    struct _zwdog_list_t    *next;
} zwdog_list_t;


/* zwdog api */
int zwdog_new(int *fd, int timeout, __unfeed_cback cback, const char *pname);

int zwdog_feed(int fd);
int zwdog_sche(uint64_t freq_ms);
#endif