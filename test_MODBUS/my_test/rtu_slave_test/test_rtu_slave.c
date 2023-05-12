/*******************************************************
 * 声明： modbus主站测试程序， 
 * modbus报文帧结构解析： 
 * RX：01 03 00 00 00 01 84 0A
		01：ID，从站地址
		03：功能码
		00 00：读取从站的起始地址
		00 01：读取（从站起始地址开始）总共的寄存器个数
		84 0A：CRC
 * TX：01 03 02 00 01 79 84  
		01：从设备地址
		03：功能码
		02：返回数据的字节个数
		00 01：寄存器值
		xx xx：CRC
 * 
 * 从站与主站区别在于： mb_mapping = modbus_mapping_new(500, 500, 500, 500);    // 设置寄存器空间，并通过结构体初始化值。
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
    int idx;
    int ret = 0;
    uint8_t device = 1;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    modbus_t *mb;
    modbus_mapping_t *mb_mapping;                   /* lib_modbus slave 通过该结构体，配置寄存器值 */
    char port[20];
    printf("argc = %d, argv[1] = %s\n", argc, argv[1]);
    if(argc == 2)
        strcpy(port, argv[1]);
    else 
        strcpy(port, PORT_NAME);
    
    printf("libmodbus modbu-rtu slave demo: %s, 19200, E, 8, 1\n", port);
    mb = modbus_new_rtu(port, 19200, 'E', 8, 1);
    if (mb == NULL)
    {
        modbus_free(mb);
        printf("new rtu failed: %s\n", modbus_strerror(errno));
        return 0;
    }

    //register: 15/16/17
    // mb_mapping = modbus_mapping_new_start_address(0, 0, 0, 0, 15, 3, 0, 0);
    mb_mapping = modbus_mapping_new(500, 500, 500, 500);        // 该函数申请并初始化4个区寄存器 缓存
    if(mb_mapping == NULL)
    {
        modbus_free(mb);
        printf("new mapping failed: %s\n", modbus_strerror(errno));
        return 0;
    }

    // 保持寄存器数据
    mb_mapping->tab_registers[0] = 1;
    mb_mapping->tab_registers[1] = 2;
    mb_mapping->tab_registers[2] = 3;

    // 输入寄存器
    mb_mapping->tab_input_registers[0] = 1;
    mb_mapping->tab_input_registers[1] = 0x55;


	//设置调试模式      即打印modbus收发报文
//	modbus_set_debug(mb, TRUE);


    modbus_set_slave(mb, device);           // 设置从设备地址
    ret = modbus_connect(mb);
    
    if(ret == -1)
    {
        modbus_free(mb);
        printf("connect failed: %s\n", modbus_strerror(errno));
        return 0;
    }
    printf("create modbus slave success\n");
    while(1)
    {
        do {
            ret = modbus_receive(mb, query);    //轮询串口数据，
        } while (ret == 0);

        if(ret > 0) //接收到的报文长度
        {
            printf("len=%02d: ", ret);
            for(idx = 0; idx < ret; idx++)
            {
                printf(" %02x", query[idx]);
            }
            printf("\n");

            modbus_reply(mb, query, ret, mb_mapping);           // 打包请求的数据 然后返回给主机
        }
        else
        {
            printf("Connection close\n");
            // modbus_mapping_free(mb_mapping);
            break;
        }

        // 寄存器递增
        mb_mapping->tab_registers[0]++;
        printf("  test : mb_mapping->tab_registers[0] is %d \n",mb_mapping->tab_registers[0]);
    }
    
    printf("quit the loop: %s \n", modbus_strerror(errno));
    
    modbus_mapping_free(mb_mapping);
    modbus_close(mb);
    modbus_free(mb);
    return 0;
}


/* 
 * modbus 从机寄存器地址配置
 */
// typedef struct {
//     int nb_bits;
//     int start_bits;
//     int nb_input_bits;
//     int start_input_bits;
//     int nb_input_registers;
//     int start_input_registers;
//     int nb_registers;
//     int start_registers;
//     uint8_t *tab_bits;
//     uint8_t *tab_input_bits;
//     uint16_t *tab_input_registers;
//     uint16_t *tab_registers;
// } modbus_mapping_t;
// ————————————————
// 版权声明：本文为CSDN博主「猪哥-嵌入式」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
