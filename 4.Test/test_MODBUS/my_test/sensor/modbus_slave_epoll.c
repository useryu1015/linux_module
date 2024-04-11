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

    int socket;
    int client_socket[10] = {0};  // 支持最多10个连接
    int num_clients = 0;

    // 监听 TCP 端口
    socket = modbus_tcp_listen(ctx, 10);  // 最多支持10个等待连接
    if (socket == -1) {
        fprintf(stderr, "Failed to listen to TCP port: %s\n", modbus_strerror(errno));
        modbus_mapping_free(mb_mapping);
        modbus_free(ctx);
        return -1;
    }

    // 循环处理 Modbus 请求
    for (;;) {
        if (num_clients < 10) {
            // 等待新连接
            client_socket[num_clients] = modbus_tcp_accept(ctx, &socket);
            if (client_socket[num_clients] == -1) {
                fprintf(stderr, "Failed to accept connection: %s\n", modbus_strerror(errno));
                break;
            }
            printf("Accepted new connection\n");
            num_clients++;
        }

        for (int i = 0; i < num_clients; ++i) {
            uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
            int rc;

            rc = modbus_receive(ctx, query);
            if (rc != -1) {
                /* rc is the query size */
                modbus_reply(ctx, query, rc, mb_mapping);
            } else {
                /* Connection closed by the client or error */
                close(client_socket[i]);
                client_socket[i] = -1;
                printf("Client disconnected\n");
            }
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    // 关闭所有客户端套接字
    for (int i = 0; i < num_clients; ++i) {
        if (client_socket[i] != -1) {
            close(client_socket[i]);
        }
    }

    // 释放资源
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
