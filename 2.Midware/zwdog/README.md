## Linux窗口看门狗（驱动程序）
**概述**
* 监控程序是否跑飞， 任务线程是否阻塞&超时等；
* 自定义喂狗超时回调函数，可用于资源回收、异常处理等。

**示例**

``` C
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
```



