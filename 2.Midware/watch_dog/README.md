## LED控制程序
**说明**
1. **进程看门狗：** 使用fork、shell等方式，监控进程状态
2. **线程看门狗：** 基于定时器，配置窗口看门狗的初始化、喂狗等流程；并根据任务权重，按需重新系统
3. **任务看门狗：** 


**Linux窗口看门狗**
1. 定义&初始化任务堆栈结构体 task_info;
2. 监控task_info的喂狗状态， 超时返回任务句柄（&task_info）
3. 根据task_info信息， 处理超时程序



**拓展**
* **迷你操作系统：**
> 使用xx接口函数， 能实现task 初始化，使能，监控，喂狗等捆绑操作

* **mini_scheduler()函数开发** 
> 创建一个线程，管理多个任务的调度策略。包括任务执行频率...     
> 使用一个链表，注册任务 频率、状态、喂狗、等操作句柄。     
> 备注： 通过一个线程管理多个任务，能够减少对系统资源的占用。 （如果使用线程使能mini_scheduler，需要合理控制其堆栈大小）



**参考资料**    
* [Linux软件看门狗-已弃用](https://github.com/sunmingbao/soft-wdt)：一款通过/dev/soft_wdt喂狗的程序 
<!-- * [守护线程设计]()
* [MCU独立看门狗&窗口看门狗]()
* [fork监控]() -->