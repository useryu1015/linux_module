/**
 * @file zth_wdog_demo.c
 * @author your name (you@domain.com)
 * @brief    用法示例程序
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


void *test_task1(void *args)
{
    zthread_info *pt = (zthread_info *)args;
    if (!pt) return;

    zlog_info("start %s", pt->task_name);
    while (1)
    {
        zlog_info("run %s", pt->task_name);
        sleep(3);
    }
    
    zlog_error("stop %s", pt->task_name);
}

void *test_task2(void *args)
{
    zthread_info *pt = (zthread_info *)args;

    while (1)
    {
        zlog_info("run %s", pt->task_name);
        sleep(4);
    }
    
    zlog_error("stop %s", pt->task_name);
}


void *test_task3(void *args)
{
    zthread_info *pt = (zthread_info *)args;

    while (1)
    {
        zlog_info("run %s", pt->task_name);
        sleep(4);
    }
    
    zlog_error("stop %s", pt->task_name);
}


int main()
{
    int i;
    pthread_t watchdog;
    zthread_info zhead = {.next = NULL};        // head mast null

    // for (i = 0; i < 10; i++) {
    //     thread_init(&zhead, NULL);                      // 注册线程接口
    //     zlog_info("thread_init[%d]: %s",i , "done");
    // }

    zlog_info("");

    zth_task_init(&zhead, test_task1, "test_task1", 0);
    zth_task_init(&zhead, test_task2, "test_task2", 0);
    zth_task_init(&zhead, test_task3, "test_task3", 0);


    while (1){
        
    }

    return 0;
}

