#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include "zwdog.h"

static volatile int         fd_pointer = 1;             /* fd 索引 */
static zwdog_list_t         *pdog_head;                 /* 监控对象链表头 */

static pthread_mutex_t      fd_init_mutex = PTHREAD_MUTEX_INITIALIZER;


/* cpu 时钟源 */
static uint64_t get_cpu_time_ms()
{
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    return (uint64_t)(current_time.tv_sec) * 1000 + (uint64_t)(current_time.tv_nsec / 1000000);
}

/* 获取 fd 的数据结构 */
static zwdog_list_t *zwdog_get_pd(int fd)
{
    zwdog_list_t *pdog = pdog_head;

    while (pdog != NULL) {
        if (pdog->fd == fd) {
            return pdog;
        }

        pdog = pdog->next;
    }

    return NULL;
}

static void zwdog_default_cback(int fd)
{
    zwdog_list_t *pdog = zwdog_get_pd(fd);

    printf("\n\n");
    printf("    zwdog fd %d feed timeout, process:%s \n", fd, pdog->name);
    printf("    exit process \n\n");

    // system("echo ");         /* 写入文件 /tmp/pid.xx */
    exit(-1);
}

/**
 * @brief 新建一个监控对象
 * 
 * @param fd 
 * @param timeout 喂狗超时时间 单位毫秒
 *          !timeout： 默认为 ZWDOG_MAX_FEED_TIME 毫秒
 * @param cback 喂狗超时动作 回调函数
 *          null：默认退出当前进程
 * @param name
 * @return int 
 */
int zwdog_new(int *fd, int timeout, __unfeed_cback cback, const char *pname)
{
    zwdog_list_t *pdog = (zwdog_list_t *)calloc(1, sizeof(zwdog_list_t));

    if (!fd || !pdog) {
        return -1;
    }

    static int init_flag = 0;
    if (!init_flag) {
        init_flag = 1;
    }

    pthread_mutex_lock(&fd_init_mutex);
    *fd = fd_pointer++;     // init fd
    pthread_mutex_unlock(&fd_init_mutex);

    pdog->fd = *fd;
    pdog->timeout = timeout ? timeout : ZWDOG_MAX_FEED_TIME;
    pdog->unfeed_cback = cback ? cback : zwdog_default_cback;

    strncpy(pdog->name, pname, sizeof(pdog->name));

    /* 插入链表 */
    if (pdog_head == NULL) {
        pdog_head = pdog;
    } else {
        zwdog_list_t *p = pdog_head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = pdog;
    }

    zwdog_feed(*fd);
    return 0;
}


/* 喂狗 */
int zwdog_feed(int fd)
{
    zwdog_list_t *pdog = pdog_head;

    while (pdog != NULL) {
        if (pdog->fd == fd) {
            pdog->fdog_time = get_cpu_time_ms();
            return 0;
        }

        pdog = pdog->next;
    }

    return -1;
}

/**
 * @brief 
 * 
 * @param freq_ms   调度频率，单位 ms
 * @return int 
 */
int zwdog_sche(uint64_t freq_ms)
{
    zwdog_list_t *pdog = pdog_head;
    uint64_t now_time = get_cpu_time_ms();

    if (freq_ms > 0) {
        static uint64_t last_time = 0;

        /* schedule 频率控制 */
        if ((now_time - last_time) < freq_ms) {
            return 0;
        }
        last_time = now_time;
    }

    while (pdog != NULL) {
        if (now_time - pdog->fdog_time > pdog->timeout) {
            /* 喂狗超时动作 */
            pdog->unfeed_cback(pdog->fd);
            return pdog->fd;
        }

        pdog = pdog->next;
    }

    return 0;
}

void zwdog_demo(void)
{
    int user_fd1;
    int user_fd2;

    zwdog_new(&user_fd1, 2 *1000, NULL, "DEMO 1");     /* 新建监控对象，并喂狗 */
    zwdog_new(&user_fd2, 0, NULL, "DEMO 2");

    while (1) {
        zwdog_feed(user_fd2);   /* 喂狗 */

        zwdog_sche(0);          /* 遛狗 */
        sleep(1);
    }
}