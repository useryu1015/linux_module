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
