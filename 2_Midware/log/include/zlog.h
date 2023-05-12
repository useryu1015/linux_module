#ifndef _ZLOG_H_
#define _ZLOG_H_

#include <errno.h>  /* for errno */
#include <string.h> /* for strerror */
#include <stdarg.h> /* for va_list */
#include <stdio.h>  /* for size_t */

#ifdef __cplusplus
extern "C"
{
#endif

        typedef struct _enum_color_t
        {
                int id;
                const char *color;
        } enum_color_t;

        typedef struct _zlog_config_t
        {
                unsigned short tagid;
                short level;
                unsigned short module_bits;
                unsigned short start_line;
                unsigned short end_line;
                unsigned short _align[3];
                char file[4];
                char func[4];
        } zlog_config_t;

        extern zlog_config_t *ZLOG_CONFIG_PTR;

        void zlog(const char *file, const char *func, int line, int level, const char *format, ...);

        void hlog(const char *file, const char *func, int line, int level, const void *buf, size_t buflen);

        void hflog(const char *file, const char *func, int line, int level, const void *buf, size_t buflen, int fd);

		void hzlog(const char *file, const char *func, int line, int level, const char *str, const void *buf, size_t buflen);
        typedef enum _zlog_level
        {
                ZLOG_LEVEL_TRACE,
                ZLOG_LEVEL_DEBUG,
                ZLOG_LEVEL_INFO,
                ZLOG_LEVEL_WARN,
                ZLOG_LEVEL_ERROR,
                ZLOG_LEVEL_FATAL
        } zlog_level;

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#if defined __GNUC__ && __GNUC__ >= 2 || __BORLANDC__
#define _func __FUNCTION__
#else
#define _func "<unknown>"
#endif
#else
#define _func __FUNCTION__
#endif

#define zlog_errno()    \
        if (0 != errno) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_ERROR, "errno:%d, cause:%s", errno, strerror(errno))

#define zlog_fatal(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_FATAL, format, ##args)
#define zlog_error(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_ERROR, format, ##args)
#define zlog_warn(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_WARN, format, ##args)
#define zlog_info(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_INFO, format, ##args)
#define zlog_debug(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_DEBUG, format, ##args)
#define zlog_trace(format, args...) \
        zlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_TRACE, format, ##args)

#define hlog_fatal(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_FATAL, buf, buflen)
#define hlog_error(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_ERROR, buf, buflen)
#define hlog_warn(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_WARN, buf, buflen)
#define hlog_info(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_INFO, buf, buflen)
#define hlog_debug(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_DEBUG, buf, buflen)
#define hlog_trace(buf, buflen) \
        hlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_TRACE, buf, buflen)

#define hzlog_info(str, buf, buflen) \
        hzlog(__FILE__, _func, __LINE__, ZLOG_LEVEL_INFO, str, buf, buflen)

#define hflog_fatal(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_FATAL, buf, buflen, fd)
#define hflog_error(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_ERROR, buf, buflen, fd)
#define hflog_warn(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_WARN, buf, buflen, fd)
#define hflog_info(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_INFO, buf, buflen, fd)
#define hflog_debug(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_DEBUG, buf, buflen, fd)
#define hflog_trace(buf, buflen, fd) \
        hflog(__FILE__, _func, __LINE__, ZLOG_LEVEL_TRACE, buf, buflen, fd)

#ifdef __cplusplus
}
#endif

#endif /* ZLOG_H_ */
