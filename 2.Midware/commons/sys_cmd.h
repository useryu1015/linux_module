#ifndef __SYS_CMD_H__
#define __SYS_CMD_H__

#include <stdint.h>

int32_t system_result(const char *cmd, char *pout, int32_t *outl);

int32_t system_get_pid(char *srv_name, char *srv_param, int *pid);

#endif