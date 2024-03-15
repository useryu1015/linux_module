# [2.Midware/led_scheduler](2.Midware/led_scheduler)
## LED控制程序
**设计思路**
* **设计模式：** 采用状态机模式，受Linux的LED驱动文件映射 `/sys/class/leds/` 的启发
* **设计原理：** 基于Linux的LED控制文件接口的设计原理，实现LED控制程序

**软件特点**
1. **多线程支持：** 具备多线程环境下对同一LED进行控制的权重配置
2. **LED闪烁控制：** 提供LED闪烁的周期、频率、间隔、次数等灵活配置选项（通过数据结构 `led_components` 进行管理）
3. **GPIO驱动接口：** 具备自定义GPIO驱动接口的能力，通过函数 `led_gpio_switch` 进行配置
4. 




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
* 

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
# [3.Tool/compile_envir](3.Tool/compile_envir)
## 自动化编译与打包工具
> 工具介绍：参考CMake、AutoMake和RPM制作zmake工具，用于快速搭建C/C++项目的交叉编译环境&程序安装包。
> 
> CSDN博文：[集成开发环境IDE设计](https://blog.csdn.net/ling0604/article/details/129144156)

**zmake**
> 自动化代码编译与软件打包工具

**Makefile**
> zmake依赖

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
# [4.Test/test_FOCAS/fwlib](4.Test/test_FOCAS/fwlib)
# Fanuc FOCAS Library
[![Docker Hub](https://img.shields.io/docker/v/strangesast/fwlib?sort=date)](https://hub.docker.com/r/strangesast/fwlib)  
Header and runtime files for CNC communication  

# Docker
Build the base image with `docker build .`  

Build an example with `docker build examples/c/Dockerfile`   

# `examples/`  
Link or rename appropriate `libfwlib\*.so` (based on platform) to `libfwlib32.so.1` and `libfwlib32.so` 

On linux x86\_64 for example: `ln -s libfwlib32-linux-x64.so.1.0.5 libfwlib32.so` 

More instructions in each example folder

---------------------------
# [4.Test/test_FOCAS/fwlib/examples/c-minimal](4.Test/test_FOCAS/fwlib/examples/c-minimal)
# Instructions

## Linux
From the root of this repository:  
```
gcc -L./ -Wl,-rpath . examples/c-minimal/main.c -lfwlib32 -lm -lpthread -o fanuc_minimal
./fanuc_minimal
```

## Windows
Requires Visual Studio or [Visual Studio Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019)  
From the root of this repository:  
```
cl.exe /Fe"fanuc_minimal.exe" .\examples\c-minimal\main.c /link .\Fwlib32.lib
.\fanuc_minimal.exe
```

---------------------------
# [4.Test/test_FOCAS/fwlib/examples/c](4.Test/test_FOCAS/fwlib/examples/c)
# Fanuc fwlib example
![CMake](https://github.com/strangesast/fwlib/workflows/CMake/badge.svg)
![Windows CMake](https://github.com/strangesast/fwlib/workflows/Windows%20CMake/badge.svg)  
An example that connects to a machine specified by configuration options (file / env / arg)  
Usage (all args are optional):  
```
./bin/fanuc_example --config=<path_to_config> --port=<device port> --ip=<device ip>
```

**Notice:** This example requires fetching submodules first (`git submodule update --init --recursive`)  

# Docker (Linux containers)
From the root of this repository:
```
docker build -t fwlib_c-example -f examples/c/Dockerfile .
docker run --rm --network=host -it fwlib_c-example
```

# Linux
1. Copy `libfwlib32-linux-$arch.so.$version` to `/usr/local/lib` then run `ldconfig`.  Install config with `apt install libconfig-dev` or compile manually.  
2. `mkdir build && cd build`  
3. `cmake ..`  
4. `cmake --build .`  
5. `ctest -V`  

# Windows
1. Compile libconfig in `extern/libconfig/build` with `cmake -A Win32 ..` and `cmake --build . --config Release`  
2. `cmake -E make_directory build`  
3. `cd build`  
4. `cmake -A Win32 ..`  
5. `cmake --build .`  
6. `.\bin\fanuc_example.exe` (Fwlib32.dll may need to be moved to cwd)  

# Development / Debug
Copy `compile_commands.json` from build dir to use with IDE  

---------------------------
# [4.Test/test_FOCAS/fwlib/examples/go](4.Test/test_FOCAS/fwlib/examples/go)
# Usage

0. Install go
1. `go build -o fwlib_example .`
2. `./fwlib_example`

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
