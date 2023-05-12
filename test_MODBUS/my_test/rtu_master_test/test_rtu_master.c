/*******************************************************
 * 声明： modbus主站测试程序， 
 * modbus报文帧： 
 * TX：01 03 00 00 00 01 84 0A
		01：ID，从站地址
		03：功能码
		00 00：读取从站的起始地址
		00 01：读取（从站起始地址开始）总共的寄存器个数
		84 0A：CRC
 * 
 * *****************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "modbus.h"
#include <errno.h>
#include <unistd.h>

#define PORT_NAME "/dev/ttyS1"

int main(int argc, char *argv[])
{
    int i;
    int ret;
    uint16_t table[3];
    modbus_t *mb;
    char port[20];
    printf("argc = %d, argv[1] = %s\n", argc, argv[1]);
    if(argc == 2)
        strcpy(port, argv[1]);
    else 
        strcpy(port, PORT_NAME);
    printf("libmodbus modbu-rtu master demo: %s, 9600, N, 8, 1\n", port);

    mb = modbus_new_rtu(port, 9600, 'N', 8, 1);             // 1. bind：绑定串口/网口

    if (mb == NULL)
    {
        modbus_free(mb);
        printf("new rtu failed: %s\n", modbus_strerror(errno));
        return 0;
    }

    modbus_set_slave(mb, 1);                            // 2. //设置从机地址，成功返回0, 否则返回-1
    ret = modbus_connect(mb);                           // 3. //连接Modbus主机，成功返回0, 否则返回-1
    
    if(ret == -1)
    {
        modbus_close(mb);
        modbus_free(mb);
        printf("connect failed: %s\n", modbus_strerror(errno));
        return 0;
    }

    for(i = 0; i < 3; i++)            
        table[i] = 1;

    while (1)    
    while(1)
    {
        sleep(2);

        const uint8_t sum = 1;
        // param 1：从设备信息    2：起始地址     3：数量     4：buf      // rtn: 成功返回3(请求数), 否则返回-1
        ret = modbus_read_registers(mb, 0x00, sum, table);            // 4.1 读取寄存器数据:0x03 
        if(ret == sum)
            printf("read success : 0x%02x 0x%02x 0x%02x \n", table[0], table[1], table[2]);
        else
        {
            printf("read error: %s\n", modbus_strerror(errno));
            break;
        }

        for(i = 0; i < sum; i++)
            table[i] += 1;

        ret = modbus_write_registers(mb, 0x00, 3, table);           // 4.2 向寄存器写入数据:0x10
        if(ret == 3)
            printf("write success: 0x%02x 0x%02x 0x%02x \n", table[0], table[1], table[2]);
        else
        {
            printf("write error: %s\n", modbus_strerror(errno));
            break;
        }
        sleep(1);
    }

    modbus_close(mb);
    modbus_free(mb);
    // system("pause");
    return 0;
}



