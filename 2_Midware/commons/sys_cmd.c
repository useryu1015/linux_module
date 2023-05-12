#include <stdint.h>
#include "sys_error.h"

int32_t system_result(const char *cmd, char *pout, int32_t outl)
{
    int len = 0;
    char *pgets = NULL;
    FILE *ptr;
    if (NULL != (ptr = popen(cmd, "r")))
    {
        if (pout)
        {
            while (NULL != (pgets = fgets(pout + len, outl - len, ptr)))
            {
                len += strlen(pgets);
            }
        }
        pclose(ptr);
    }
    else
    {
        return SYS_ERR_OTHER;
    }

    return SYS_ERR_NONE;
}

int32_t system_get_pid(char *srv_name, char *srv_param, int *pid)
{
    char cmd[1024] = {0}, pid_str[1024] = {0};
    SYS_PARA_CHECK(srv_name);

    /**首先查询进程id*/
    if (srv_param)
        snprintf(cmd, sizeof(cmd), "ps -ef | grep \"%s\" | grep \"%s\" | grep -v grep | awk '{print $2}'", srv_name, srv_param);
    else
        snprintf(cmd, sizeof(cmd), "ps -ef | grep \"%s\" | grep -v grep | awk '{print $2}'", srv_name);

    SYS_ERR_CHECK(system_result(cmd, pid_str, sizeof(pid_str)));

    if (1 != sscanf(pid_str, "%d", pid))
    {
        return SYS_ERR_STATE;
    }

    return SYS_ERR_NONE;
}
