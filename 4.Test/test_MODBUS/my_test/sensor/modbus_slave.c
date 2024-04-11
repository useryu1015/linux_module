#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <modbus.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port> <slave_address>\n", argv[0]);
        return -1;
    }

    int port = atoi(argv[1]);
    int slave_address = atoi(argv[2]);

    int socket;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;

    ctx = modbus_new_tcp("127.0.0.1", port);
    if (ctx == NULL) {
        fprintf(stderr, "Failed to create Modbus context: %s\n", modbus_strerror(errno));
        return -1;
    }

    // 设置从站地址
    if (modbus_set_slave(ctx, slave_address) == -1) {
        fprintf(stderr, "Failed to set slave address: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    // 创建 Modbus 映射
    mb_mapping = modbus_mapping_new(100, 100, 1000, 200);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    // 初始化保持寄存器数组的值
    for (int i = 0; i < mb_mapping->nb_registers; ++i) {
        mb_mapping->tab_registers[i] = i + 1;
    }

    // 监听 TCP 端口
    socket = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &socket);

    // 循环处理 Modbus 请求
    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;

        rc = modbus_receive(ctx, query);
        if (rc != -1) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
        } else {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    // 释放资源
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
