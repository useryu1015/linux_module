#ifndef __SYS_ERROR_H__
#define __SYS_ERROR_H__

#include "zlog.h"

typedef enum _sys_err_e
{
    SYS_ERR_NONE = 0x0000,
    SYS_ERR_PARA,   // 参数错误
    SYS_ERR_LEN,    // 长度错误
    SYS_ERR_MEM,    // 内存错误
    SYS_ERR_SOCKET, // socket错误
    SYS_ERR_RECV,   // 接收数据错误
    SYS_ERR_SEND,   // 发送数据错误
    SYS_ERR_FUNC,   // 处理函数错误
    SYS_ERR_DB,     // 数据库操作错误
    SYS_ERR_STATE,  // 状态错误
    SYS_ERR_FRAME,  // 帧格式错误
    SYS_ERR_LINK,   // 链接错误
    SYS_ERR_FILE,   // 文件操作错误
    SYS_ERR_STORE,  // 存储错误
    SYS_ERR_INDEX,  // 序号错误
    SYS_ERR_CONFIG, // 配置错误
    SYS_ERR_XML,    // XML错误

    SYS_ERR_OTHER = 0x0fff,
    SYS_ALREADY_DEAL = 0xf000 // SSAL协议内部已处理，外部不需要再进行转发
} sys_err_e;

#define SYS_PARA_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_PARA; \
    } while (0)

#define SYS_LEN_CHECK(expr)     \
    do                          \
    {                           \
        if (!(expr))            \
            return SYS_ERR_LEN; \
    } while (0)

#define SYS_MEM_CHECK(expr)     \
    do                          \
    {                           \
        if (!(expr))            \
            return SYS_ERR_MEM; \
    } while (0)

#define SYS_SOCK_CHECK(expr)       \
    do                             \
    {                              \
        if (!(expr))               \
            return SYS_ERR_SOCKET; \
    } while (0)

#define SYS_RECV_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_RECV; \
    } while (0)

#define SYS_SEND_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_SEND; \
    } while (0)

#define SYS_FUNC_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_FUNC; \
    } while (0)

#define SYS_DB_CHECK(expr)     \
    do                         \
    {                          \
        if (!(expr))           \
            return SYS_ERR_DB; \
    } while (0)

#define SYS_STATE_CHECK(expr)     \
    do                            \
    {                             \
        if (!(expr))              \
            return SYS_ERR_STATE; \
    } while (0)

#define SYS_FRAME_CHECK(expr)     \
    do                            \
    {                             \
        if (!(expr))              \
            return SYS_ERR_FRAME; \
    } while (0)

#define SYS_LINK_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_LINK; \
    } while (0)

#define SYS_FILE_CHECK(expr)     \
    do                           \
    {                            \
        if (!(expr))             \
            return SYS_ERR_FILE; \
    } while (0)

#define SYS_STORE_CHECK(expr)     \
    do                            \
    {                             \
        if (!(expr))              \
            return SYS_ERR_STORE; \
    } while (0)

#define SYS_INDEX_CHECK(expr)     \
    do                            \
    {                             \
        if (!(expr))              \
            return SYS_ERR_INDEX; \
    } while (0)

#define SYS_CONFIG_CHECK(expr)     \
    do                             \
    {                              \
        if (!(expr))               \
            return SYS_ERR_CONFIG; \
    } while (0)

#define SYS_XML_CHECK(expr)     \
    do                          \
    {                           \
        if (!(expr))            \
            return SYS_ERR_XML; \
    } while (0)

#define SYS_OTHERERR_CHECK(expr)  \
    do                            \
    {                             \
        if (!(expr))              \
            return SYS_ERR_OTHER; \
    } while (0)

#define SYS_ERR_CHECK(expr)                \
    do                                     \
    {                                      \
        int32_t __ret__ = (expr);          \
        if (__ret__)                       \
        {                                  \
            zlog_error("ret=%d", __ret__); \
            return __ret__;                \
        }                                  \
    } while (0)

#endif

#define SYS_CHECK_END(expr, cmd, target) \
    if (expr)                            \
    {                                    \
        cmd;                             \
        goto target;                     \
    }
