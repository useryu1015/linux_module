#include <stdbool.h>
#include "cJSON.h"
char *vstring;
int vint;
double vdouble;
_Bool vbool;

/**
 * @brief 将json配置，生成C解析伪代码。
 * @tips 需要安装jq工具
 */

/*
 * Insert user code */
{
    "id":       1001,
    "name":	    "json_test",
    "enable":   true,
    "ip":       "192.168.1.151",
    "port":     1883,
    "cycle":    60,
    "double":   60.32
}
/* Insert end */

/*
 * Output code */
int parse_some_info(const cJSON *json)
{
    struct cJSON *next = NULL;

    if (!json) {
        return -1;
    }

    for (next = json->child; NULL != next; next = next->next) {
		if (0 == cJSON_strcasecmp("cycle", next->string)) {
		} else if (0 == cJSON_strcasecmp("double", next->string)) {
			vdouble = next->valuedouble;
		} else if (0 == cJSON_strcasecmp("enable", next->string)) {
			vbool = (next->valueint == cJSON_True) ? true : false;
		} else if (0 == cJSON_strcasecmp("id", next->string)) {
			vint = next->valueint;
		} else if (0 == cJSON_strcasecmp("ip", next->string)) {
			strncpy(vstring, next->valuestring, sizeof(vstring)-1);
		} else if (0 == cJSON_strcasecmp("name", next->string)) {
			strncpy(vstring, next->valuestring, sizeof(vstring)-1);
		} else if (0 == cJSON_strcasecmp("port", next->string)) {
			vint = next->valueint;
        }
    }

    return 0;
}
/* Output end */
