## Linux窗口看门狗（驱动程序）
**概述**
* 监控程序是否跑飞， 任务线程是否阻塞&超时等；
* 自定义喂狗超时回调函数，可用于资源回收、异常处理等；
* 极小的代码量和资源消耗，可轻松集成到应用中。

**测试**

``` bash
gcc zwdog.c -o zwdog -DRUN_ZWDOG_DEMO   # 编译
./zwdog   # 运行
```
```
[root@localhost zwdog]# ./zwdog 
zwdog demo start


    zwdog fd 1 feed timeout, process:TASK 1 
    exit process 

```

**示例代码**

``` C
void main(void)
{
    int user_fd1;
    int user_fd2;

    /* 新建监控对象1（超过5秒未喂狗，则触发超时回调函数） */
    zwdog_new(&user_fd1, 5 *1000, NULL, "TASK 1");
    /* 新建监控对象2 */
    zwdog_new(&user_fd2, 0, NULL, "TASK 2");
    printf("zwdog demo start\n");

    while (1) {
        zwdog_feed(user_fd2);   // 喂狗

        zwdog_sche(0);          // 遛狗
        sleep(1);
    }

    zwdog_free();
}
```

**优化**
* 参考 pthread_t 设计句柄，直接访监控对象数据堆栈，减少CPU开销；（或两者兼容）

