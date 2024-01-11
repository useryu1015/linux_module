
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <errno.h>

/*读取文件内容至输出缓冲区中*/
int32_t read_file(const char *filename, uint8_t *out, uint32_t *outl)
{
    int fd = -1, ret = 0;
    struct stat sb;

    if (!filename || !out || !outl || *outl <= 0)
    {
        return -1;
    }

    fd = open(filename, O_RDONLY, S_IRWXU);
    if (fd <= 0)
    {
        return errno;
    }

    if (ret = fstat(fd, &sb))
    {
        goto END;
    }

    if (*outl < sb.st_size)
    {
        ret = -1;
        goto END;
    }

    if (sb.st_size != read(fd, out, sb.st_size))
    {
        ret = -1;
        goto END;
    }

    *outl = sb.st_size;

END:
    close(fd);
    return ret;
}

/*保存文件*/
int32_t write_file(const char *filename, uint8_t *in, uint32_t inl)
{
    int fd = -1, ret = 0;

    if (!filename || !in || !inl)
    {
        return -1;
    }

    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd <= 0)
    {
        return errno;
    }

    if (inl != write(fd, in, inl))
    {
        ret = -1;
        goto END;
    }

END:
    close(fd);
    return ret;
}

int write_file_from_str(const char *file_name, const char *str)
{

    FILE *fp = fopen(file_name, "wt");

    if (!fp)
    {
        return -1;
    }

    fputs(str, fp);

    fclose(fp);

    return 0;
}