# [3.Tool/shell_script/code_generator](3.Tool/shell_script/code_generator)
## 代码生成器
**简述：基于xx的代码生成器**
1. 将枚举值，生成字符串映射表
2. 将json配置，生成C解析伪代码
3. 将list链表，生成插入、删除、初始化函数

**TODO：**
1. 制作APP（插件、命令、网页）便于个人使用


### 1. enum映射表
**使用说明：**
1. 执行命令格式化代码区：./gen_enum_map.sh clean
2. 在enum_content.c 文件指定位置插入枚举声明
3. 执行生成脚本：./gen_enum_map.sh
![](/.document/code_gen/enum代码生成器-使用说明.png)

**使用效果：**

当前版本生成代码包括：枚举值字符串映射表、取值函数、调度函数
![](/.document/code_gen/enum代码生成器-使用效果.png)


### 2. json解析代码
**使用说明：**
1. 执行命令格式化代码区：./gen_json_parse.sh.sh clean
2. json_content.c 文件指定位置插入枚举声明
3. 执行生成脚本：./gen_json_parse.sh.sh
**使用效果：**
![](/.document/code_gen/json代码生成器.png)


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
# [.document](.document)
## linux_module
> LinuxC开发工具集（代码生成器、文本解析、脚本、编译、进程通信、日志、串口调试、字符驱动、协议栈等）

**TODO**
1. 整理并封装程序，包括：
    * socket： 封装sink套接字接口
    * serial： 
    * cjson、zmq、sqlite：
    * mqtt： 
2. 整理文件结构


---------------------------
