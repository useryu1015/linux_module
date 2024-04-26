#ifndef _ZTHREAD_WDOG_H_
#define _ZTHREAD_WDOG_H_

/* 默认喂狗超时时间 单位ms  */
#define WDOG_MAX_TIMEOUT (10 *1000)     // fixme: 改为zth独立配置

typedef void *(*zth_task)(void *);

/* zthread handle */
typedef struct _zthread_info {
    pthread_t tid;

    int running;                // 线程运行状态： true为使能
    zth_task ztask;             // 用户 task函数指针
    char task_name[32];         // 函数名称：调试&追溯

    int time_out;               // 配置超时时间

    struct {
        uint32_t run_time;      // 当前喂狗时间： 校验是否超时
        // uint32_t err_time;   // 超时响应时间
        pthread_mutex_t mutex;  // 喂狗时间锁： 就可能避免静态竞争导致的问题
    } wdog;
    
    // user code
    // lua_State *L;
    // SCRIPT *script;
    void *user_args;
    struct _zthread_info* next;
} zthread_info;




/* user function API */
int zth_task_init(zthread_info* head, zth_task ztask, const char *task_name, int pri);



#endif