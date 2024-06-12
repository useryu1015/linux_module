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


**Linux虚拟串口工具**
1. 安装 socat 工具： yum install socat / apt-get update; sudo apt-get install socat
2. 使用 socat 创建一对虚拟串口：socat -d -d pty,raw,echo=0 pty,raw,echo=0

## CRC16校验计算工具（modbus-crc16.cpp）
## 其他工具
1. 串口设备 从站地址 扫描
2. 传感器 数据报文 模拟
3. 批量测试
