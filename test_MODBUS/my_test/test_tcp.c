#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <error.h> 
#include <memory.h> 
#include <sys/socket.h> 
#include "modbus.h"
#include "time.h"

//#define uint16_t unsigned short

void usr_main(void)
{
    fprintf(stderr,"Modbus TCP master \n");
    int i;
    int regs = 0;
    int num = 0;
    modbus_t *ctx = NULL;
    uint16_t tab_reg[64] = {0};
    uint16_t data[64] = {0xaacc,0x1F,0x23};
    struct timeval tval;
    tval.tv_sec=10;
    ctx = modbus_new_tcp("192.168.101.73",1502);     // 并未实际连接
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    modbus_set_slave(ctx, 1);       // 设置从站地址域值，并验证
    int rtn = modbus_connect(ctx);
    printf("step1: %d \n", rtn);
    modbus_set_response_timeout(ctx, &tval);

    while (1){
        memset(tab_reg, 0, sizeof(tab_reg));
        regs = modbus_read_registers(ctx, 0, 20, tab_reg);
        printf("[%4d][read num = %d]", num++, regs);
        for (i = 0; i < 20; i++){
            printf("<%#x>", tab_reg[i]);
        }
        printf("n");
        sleep(5);
    }

    modbus_close(ctx);
    modbus_free(ctx); 

    
}

