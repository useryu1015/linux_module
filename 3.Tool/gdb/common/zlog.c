#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "zlog.h"

#ifdef __linux__
#include <pthread.h>
pthread_mutex_t _log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static int g_zlog_plev = ZLOG_LEVEL_TRACE;	// 默认打印所有调试信息
// static int g_zlog_flev = ZLOG_LEVEL_ERROR;	// 默认输出级别为ERROR及以上的内容写入文件
static int g_zlog_flev = ZLOG_LEVEL_TRACE;	// 默认输出级别为ERROR及以上的内容写入文件

void zlog_set_printlev(zlog_level level) {
	g_zlog_plev = (int)level;
}

void zlog_set_zfilelev(zlog_level level) {
	g_zlog_flev = (int)level;
}

void zlog_limit_init(zlog_level xxx, zlog_level xx) {
	g_zlog_flev = (int)xxx;
	g_zlog_plev = (int)xx;
}


#ifdef USE_XLIAN_MODULE
zlog_config_t* ZLOG_CONFIG_PTR;
static const char* _module_names[] = { "virtual"/*(1)*/, "schedule"/*(2)*/, "gprs"/*(4)*/, "realtime"/*(8)*/,
        "hisdata"/*(16)*/, "modbus"/*(32)*/, "dlt645"/*(64)*/, "bacnet"/*(128)*/, "opcua"/*(256)*/,
        "gdw376"/*(512)*/, "iec104"/*(1024)*/, "crypto"/*(2048)*/, "httpd"/*(4096)*/, "nrsec3000" /*(8192)*/,
		"services"/*(16384)*/, "modem"/*(32768)*/, "config"/*(65536)*/, "commons"/*(131072)*/, "watchdog" /*(262144)*/};

static _Bool is_filter_accept(const char *file, const char *func, int line, int level) {
	int module_bits;

	if (NULL == ZLOG_CONFIG_PTR)
		return true;

	if (level < ZLOG_CONFIG_PTR->level + 2)
		return false;

	if (ZLOG_CONFIG_PTR->start_line && line < ZLOG_CONFIG_PTR->start_line) {
		return false;
	}

	if (ZLOG_CONFIG_PTR->end_line && line > ZLOG_CONFIG_PTR->end_line) {
		return false;
	}

	module_bits = ZLOG_CONFIG_PTR->module_bits;
//	module_bits= 16;
	if (module_bits) {
		unsigned int i;

		for (i = 0; i < sizeof(_module_names) / sizeof(_module_names[0]); ++i) {
			if (module_bits & (1 << i)) {
				if (strstr(file, _module_names[i])) {
					return true;
				}

			}
		}

		return false;
	}

	if ('\0' == ZLOG_CONFIG_PTR->file[0] && '\0' == ZLOG_CONFIG_PTR->func[0]) {
		return true;
	} else if ('\0' != ZLOG_CONFIG_PTR->file[0] && strstr(file, ZLOG_CONFIG_PTR->file)) {
		return true;
	} else if ('\0' != ZLOG_CONFIG_PTR->func[0] && strstr(func, ZLOG_CONFIG_PTR->func)) {
		return true;
	} else {
		return false;
	}
}
#endif

#ifdef USE_LOG_COLOURS
static const enum_color_t log_colours[] = {
		{ZLOG_LEVEL_ERROR, "[31;1m"}, /* LLL_ERR */
		{ZLOG_LEVEL_WARN, "[36;1m"}, /* LLL_WARN */
		{ZLOG_LEVEL_TRACE,  "[35;1m"}, /* LLL_NOTICE */
		{ZLOG_LEVEL_INFO, "[0m"}, /* LLL_INFO */
		{ZLOG_LEVEL_DEBUG, "[34;1m"}, /* LLL_DEBUG */
		{ZLOG_LEVEL_FATAL, "[31;1m"}
};

// inline const char *get_color_by_level(int id)		// fix: inline在aarch中报错
const char *get_color_by_level(int id)
{
    int i = 0, cnt = sizeof(log_colours) / sizeof(log_colours[0]);

    for (; i < cnt; ++i)
    {
        if (id == log_colours[i].id)
        {
            return log_colours[i].color;
        }
    }

    return ("[0m");
}
#endif

static void _log(FILE* fp, const char *file, const char *func, int line, int level) {
	time_t sys_time;
	struct tm sys_tm;

	time(&sys_time);
	localtime_r(&sys_time, &sys_tm);

	const char* level_desc[] = { "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL" };
	fprintf(fp, "%02d:%02d:%02d %ld %s %20s:%-4d %24s: ", sys_tm.tm_hour, sys_tm.tm_min, sys_tm.tm_sec,
	        syscall(SYS_gettid),level_desc[level], file, line, func);
}

static const char* get_simple_file(const char* file) {
	const char* str = strrchr(file, '/');
	return str ? (++str) : file;
}


/**
 * @brief 写入日志文件
 * 	tips: 程序重启，也只有出现异常才rewrite
 */
#ifdef USE_LOG_WFILE
void zlog_wfile(const char *file, const char *func, int line, int level, const char *format, ...) {
	va_list args;
	static int init_sign = 1;
	static FILE *wfp = NULL;
	const char* sfile = get_simple_file(file);

	if (level < g_zlog_flev) {
		return;
	}

	if (is_file_size_exceeded(LOG_FILE, LOGF_MAX_SIZE)) {
		init_sign = 1;
		init_sign = 1;

		if (wfp) fclose(wfp);
	}

	if (init_sign) {
		if (is_file_open(LOG_FILE)) {
			wfp = fopen(LOG_FILE, "a");		// 文件被其他进程占用
		} else {
			wfp = fopen(LOG_FILE, "w");
		}
		if (!wfp) return -1;
		// fclose(wfp);  // 关闭文件

		init_sign = 0;
	}

	if (level >= g_zlog_plev && wfp != NULL) {
#ifdef __linux__
		pthread_mutex_lock(&_log_mutex);
#endif
		do {
#ifdef USE_LOG_COLOURS
			printf("\x1b%s", get_color_by_level(level));
#endif
			_log(wfp, sfile, func, line, level);

			va_start(args, format);
			vfprintf(wfp, format, args);
			va_end(args);

#ifdef USE_LOG_COLOURS
			printf("\x1b[0m");
#endif
			fprintf(wfp, "\n");
			fflush(wfp);
		} while (0);
#ifdef __linux__
		pthread_mutex_unlock(&_log_mutex);
#endif
	}
}

#else
void zlog_wfile(const char *file, const char *func, int line, int level, const char *format, ...) {
	return;
}
#endif

void zlog(const char *file, const char *func, int line, int level, const char *format, ...) {
	va_list args;
	const char* sfile = get_simple_file(file);
    
	if (level >= g_zlog_plev) {
		FILE* fp = stdout;

#ifdef __linux__
		pthread_mutex_lock(&_log_mutex);
#endif
		do {
#ifdef USE_LOG_COLOURS
			printf("\x1b%s", get_color_by_level(level));
#endif
			_log(fp, sfile, func, line, level);

			va_start(args, format);
			vfprintf(fp, format, args);
			va_end(args);

#ifdef USE_LOG_COLOURS
			printf("\x1b[0m");
#endif
			fprintf(fp, "\n");
			fflush(fp);
		} while (0);
#ifdef __linux__
		pthread_mutex_unlock(&_log_mutex);
#endif
	}
}

void hlog(const char *file, const char *func, int line, int level, const void *buf, size_t buflen) {
	int i;

	const char* sfile = get_simple_file(file);

	// if (is_filter_accept(file, func, line, level)) {
	if (level >= g_zlog_plev) {
		FILE* fp = stdout;

#ifdef __linux__
		pthread_mutex_lock(&_log_mutex);
#endif
		do {
#ifdef USE_LOG_COLOURS
			printf("\x1b%s", get_color_by_level(level));
#endif			
			_log(fp, sfile, func, line, level);

			fprintf(fp, "[%d] - ", buflen);

			for (i = 0; i < buflen; ++i) {
				fprintf(fp, "%02x ", ((unsigned char*) buf)[i]);
			}

#ifdef USE_LOG_COLOURS
			printf("\x1b[0m");
#endif
			fprintf(fp, "\n");
			fflush(fp);
		} while (0);
#ifdef __linux__
		pthread_mutex_unlock(&_log_mutex);
#endif
	}
}

void hflog(const char *file, const char *func, int line, int level, const void *buf, size_t buflen, int fd) {
	int i;

	const char* sfile = get_simple_file(file);

	if (level >= g_zlog_plev) {
		FILE* fp = stdout;

#ifdef __linux__
		pthread_mutex_lock(&_log_mutex);
#endif
		do {
			_log(fp, sfile, func, line, level);

			fprintf(fp, "fd=%d, ", fd);
			fprintf(fp, "[%d] - ", buflen);

			for (i = 0; i < buflen; ++i) {
				fprintf(fp, "%02x ", ((unsigned char*) buf)[i]);
			}

			fprintf(fp, "\n");
			fflush(fp);
		} while (0);
#ifdef __linux__
		pthread_mutex_unlock(&_log_mutex);
#endif
	}
}

void hzlog(const char *file, const char *func, int line, int level, const char *str, const void *buf, size_t buflen) {
	int i;

	const char* sfile = get_simple_file(file);

	if (level >= g_zlog_plev) {
		FILE* fp = stdout;

#ifdef __linux__
		pthread_mutex_lock(&_log_mutex);
#endif
		do {
#ifdef USE_LOG_COLOURS
			printf("\x1b%s", get_color_by_level(level));
#endif
			_log(fp, sfile, func, line, level);

			fprintf(fp, "%s-", str);
			fprintf(fp, "dump[%d]:\n", buflen);

			for (i = 1; i <= buflen; ++i) {
				fprintf(fp, "%02x ", ((unsigned char*) buf)[i-1]);
                if(0==(i)%32){
			        fprintf(fp, "\n");
                }
			}

#ifdef USE_LOG_COLOURS
			printf("\x1b[0m");
#endif
			fprintf(fp, "\n");
			fflush(fp);
		} while (0);
#ifdef __linux__
		pthread_mutex_unlock(&_log_mutex);
#endif
	}
}
