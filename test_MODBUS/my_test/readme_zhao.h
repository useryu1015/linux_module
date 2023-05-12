// lib_modbus 常用函数


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

