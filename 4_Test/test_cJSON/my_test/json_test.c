#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_OBJECT_SNUM 56
#define MAX_OBJECT_LEN  32

void printJson(cJSON * root);
int loop = 0;           // 测试标签

struct object_mes_t
{
    int id_snum;                    // 单个点 子项总数 （title数量）
    char object_name[MAX_OBJECT_SNUM][MAX_OBJECT_LEN];     
    // char object_name[56][32];       // 最多56个（对象最大32字节）对象数量        

    int point_snum;                 // 点的总数
    char (**object_value)[MAX_OBJECT_LEN];

} object_mes;


void sys_init(){
    object_mes.id_snum = 0;
    object_mes.point_snum = 0;
    printf(" len %d  len %ld \n", MAX_OBJECT_LEN, sizeof(object_mes.object_name[0]));
}

// 如果该对象不存在，则追加到 对象ID 缓存
int add_once_object_name(const char * object){
    int i;
    char (*name)[MAX_OBJECT_LEN] = object_mes.object_name;            // 赋值失败。。。

    for (i=0; i<object_mes.id_snum; i++){
        if ( cJSON_strcasecmp(object, name[i]) == 0) {           // 如果已存在对象名称，则说明到了第二个点
            return -1;
        }
    }

    object_mes.id_snum += 1;
    strncpy(name[i], object, MAX_OBJECT_LEN);
    printf(" id_snum: %2d    object_id: %-32s    buf_id: %-32s \n", object_mes.id_snum, object, name[i]);
    
    return 0;
}


// 时间换空间： 多次遍历
// 空间换时间： 一次遍历所有元素， 链表保存数据
void get_item_name_loop(cJSON * root){
    int i; 

    printf(" ****** loop : %d  ****cJSON_GetArraySize(root): %d \n", loop, cJSON_GetArraySize(root));
    loop++;

    for(i=0; i<cJSON_GetArraySize(root); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type || cJSON_Array == item->type)     //如果对应键的值仍为cJSON_Object，非数据（对象名称、数组等）就递归调用printJson
            get_item_name_loop(item);
        else                                                            //值不为json对象就直接打印出键和值
        {
            printf("          %s->    \n", item->string);
            
            if (add_once_object_name(item->string) == -1) {
                continue;
            }
            // printf("%s\n", cJSON_Print(item));

        }
    }
}


// 解析出第一个 value 类型
void demo_cJSON_Print(cJSON *root){
    int i;

    printf(" ******  ****cJSON_GetArraySize(root): %d \n", cJSON_GetArraySize(root));
    // if (cJSON_GetArraySize(root) == 0)
    //     return cJSON_Print(root);

    for(i=0; i<=cJSON_GetArraySize(root); i++)   //遍历最外层json键值对
    {
        printf(" test \n");
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type || cJSON_Array == item->type) 
            demo_cJSON_Print(item);
        else
            return cJSON_Print(item);
    }
}

void get_item_value(cJSON * root) {
    int i,k;
    static j=0; 
    char (*ptr)[MAX_OBJECT_LEN];
    char *p = object_mes.object_name[0/*object_mes.id_snum-1*/];     // 最后一个元素值

    char (**p_value)[MAX_OBJECT_LEN] = object_mes.object_value;

    // static char key_object_name[] = "test";
    // static char key[MAX_OBJECT_LEN];
    // printf(" the key work is(with loop): %s \n", object_mes.object_name[0]);

    // printf(" ****** loop : %d  ****cJSON_GetArraySize(root): %d \n", loop, cJSON_GetArraySize(root));
    // loop++;
    for(i=0; i<cJSON_GetArraySize(root); i++)   // 遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type || cJSON_Array == item->type)     // 如果对应键的值仍为cJSON_Object，非数据（对象名称、数组等）就递归调用printJson
            get_item_value(item);
        else                                                            // 值不为json对象就直接打印出键和值
        {
            if (cJSON_strcasecmp(p, item->string) == 0) {               // 递归到最后一个元素(or第一个)，则添加一条xx
                printf("\n\n\n ---------------the key work is(with loop): %s \n", p);
                object_mes.point_snum++;
                ptr = (char(*)[MAX_OBJECT_LEN])malloc(sizeof(char) * MAX_OBJECT_LEN * object_mes.id_snum);
                // &(object_mes.object_value[object_mes.point_snum]) = ptr;             // malloc一条数据，并赋值给结构体
                // p_value[object_mes.point_snum] = ptr;
                j = 0;
            }
            
            // point[j] = "str";
            /* 遍历title 找到相匹配title并赋值 */
            for (k=0; k<object_mes.id_snum; k++){
                if ( cJSON_strcasecmp(object_mes.object_name[k], item->string) == 0) {      // 赋值给对应ID的 缓存
                    // ptr[k] =  cJSON_Print(item);
                    printf("  array:%d  snum:%d \n", cJSON_GetArraySize(item), object_mes.id_snum);
                    if (cJSON_GetArraySize(item) == 0) {
                        // return cJSON_Print(root);
                        printf(" **** str: %s \n", cJSON_Print(item));
                        strncpy(ptr[k], cJSON_Print(item), MAX_OBJECT_LEN);
                    }
                    else {
                        // demo_cJSON_Print(item);
                        // strncpy(ptr[k], demo_cJSON_Print(item), MAX_OBJECT_LEN);
                    }

                    printf("demo \n");
                    printf("  **data: %-40s    **data_buf: %-40s  \n", ptr[k], ptr[k]);
                    printf(" j:%d  point:%d\n", j, object_mes.point_snum);                    
                }
            }

            j++;
        }
    }
}













cJSON* load_json_file(const char* json_file) {
    const int FILE_BUF_SIZE = 1024 * 1024 * 20;		// 20M
    FILE* fp;
    char *jsonstr, *ps, *pe;
    cJSON* json;

    if (!(jsonstr = malloc(FILE_BUF_SIZE))) {
        return NULL;
    }
    ps = jsonstr;
    pe = ps + FILE_BUF_SIZE;

    if (!(fp = fopen(json_file, "rt"))) {
        printf("cann't open file '%s'", json_file);
        free(jsonstr);
        return NULL;
    }

    while (!feof(fp) && ps < pe) {
        int r = 1024 * 4;
        if (pe - ps < r)
            r = pe - ps;
        r = fread(ps, 1, r, fp);
        ps += r;
    }

    if (!feof(fp)) {
        printf("file '%s' is too large, length: %ld", json_file, ftell(fp));
        fclose(fp);
        free(jsonstr);
        return NULL;
    }

    printf("load file '%s', length: %ld\n\n", json_file, ftell(fp));
    fclose(fp);

    json = cJSON_Parse(jsonstr);
    if (!json) {
        printf("json parse failed: %s", jsonstr);
        free(jsonstr);
        return NULL;
    }

    free(jsonstr);

    return json;
}

void usr_main(){

    char json_file[] = "./example.json";
    cJSON * root;
    
    sys_init();
    
    root = load_json_file(json_file);           // 解析jison字符串，  FILE* -> char * -> cJSON *
    if (!root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        exit(1);
    }

    get_item_name_loop(root);                   // 1. 记录有多少项目title
    // return 0;
    // root = load_json_file(json_file); 
    get_item_value(root);
    
    exit(0);

}

