#ifndef _LOG_ERR_H_
#define _LOG_ERR_H_
#include <unistd.h>
#include "err.h"

/*-------------------------------------------------- 
日志类型:   1:操作日志 2:系统日志 3:通信日志
-------------------------------------------------- */
enum logger_type {
    LOGGER_TYPE_OPR=1,
    LOGGER_TYPE_SYS=2,
    LOGGER_TYPE_COM=3
};

int logger_init(void);
void logger_deinit(void);

/* 设置平台名称 */
int set_platform_name(const char* name);

/* 操作日志 */
void logger_opr(int level,
		int error_id,
		int event_id,
        int event_subid,
		const char *filepath,
		const char *subject,
		const char *object,
		const char *msg, ...);

/* 系统日志 */
void logger_sys(int level,
		int error_id,
		int event_id,
        int event_subid,
		const char *filepath,
		const char *msg, ...);

/* 通信日志 */
void logger_com(int level,
		int error_id,
		int event_id,
        int event_subid,
		const char *filepath,
		const char *subject,
		const char *object,
		const char *msg, ...);
#endif

