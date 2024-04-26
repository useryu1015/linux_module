/**
 * @file thread_wdog.c
 * @author your name (you@domain.com)
 * @brief
 *  线程看门狗：
 *      1. 线程初始化
 *      2. 线程状态堆栈
 *      3. 线程独立看门狗
 * @improve 
 *  将看门狗与线程剥离， 即窗口看门狗
 *      1. 根据xx初始化 
 *      2. 使用 zthread_sheduler() 监控
 * @version 0.1
 * @date 2024-04-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/time.h>

#include "zthread_wdog.h"
#include "zlog.h"




void* run_scriptThread(void* arg);

static uint32_t get_century_ms()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);       // 世纪毫秒
}

/* 终止任务，并自行结束（SIGQUIT：kill -3） */
void zth_stop_quit(zthread_info *th)
{
    th->running = 0;
}

/* 强制终止任务（SIGKILL：kill -9） */
void zth_stop_kill(zthread_info *th)
{
    // todo: Linux如何杀死指定线程
    pthread_cancel(th->tid);
}

/* 喂狗 */
void thread_feeddog(zthread_info *th)
{
    pthread_mutex_lock(&th->wdog.mutex);
    th->wdog.run_time = get_century_ms();
    pthread_mutex_unlock(&th->wdog.mutex);
}

/**
 * @brief 检测线程是否正常运行（喂狗是否超时）
 * 
 * @param th 线程信息结构体指针
 * @param max_sleeptime 最大睡眠时间
 * @return _Bool 返回true表示线程正常运行，返回false表示线程运行超时
 */
_Bool thread_isrunning(zthread_info *th, uint32_t max_sleeptime)
{
    if (!th)
        return false;
    pthread_mutex_lock(&th->wdog.mutex);

    /* 喂狗异常 */
    if (get_century_ms() - th->wdog.run_time > max_sleeptime){
        zlog_error("thread[%s] run timeout");
        pthread_mutex_unlock(&th->wdog.mutex);
        return false;
    }

    pthread_mutex_unlock(&th->wdog.mutex);
    return true;
}

/**
 * @brief 注册线程接口函数
 * 
 * @param head 线程信息链表头指针
 * @param ztask 线程执行函数
 * @param pri 线程优先级
 * @return int 返回0表示注册成功，返回-1表示注册失败
 */
int zth_task_init(zthread_info* head, zth_task ztask, const char *task_name, int pri) {
    zthread_info* pth = (zthread_info*)calloc(1, sizeof(zthread_info));
    if (pth == NULL) {
        zlog_error("malloc failed.\n");
        return -1;
    }

    if (!ztask) {
        zlog_error("user function is null.\n");
        return -1;
    }

    pthread_mutex_init(&pth->wdog.mutex, NULL);
    pth->running = 1;
    pth->ztask = ztask;
    strncpy(pth->task_name, task_name, sizeof(pth->task_name));
    pthread_create(&(pth->tid), NULL, ztask, (void*)pth);

    thread_feeddog(pth);

    while (head->next != NULL) {                // fixme: null 则挂载为第一个
        head = head->next;
    }
    head->next = pth;
    pth->next = NULL;

    return 0;
}


void zth_delete_all(zthread_info* head) {
    zthread_info* current = head->next;         // fixme: head begin
    while (current != NULL) {
        current->running = 0;
        pthread_cancel(current->tid);
        pthread_join(current->tid, NULL);           // tips: 阻塞等待线程结束

        zthread_info* temp = current;
        current = current->next;
        pthread_mutex_destroy(&temp->wdog.mutex);   // 销毁互斥锁
        free(temp);
    }

    head->next = NULL;
}

void zth_delete(zthread_info *th) {
    if (!th)
        return;

    th->running = 0;
    usleep(500 *1000);              // 等待线程回收资源

    if (pthread_cancel(th->tid) != 0) {
        zlog_error("Failed to cancel thread.\n");
        return;
    }

    return;
}



/**
 * @brief ZThread模块的调度器函数。
 * 
 * @param zth 指向ZThread信息结构的指针。
 * @param watch_cycle 监控响应周期，单位为毫秒。
 * @return int 返回0表示成功，-1表示zth指针为空。
 */
int zth_scheduler(zthread_info *zth, uint32_t watch_cycle) {
    static uint32_t able_time = 0;
    uint32_t now_time = get_century_ms();
    if (!zth)
        return -1;

    /* 时间片控制 */
    if (now_time - able_time <= watch_cycle)
        return 0;
    able_time = now_time;
    
    /* 喂狗超时监控 */
    zthread_info* current = zth->next;
    while (current != NULL) {
        int res = pthread_kill(current->tid, 0);    // 检查线程是否存在: 这种方法仅用于检查线程是否存在，不会对线程进行实际操作或改变其状态。
        if (res != 0) {
            zlog_error("线程不存在或无法访问\n");
        }

        if (!current->running || !thread_isrunning(current, WDOG_MAX_TIMEOUT) || res != 0) {
            zlog_error("Thread %p stopp. Restarting...\n", (void*)current);
            zth_delete(current);
            current->running = 1;
            pthread_create(&(current->tid), NULL, current->ztask, (void*)current);
        }
        current = current->next;
    }


    return 0;
}

/* 守护线程 */
void *zth_watchdog(void* arg) {
    zthread_info* head = (zthread_info*)arg;

    while (1) {
        zth_scheduler(head, 500);
    }

    return NULL;
}


int zth_wdog_init(zthread_info *zhead, pthread_t *pth, int able_wdog) {
    if (!zhead || !pth) 
        return -1;
    
    /* 注册-窗口看门狗 */
    if (able_wdog) {
            // 配置线程属性
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, 1024 * 1024); // 设置堆栈大小为1M
        pthread_create(&pth, &attr, zth_watchdog, (void*)zhead);
    }

    return 0;
}





