# modbus报文
#### 0x03 读取保持寄存器
TX：01 03 00 00 00 01 84 0A
		01：ID，从站地址
		03：功能码
		00 00：读取从站的起始地址
		00 01：读取（从站起始地址开始）总共的寄存器个数
		84 0A：CRC
RX：01 03 02 00 0C B8 41 
		01：从设备地址
		03：功能码
		02：返回数据的字节个数
		00 0C：寄存器值
		xx xx：CRC

#### 0x06 写入保存寄存器
TX：01 06 00 02 00 0C 28 0F
RX：01 06 00 02 00 0C 28 0F
	06：功能码
	00 02：写入字节数
	00 0C：写入值


	
# lib_modbus
 使用标准库，  测试 modbus交互报文、基础指令、寄存器		（modbus主站）
 sink程序modbus RTU/TCP 测试


# lib_modbus常用函数

modbus_t *mb;
int ret;
//创建一个modbus-rtu对象，指定串口号，波特率，校验位，数据位，停止位
//成功返回指针，否则返回NULL, 会调用malloc申请内存
mb = modbus_new_rtu("/dev/ttySP1", 115200, 'N', 8, 1);	//linux
mb = modbus_new_rtu("COM1", 115200, 'N', 8, 1);		//windows
//创建modbus-tcp对象，指定IP地址和端口号
mb = modbus_new_tcp("127.0.0.1", 502);	//TCP/IP

//设置从机地址，成功返回0, 否则返回-1
ret = modbus_set_slave(mb, slave);

//连接Modbus主机，成功返回0, 否则返回-1
ret = modbus_connect(mb);

//设置响应超时时间1s,200ms
ret = modbus_set_response_timeout(mb, 1, 200000);

//读取寄存器数据,起始地址2, 数量5, 保存到table数组中
//成功返回5, 否则返回-1
uint16_t *table;
ret = modbus_read_registers(mb, 2, 5, table);

//modbus设备关闭和释放内存
modbus_close(mb);
modbus_free(mb);

//写单个寄存器, 地址2写入56, 成功返回1，否则返回-1
ret = modbus_write_register(mb, 2, 56);

//写多个寄存器, 地址12起始，写入5个数据，成功返回5，否则返回-1
uint16_t table[5] = {11, 22, 33, 44, 55};
ret = modbus_write_registers(mb, 12, 5, table);

//写单个线圈，线圈地址写入TRUE,成功返回1，否则返回-1
ret = modbus_write_bit(mb, 11, TRUE);

//查看错误信息
char *err_str;
err_str = modbus_strerror(errno);


 /*
  * modbus_read_bits  读线圈  fc=1
  * modbus_read_input_bits 读输入线圈 fc=2
  * modbus_read_registers 读取保持寄存器 fc=3
  * modbus_read_input_registers 读输入寄存器 fc=4
  * modbus_write_bit 写一位数据（强置单线圈） fc=5
  * modbus_write_register 写单寄存器（预置单寄存器） fc=6
  * modbus_write_bits 写多位数据（强置多线圈） fc=15
  * modbus_write_and_read_registers 读/写多个寄存器 fc=23
 */



typedef struct {
    int nb_bits;
    int start_bits;
    int nb_input_bits;
    int start_input_bits;
    int nb_input_registers;
    int start_input_registers;
    int nb_registers;
    int start_registers;
    uint8_t *tab_bits;
    uint8_t *tab_input_bits;
    uint16_t *tab_input_registers;
    uint16_t *tab_registers;
} modbus_mapping_t;
————————————————
版权声明：本文为CSDN博主「猪哥-嵌入式」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/u012351051/article/details/104088091



# 电磁流量计
bb 48 e0 f4		48 bb f4 e0		384935
bb 48 60 f7		48 bb f7 60		384955





# linux 串口工具
## minicom
	