#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "iohandle.h"

// 向arg1 写数据
static int _write(srv_args_t* p_args, const void *buf, const size_t len)
{
    int rc = 0;
    const char *ptr = buf, *ps = ptr, *pe = ptr + len;

    while (ps < pe) {
        rc = p_args->p_io->send(p_args, ps, pe - ps);	// ？？ 向

        if (rc <= 0) {
            return rc;
        } else {
            ps += rc;
        }
    }

    return rc;
}

static int _read(srv_args_t* p_args, void* buf, size_t len)
{
    int rc = 0;
    char *ptr = buf, *ps = ptr, *pe = ptr + len;

    while (ps < pe) {
        rc = p_args->p_io->recv(p_args, ps, pe - ps);

        if (rc <= 0) {
            return rc;
        } else {
            ps += rc;
        }
    }

    rc = ps - ptr;

    return rc;
}

int mqtt_write(srv_args_t* p_args, \
        const unsigned char *buf, const int len)
{
    if (!buf)
        return -1;

    return _write(p_args, buf, len);
}

int mqtt_read_len(srv_args_t* p_args, \
        unsigned char* buf, int *len)
{
    int i = 0, multipler = 1;
    unsigned char *ps = buf;

    for (i = 0; i < 4; ++i) {
        if (1 != _read(p_args, ps, 1))
            return 0;

        *len += ((*ps) & 0x7f)*multipler;
        multipler *= 128;

        if (0 == ((*ps) & 0x80))
            break;

        ps++;
    }

    return i+1;
}

int mqtt_read(srv_args_t* p_args, unsigned char* buf, int len)
{
    int remain_len = 0, offset = 0, nread = 0;
    unsigned char *ps = buf;

    if (!ps)
        return -1;

    memset(buf, 0, len);

    nread = _read(p_args, ps, 1);
    if (1 != nread)
        return nread;
    ps += 1;

    offset = mqtt_read_len(p_args, ps, &remain_len);
    ps += offset;

    if (remain_len != _read(p_args, ps, remain_len))
        return -1;

    return remain_len + offset + 1;
}
