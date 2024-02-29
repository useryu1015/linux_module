#ifndef __LOG_H__
#define __LOG_H__

#include "logger.h"

#define MAX_LOG_SZ (2048)

#define SUCCESS	(0)
#define FAIL	(-1)

/* level-日志等级 */
#define LOG_EMERG		0    	/* system is unusable - 系统不可用*/
#define LOG_ALERT		1       /* action must be taken immediately - 必须立即执行操作*/
#define LOG_CRIT		2       /* critical conditions - 关键条件*/ 
#define LOG_ERR			3       /* error conditions - 错误条件*/
#define LOG_WARNING		4       /* warning conditions - 警告条件*/
#define LOG_NOTICE		5       /* normal but significant condition - 正常但重要的条件*/
#define LOG_INFO		6       /* informational - 信息*/
#define LOG_DEBUG		7       /* debug-level messages - 调试级别消息*/


/* ###################错误ID(error id)######################### */
/* #1-错误ID----系统配置(system configuration) */
#define E_S_SUCC			0x0000  /* 成功            */
#define E_S_FAIL			0x0001  /* 操作失败        */
#define E_S_PSWD			0x0002  /* 管理员口令错误  */
#define E_S_UKEY			0x0003  /* 管理员KEY无效   */
#define E_S_ACCT_LOCKED		0x0004  /* 管理员账户锁定  */
#define E_S_FILE			0x0005  /* 文件操作错误    */
#define E_S_DB				0x0006  /* 数据库错误      */
#define E_S_CONF			0x0007  /* 配置文件错误     */

/* #2-错误ID----密码运算(cryptographic operation) */
#define E_C_SUCC			0x1000 /* !! 成功 */
#define E_C_PUBKEY_ENC		0x1001 /* 公钥加密错误   */
#define E_C_PRIKEY_DEC		0x1002 /* 私钥解密错误   */
#define E_C_VERIFY_SIGN		0x1003 /* 验证签名失败   */
#define E_C_PRIKEY_SIGN		0x1004 /* 私钥签名错误   */
#define E_C_SYM_ENC			0x1005 /* 对称加密错误   */
#define E_C_SYM_DEC			0x1006 /* 对称解密错误   */
#define E_C_DIGEST_OPR		0x1007 /* 摘要运算错误   */
#define E_C_RANDOM_GEN		0x1008 /* 随机数生成失败 */
#define E_C_INTEG_VERIFY	0x1009 /* 完整性校验失败 */
#define E_C_CIPHER_HANDLE	0x100a /* 密码模块句柄错误*/

/* #3-错误ID----密钥管理(key management) */
#define E_K_SUCC			0x2000 /* !! 成功 */
#define E_K_GEN_SIGKEY		0x2001 /* 产生签名密钥对失败    */
#define E_K_IMP_MKEY		0x2002 /* 导入主密钥失败        */
#define E_K_IMP_DEV_CERT	0x2003 /* 导入设备证书失败      */
#define E_K_CERT_INVAL		0x2004 /* 证书无效              */
#define E_K_CERT_NOT_EXIST	0x2005 /* 证书不存在            */
#define E_K_ENC_KEY_PAIR	0x2006 /* 无效的加密公私钥对    */
#define E_K_SKEY_NOT_EXIST	0x2007 /* 终端通信主密钥不存在  */
#define E_K_TERM_FILE_NOT_EXIST		0x2008 /* 终端档案不存在        */
#define E_K_KEY_NOT_FOUND			0x2009 /* !!密钥不存在。 如灌注的密钥，使用时索引下没有密钥错误。*/
#define E_K_SKEY_MNG				0x200a /* !!会话密钥管理*/
#define E_K_ROOTKEY_INVAL	0x200b	/* 根密钥无效 */
#define E_K_SYMKEY_INVAL	0x200c	/* 对称密钥无效 */
#define E_K_ASYMKEY_INVAL	0x200d	/* 非对称密钥无效 */

/* #4-错误ID----通信状态(Communication state) */
#define E_N_SUCC			0x3000	/* !! 成功 */
#define E_N_SOCK			0x3001 /* 套接字操作失败  */
#define E_N_COMM_TMO		0x3002 /* 通信超时        */
#define E_N_MSG_FMT			0x3003 /* 报文格式无效    */
#define E_N_PRO_VER			0x3004 /* 协议版本无效    */
#define E_N_UNSUP_CMD		0x3005 /* 不支持的命令    */
#define E_N_DISCONN			0x3006 /* 连接被断开      */
#define E_N_BLOCK			0x3007 /* 白名单过滤阻断  */

/* !! 通用错误码 */
#define E_SUCC				0x4000	/* 成功 */
#define E_MEMORY			0x4001	/* 内存操作失败，如内存分配*/
#define E_INVAL_PARAM		0x4002	/* 参数错误 */
#define E_CONSTRUCT_DATA	0x4003	/* 构造数据失败，比如构造摆渡buffer等 */
#define E_INIT				0x4004	/* 初始化失败 */
#define E_OTHER				0x400f	/* 其他错误 */
/* ###################错误ID(error id)-end######################### */



/* #####################事件ID(event id)########################### */

#define EVENT_USER_OPE          0x0001      /* 用户操作 */
#define EVENT_NETWORK           0x0002      /* 网络事件 */
#define EVENT_SECURITY          0x0003      /* 安全事件 */
#define EVENT_HARDWARE          0x0004      /* 硬件事件 */
#define EVENT_PROGRESS          0x0005      /* 运行异常事件 */
#define EVENT_CIPHER            0x0006      /* 密码运算 */
#define EVENT_KEYMAN            0x0080      /* 密钥管理 */
#define EVENT_OTHER             0x00FF      /* 其他异常事件 */

/* #1 用户操作告警子类型 */
#define LOGIN_SUCCESS           0x0001      /* 登录成功 */
#define LOGIN_FAILURE           0x0002      /* 登录失败 */
#define LOGOUT_SUCCESS          0x0003      /* 用户退出 */
#define AUDIT_LOGINFO           0x0004      /* 查看日志 */
#define MODIFY_PARAM            0x0005      /* 修改配置 */
#define TIMEOUT_EXIT            0x0006      /* 超时退出 */
#define USER_OP_OTHER           0x00FF      /* 其他用户操作 */

/* #2 网络故障告警子类型 */
#define LINK_ABNORMAL           0x0001      /* 网络故障 */
#define LINK_RECOVER            0x0002      /* 网络恢复 */
#define ETHDEV_START            0x0003      /* 网卡启动 */
#define ETHDEV_CLOSE            0x0004      /* 网卡关闭 */
#define NETWORK_OTHER           0x00FF      /* 其他网络事件 */

/* #3 安全事件告警子类型 */
#define TERMINAL_LOGIN          0x0001      /* 终端非法登录 */
#define TERMINAL_VERIFY         0x0002      /* 终端认证失败 */
#define VIOLATE_ACL             0x0003      /* 违反访问控制 */
#define ATTEMPT_ATTACK          0x0004      /* 尝试攻击 */
#define ILLEGAL_FARME           0x0005      /* 非法报文 */
#define ILLEGAL_SQL             0x0006      /* 非法SQL */
#define APP_AUTH_FAILE          0x0007      /* 业务认证失败 */
#define SECURITY_OTHER          0x00FF      /* 其他安全事件 */

/* #4 硬件故障告警子类型 */
#define CRYPT_CARD_FAILURE      0x0001      /* 加密卡故障 */
#define CRYPT_CARD_RECOVER      0x0002      /* 加密卡恢复 */
#define FERRY_CARD_FAILURE      0x0003      /* 隔离卡故障 */
#define FERRY_CARD_RECOVER      0x0004      /* 隔离卡恢复 */
#define HARDWARE_OTHER          0x00FF      /* 其他硬件故障 */

/* #5 运行异常告警子类型 */
#define SERVICE_RESTART         0x0001      /* 服务重启 */
#define DATA_DAMAGED            0x0002      /* 数据损坏 */
#define APP_INITED              0x0080      /* 应用初始化 */
#define KEY_FAILURE             0x0081      /* 密钥错误 */
#define PROGRESS_OTHER          0x00FF      /* 其他运行异常事件 */

/* #6 密码运算异常告警子类型 */
#define CIPHER_CRYPTO           0x0001      /* 密码运算错误 */
#define CIPHER_PADDING          0x0002      /* 填充操作错误 */
#define CIPHER_OTHER            0x00FF      /* 其他密码运算异常事件 */

/* ###################事件ID(event id)-end######################### */

#ifndef USE_LOGGER

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#define DFT_LOG_BUF_SZ	(1024)

static inline  char* lvl_to_string(int lvl)
{
	char* level_string[LOG_DEBUG + 1] =
	{
		" \e[1;31mEMERG\e[0m ",
		" \e[1;31mALERT\e[0m ",
		" \e[1;31mCRIT\e[0m ",
		" \e[1;31mERR\e[0m ",
		" \e[1;31mWARNING\e[0m ",
		" NOTICE ",
		" INFO ",
		" DEBUG ",
	};
	if (lvl > LOG_DEBUG || lvl < 0)
		return NULL;
	return level_string[lvl];
}
#define log_fmt_msg(level, e, v, v1, fmt, ...)\
({\
        struct timespec 	t;				\
        clock_gettime(CLOCK_REALTIME, &t);			\
	fprintf(stdout, "%08lu.%06lu %-8s EID:%08X EVID:%08X EVSID:%08X" fmt, 	\
		t.tv_sec, t.tv_nsec /1000, 			\
		lvl_to_string(level),			\
		(e), (v), (v1),							\
		##__VA_ARGS__);					\
	fflush(stdout);						\
})

static inline void _log_fmg_msg(int lvl, int e, int v, int v1, char* user, char *fmt)
{
	struct timespec	t;
	char buf[MAX_LOG_SZ];
	clock_gettime(CLOCK_REALTIME, &t);
	snprintf(buf, sizeof(buf),
		"%08lu.%06lu %-8s EID:%08X EVID:%08X EVSID:%08X %s %s\n",
		t.tv_sec, t.tv_nsec /1000,
		lvl_to_string(lvl),
		e, v, v1, user, fmt);
	fprintf(stdout, buf);
	fflush(stdout);

}
#define comm_log_fmt_msg(level, e, v, v1, idt, fmt, ...)\
({\
        struct timespec 	t;				\
        clock_gettime(CLOCK_REALTIME, &t);			\
	fprintf(stdout, "%08lu.%06lu %-8s EID:%08X EVID:%08X EVSID:%08X %s " fmt, 	\
		t.tv_sec, t.tv_nsec /1000, 			\
		lvl_to_string(level),			\
		(e), (v), (v1),							\
		(idt),						\
		##__VA_ARGS__);					\
	fflush(stdout);						\
})
/**********************************通信日志*********************/
/* log emerg macro functtion */
#define comm_logemerg(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_EMERG,(e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define comm_logalert(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_ALERT,(e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define comm_logcrit(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_CRIT, (e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define comm_loge(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_ERR, (e),(v),(v1),(peer),fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define comm_logw(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_WARNING, (e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})

#ifdef USE_DEBUG
/* log notice macro functtion */
#define comm_logn(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_NOTICE, (e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define comm_logi(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_INFO, (e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define comm_logd(e, v, v1, local, peer, fmt,...)\
({\
	comm_log_fmt_msg(LOG_DEBUG, (e),(v),(v1),(peer), fmt, ##__VA_ARGS__);\
})
#else
#define comm_logn(e, v, v1, local, peer, fmt,...)
#define comm_logd(e, v, v1, local, peer, fmt,...)
#define comm_logi(e, v, v1, local, peer, fmt,...)
#endif // #ifdef DEBUG

/**********************************系统日志*********************/
/* log emerg macro functtion */
#define sys_logemerg(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_EMERG,(e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define sys_logalert(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_ALERT,(e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define sys_logcrit(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_CRIT, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define sys_loge(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_ERR, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define sys_logw(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_WARNING, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log notice macro functtion */
#define sys_logn(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_NOTICE, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define sys_logi(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_INFO, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define sys_logd(e, v, v1, fmt,...)\
({\
	log_fmt_msg(LOG_DEBUG, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})
/**********************************操作日志*********************/
/* log emerg macro functtion */
static void inline _opr_log(int lvl, int e, int v, int v1, char* user, char* fmt)
{
	_log_fmg_msg(lvl, e, v, v1, user, fmt);
}
#define opr_logemerg(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_EMERG,(e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define opr_logalert(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_ALERT,(e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define opr_logcrit(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_CRIT, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define opr_loge(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_ERR, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define opr_logw(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_WARNING, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log notice macro functtion */
#define opr_logn(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_NOTICE, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define opr_logi(e, v, v1, u, o, fmt,...)\
({\
	log_fmt_msg(LOG_INFO, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define opr_logd(e, v, v1,u, o, fmt,...)\
({\
	log_fmt_msg(LOG_DEBUG, (e),(v),(v1),fmt, ##__VA_ARGS__);\
})
#else
/**********************************通信日志*********************/
/* log emerg macro functtion */
#define comm_logemerg(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_EMERG, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define comm_logalert(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_ALERT, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define comm_logcrit(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_CRIT, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define comm_loge(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_ERR, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define comm_logw(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_WARNING, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log notice macro functtion */
#define comm_logn(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_NOTICE, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define comm_logi(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_INFO, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define comm_logd(e, v, v1, local, peer, fmt,...)\
({\
	logger_com(LOG_DEBUG, (e), (v), (v1), __FILE__, (local), (peer), fmt, ##__VA_ARGS__);\
})

/**********************************系统日志*********************/
/* log emerg macro functtion */
#define sys_logemerg(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_EMERG, (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define sys_logalert(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_ALERT, (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define sys_logcrit(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_CRIT, (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define sys_loge(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_ERR, (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define sys_logw(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_WARNING,  (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log notice macro functtion */
#define sys_logn(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_NOTICE, (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define sys_logi(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_INFO,  (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define sys_logd(e, v, v1, fmt,...)\
({\
	logger_sys(LOG_DEBUG,  (e), (v), (v1), __FILE__, fmt, ##__VA_ARGS__);\
})

/**********************************操作日志*********************/

/* log emerg macro functtion */
#define opr_logemerg(e, v, v1, u, o, fmt,...)\
({\
	logger_opr(LOG_EMERG, (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log alert macro functtion */
#define opr_logalert(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_ALERT, (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log crit macro functtion */
#define opr_logcrit(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_CRIT, (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log error macro functtion */
#define opr_loge(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_ERR, (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log warning macro functtion */
#define opr_logw(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_WARNING,  (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log notice macro functtion */
#define opr_logn(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_NOTICE, (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})

/* log info macro functtion */
#define opr_logi(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_INFO,  (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})
/* log info macro functtion */
#define opr_logd(e, v, v1, u, fmt,...)\
({\
	logger_opr(LOG_DEBUG,  (e), (v), (v1), __FILE__, (const char*)(u), (const char*)(o), fmt, ##__VA_ARGS__);\
})
#endif // #ifndef USE_LOGGER

#endif
