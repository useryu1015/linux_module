# [2.Midware/led_scheduler](2.Midware/led_scheduler)
## LED控制程序
**设计思路**
* **设计模式：** 采用状态机模式，受Linux的LED驱动文件映射 `/sys/class/leds/` 的启发
* **设计原理：** 基于Linux的LED控制文件接口的设计原理，实现LED控制程序

**软件特点**
1. **多线程支持：** 具备多线程环境下对同一LED进行控制的权重配置
2. **LED闪烁控制：** 提供LED闪烁的周期、频率、间隔、次数等灵活配置选项（通过数据结构 `led_components` 进行管理）
3. **GPIO驱动接口：** 具备自定义GPIO驱动接口的能力，通过函数 `led_gpio_switch` 进行配置




**优化**
1. 动态管理LED数量，XX需求
2. 基于led_v2做定制拓展
3. 时钟源：定时器替换get_timeofday_ms

## 按键驱动程序
---------------------------
# [2.Midware/log](2.Midware/log)
## 日志系统
**设计思路**

**软件特点**

可配置 日志等级、颜色、
文件日志： 可配置文件大小、自动覆盖、多进程使用、
* 支持日志文件最大限制
* 支持日志文件多进程同时占用（如果对竞态条件有严格需求，需要加文件锁）
* 支持日志文件自动初始化（通过lsof检测文件是否被进程占用，NULL则覆盖日志）
* 支持调试日志输出等级配置（zlog_set_printlev函数配置）

**优化**



## 日志系统设计思路

**设计思路**

* **模块设计：** 采用模块化设计思路，使日志系统易于维护和扩展。
* **灵感来源：** 受启发于Linux的日志文件映射 `/var/log/`，采用类似设计原理。

**软件特点**
- **可配置性：** 提供灵活的配置选项，包括日志等级、颜色等。
- **文件日志配置：** 
  - 可配置日志文件大小。
  - 支持自动覆盖旧日志或多进程共享。
- **多进程支持：** 允许多进程同时占用日志文件；有严格时序需求，建议使用文件锁来解决竞态条件。
- **自动初始化：** 通过 `lsof` 检测文件是否被其他进程占用，若无占用则自动初始化日志。
- **调试日志配置：** 提供配置调试日志输出等级的功能，通过 `zlog_set_printlev` 函数进行配置。

**优化建议**
---------------------------
# [2.Midware/watch_dog](2.Midware/watch_dog)
## Linux看门狗（线程守护、进程守护、任务守护） 已弃用
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
---------------------------
# [2.Midware/zwdog](2.Midware/zwdog)
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
}
```
---------------------------
# [3.Tool/compile_envir](3.Tool/compile_envir)
## 自动化编译与打包工具
> 工具介绍：参考CMake、AutoMake和RPM制作zmake工具，用于快速搭建C/C++项目的交叉编译环境&程序安装包。
> 
> CSDN博文：[集成开发环境IDE设计](https://blog.csdn.net/ling0604/article/details/129144156)

**Usage**
1. 将 zmake 和 Makefile 拷贝到工程目录
2. make 默认编译当前目录所有c/cpp源文件，可自定义源文件路径、链接库、交叉编译、ignore等
3. 查看交叉编译选项执行命令：./zmake -h
```
[root@localhost compile_envir]# ./zmake -h
Usage: ./zmake [-h] [-c core] [-o opt]
Options:
  -h, --help      Display help message
  -c core         Set target platform architecture(e320, x100, x300, x86)
  -o opt          Set operation mode:
                    debug: build in debug mode
                    clean: clean up build artifacts
                    pack: pack the built artifacts
```
---------------------------
# [3.Tool/serial](3.Tool/serial)
## Linux 串口收发测试工具（serial_rxtx_test.c）
**概述**
1. 支持串口收发
2. 支持 modbus 校验

**使用**
1. 编译： gcc serial_rxtx_test.c -lrt -lpthread -o serial_rxtx_test
2. 运行： ./serial_rxtx_test -h
```
[root@localhost serial]# ./serial_rxtx_test -h

Usage: ./serial_rxtx_test [OPTION]...
	-t Device Identifier [default /dev/ttyS0]
	-b Baud int [default 115200]
	-d Data Bits int [default 8]
	-p Parity  char [default N]
	-s Stop Bits int [default 1]
	-m 1: Use CRC16, 0: No CRC16 [default 0]
	-h Show this message

```

![](/3.Tool/serial/serial_rxtx_demo.png)


## CRC16校验计算工具（modbus-crc16.cpp）
## 其他工具
1. 串口设备 从站地址 扫描
2. 传感器 数据报文 模拟
3. 批量测试

---------------------------
# [3.Tool/shell_script](3.Tool/shell_script)
## Shell脚本常用函数封装

``` shell
[root@localhost shell_script]# ./zh_shell_function.sh -d
get_last_word: orange
This is red text
This is blue text
This is green text
This is yellow text
This is cyan text
This is purple text
This is white text

[root@localhost shell_script]# ./zh_shell_function.sh -h
Usage: ./zh_shell_function.sh [-h] [-c core] [-o opt]
Options:
  -h, --help      Display help message
  -d, --demo      Display help message
  -c core         core: target platform architecture
  -o opt          opt: [debug | clean | pack]
[root@localhost shell_script]# 
```



---------------------------
# [3.Tool/shell_script/code_generator](3.Tool/shell_script/code_generator)
## 代码生成器
**简述：**
1. 将枚举值，生成字符串映射表
2. 将json配置，生成C解析伪代码
3. 将list链表，生成插入、删除、初始化函数

**目的**
1. 减少开发过程中CRUD工作量，专注于项目系统框架的搭建
2. 避免CRUD过程中，粗忽大意造成的代码BUG
3. 与AI代码相比，不用繁琐的校验和示教

**TODO：**
1. 制作APP（插件、命令、网页、QT）便于个人使用
2. 完善json解析，根据数组名称构建函数名


### 1. enum映射表
**使用说明：**
1. 执行命令格式化代码区：./gen_enum_map.sh clean
2. 在enum_content.c 文件指定位置插入枚举声明
3. 执行生成脚本：./gen_enum_map.sh

![](/.Document/code_gen/enum代码生成器-使用说明.png)

**使用效果：**

当前版本生成代码包括：枚举值字符串映射表、取值函数、调度函数
![](/.Document/code_gen/enum代码生成器-使用效果.png)


### 2. json解析代码
**使用说明：**
1. 执行命令格式化代码区：./gen_json_parse.sh.sh clean
2. json_content.c 文件指定位置插入枚举声明
3. 执行生成脚本：./gen_json_parse.sh.sh

**使用效果：**
![](/.Document/code_gen/json代码生成器.png)


<!---
- 说明：
	1. 将json代码转为结构体， 再增删改查
	2. 将enum枚举类型，转换为字符串与之对应（关键词xx，命名规范xx）
	3. 结合CHAGPT使用， 调教方式

- 技术方案：
	1. shell
	2. C
	3. ChatGPT
	4. python

- 难点：
	1. 使用方式
		本地程序， 繁琐
		云服务部署， 成本
		三方平台， MQTT数据摆渡
		vscode插件 + github

- 发布：GitHub、csdn、公众号、抖音、小红书、B站
-->

<!---
``` C
enum {
	E_ID, 
	E_NAME, 
	E_SIZE
} device_type_e;
// enum_name = device_type;

typedef struct _enum_map_t {
	int id;
	char name[32];
} enum_map_t;

写一个程序，根据enum的配置， 生成下列代码:
enum_map_t device_type_values[] = {
	{E_ID,		"ID"},
	{E_NAME,	"NAME"},
	{E_SIZE,	"SIZE"},
};
	
const char *get_device_type_name(device_type_e type)
{
    int i = 0, cnt = sizeof(device_type_values) / sizeof(device_type_values[0]);

    for (i = 0; i < cnt; ++i) {
        if (type == device_type_values[i].id) {
            return device_type_values[i].name;
        }
    }
    return "";
}

device_type_e get_device_type_id(const char *name)
{
    int i = 0, cnt = sizeof(device_type_values) / sizeof(device_type_values[0]);

    for (i = 0; i < cnt; ++i) {
        if (0 == strcasecmp(name, device_type_values[i].name)) {
            return (fblock_type_e)device_type_values[i].id;
        }
    }
    return 0;
}

for (int i = E_ID; i < E_SIZE; ++i) {
	switch (i) {
		case E_ID:
			printf("Device type: E_ID\n");
			break;
		case E_NAME:
			printf("Device type: E_NAME\n");
			break;
		case E_SIZE:
			printf("Device type: E_SIZE\n");
			break;
		default:
			printf("Unknown device type\n");
			break;
	}
}

```
-->



---------------------------
# [.Document](.Document)
## linux_module
> LinuxC开发工具集（代码生成器、文本解析、脚本、编译、进程通信、日志、串口调试、字符驱动、协议栈等）

**Usage:**
1. 修改子目录下README.md
2. 执行脚本：./generate_readme.sh 根目录生成README.md文件

**Tips:**
* 脚本输出REDEME.md大纲
* vscode预览快捷键：ctrl+K V
* 单击标题跳转到对应目录

**ToDo:**
1. 整理并封装程序，包括：
    * socket： 封装sink套接字接口，select和epoll程序
    * serial： 
    * cjson、zmq、sqlite：
    * mqtt： mosmqtt和pahomqtt库
2. 整理文件结构

**2024年2月1日-仓库目标**
* 参考文献，编写《Linux网关开发指南》；包括开发、调试、测试、运维等XX研发流程。
* 每一个README.md为一个文档小节
* 参考书籍：《嵌入式Linux应用开发完全手册-韦东山》、《CPU眼里的C/C++》、《C Priimer Plus》、《FreeRTOS内核实现与应用开发实战指南》、《LinuxXXXX》

---------------------------
