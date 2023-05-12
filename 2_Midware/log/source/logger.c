#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>

#include "logger.h"

#define LOG_MSG_LEN (2048)
#define MSG_LEN (2048)
#define WORD_LEN (128)
#define MIN_DATA(a, b) (((a) < (b)) ? (a) : (b))
#define MAX_DATA(a, b) (((a) > (b)) ? (a) : (b))
static char g_platform_name[128] = "JGW";

#define JGW_OPR_IDENT "1"
#define JGW_SYS_IDENT "2"
#define JGW_COM_IDENT "3"

static int type_mapping_int(enum logger_type type)
{
	switch (type)
	{
	case LOGGER_TYPE_SYS:
		return LOG_LOCAL0;
	case LOGGER_TYPE_OPR:
		return LOG_LOCAL1;
	case LOGGER_TYPE_COM:
		return LOG_LOCAL2;
	default:
		return -1;
	}
	return -1;
};

int logger_init(void)
{
	openlog(NULL, LOG_CONS | LOG_PID, LOG_LOCAL0);
	openlog(NULL, LOG_CONS | LOG_PID, LOG_LOCAL1);
	openlog(NULL, LOG_CONS | LOG_PID, LOG_LOCAL2);
	return 0;
}

void logger_deinit(void)
{
	closelog();
}

static char *strrstr(const char *s, char *str)
{
	char *p;
	int len = strlen(s);
	for (p = (char *)s + len - 1; p >= s; p--)
	{
		if ((*p == *str) && (memcmp(p, str, strlen(str)) == 0))
			return p;
	}
	return NULL;
}

/*根据文件路径转化成模块名称*/
char *file_to_module(char *buf, int lbuf, const char *filepath)
{
	char *ps = NULL, *pe = NULL;
	char module[32] = {0};
	if (!filepath || !buf)
		goto err;

	ps = strrstr(filepath, "/");
	if (!ps)
		goto err;
	ps++;

	pe = strrstr(ps, ".");
	if (!pe)
		goto err;
	memcpy(module, ps, MIN_DATA(pe - ps, sizeof(module) - 1));
	snprintf(buf, lbuf, "%s<%s>", g_platform_name, module);
	return buf;

err:
	return g_platform_name;
}

/* 操作日志 */
void logger_opr(int level,
				int error_id,
				int event_id,
				int event_subid,
				const char *filepath,
				const char *subject,
				const char *object,
				const char *msg, ...)
{
	char buf[LOG_MSG_LEN] = {0}, module[32] = {0};
	va_list ap;
	va_start(ap, msg);
	vsnprintf(buf, LOG_MSG_LEN, msg, ap);
	va_end(ap);

	int type = type_mapping_int(LOGGER_TYPE_OPR);
	syslog(type | level,
		   "%d %d %s %d %s %s %s %s",
		   event_id,
		   event_subid,
		   file_to_module(module, sizeof(module), filepath),
		   error_id,
		   subject,
		   object,
		   (error_id % 0x1000) ? "failed" : "success",
		   buf);
}

/* 系统日志 */
void logger_sys(int level,
				int error_id,
				int event_id,
				int event_subid,
				const char *filepath,
				const char *msg, ...)
{
	char buf[LOG_MSG_LEN] = {0}, module[32] = {0};

	va_list ap;
	va_start(ap, msg);
	vsnprintf(buf, LOG_MSG_LEN, msg, ap);
	va_end(ap);

	int type = type_mapping_int(LOGGER_TYPE_SYS);
	syslog(type | level,
		   "%d %d %s %d %s %s",
		   event_id,
		   event_subid,
		   file_to_module(module, sizeof(module), filepath),
		   error_id,
		   g_platform_name,
		   buf);
}

/* 通信日志 */
void logger_com(int level,
				int error_id,
				int event_id,
				int event_subid,
				const char *filepath,
				const char *subject,
				const char *object,
				const char *msg, ...)
{
	char buf[LOG_MSG_LEN] = {0}, module[32] = {0};
	va_list ap;
	va_start(ap, msg);
	vsnprintf(buf, LOG_MSG_LEN, msg, ap);
	va_end(ap);

	int type = type_mapping_int(LOGGER_TYPE_COM);
	syslog(type | level,
		   "%d %d %s %d %s %s %s",
		   event_id,
		   event_subid,
		   file_to_module(module, sizeof(module), filepath),
		   error_id,
		   subject,
		   object,
		   buf);
}

/* 设置平台名称 */
int set_platform_name(const char *name)
{
	if (NULL == name)
		return -1;

	if (strlen(name) > sizeof(g_platform_name))
		return -1;

	strncpy(g_platform_name, name, sizeof(g_platform_name) - 1);
	return 0;
}
