#ifndef __SYS_FILE_H__
#define __SYS_FILE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*读取文件内容至输出缓冲区中*/
    int32_t read_file(const char *filename, uint8_t *out, uint32_t *outl);

    /*保存文件*/
    int32_t write_file(const char *filename, uint8_t *in, uint32_t inl);

    /*保存字符串到文件*/
    int write_file_from_str(const char *file_name, const char *str);

#ifdef __cplusplus
}
#endif
#endif