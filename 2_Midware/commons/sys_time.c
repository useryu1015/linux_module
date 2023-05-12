#include <time.h>
#include <sys/timeb.h>
#include <stdbool.h>
#include "sys_time.h"
#include "zlog.h"



int get_sys_time(char *time)
{
    int rtn = 0;
    struct timeb tTimeB;

    if (!time)
        return -1;

    ftime(&tTimeB);//获取当前的秒数和毫秒数
    struct tm *tTM = localtime(&tTimeB.time);//将秒数转为时间格式
    
    // zlog_info("%04d-%02d-%02d %02d:%02d:%02d.%03d", tTM->tm_year + 1900, 
    //             tTM->tm_mon + 1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, 
    //             tTimeB.millitm);

    // 格式： "2023-04-05 09:28:47.475"
    rtn = sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d.%03d", tTM->tm_year + 1900, 
    tTM->tm_mon + 1, tTM->tm_mday, tTM->tm_hour, tTM->tm_min, tTM->tm_sec, 
    tTimeB.millitm);

    return rtn;
}


