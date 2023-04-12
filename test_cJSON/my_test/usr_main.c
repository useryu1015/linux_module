#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main()
{
    // char * jsonStr = "{\"semantic\":{\"slots\":{\"name\":\"张三\"}}, \"rc\":0, \"operation\":\"CALL\", \"service\":\"telephone\", \"text\":\"打电话给张三\"}";
    cJSON * root = NULL;
    // cJSON * item = NULL;//cjson对象

    usr_main();

    printf("\n\n test ok \n");
    return 0;
    
    char json_file[] = "./example.json";
    root = load_json_file(json_file);

    // root = cJSON_Parse(jsonStr);    // 解析json字符串；
    if (!root) 
    {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    }
    else
    {
        

	/*
        //char *json_test = cJSON_Print(root);
        //printf("mytest\n%s\n\n", json_test);
        printf("%s\n", "有格式的方式打印Json:");
        printf("%s\n\n", cJSON_Print(root));

        printf("%s\n", "无格式方式打印json：");
        printf("%s\n\n", cJSON_PrintUnformatted(root));

        printf("%s\n", "一步一步的获取name 键值对:");           // 双\n，编码格式
        printf("%s\n", "获取semantic下的cjson对象:");
        item = cJSON_GetObjectItem(root, "semantic");       // 获取 键值 
        printf("%s\n", cJSON_Print(item));

        //printf("\n\n结构体： \n%s", item->)

        printf("%s\n", "获取slots下的cjson对象");
        item = cJSON_GetObjectItem(item, "slots");
        printf("%s\n", cJSON_Print(item));
        printf("%s\n", "获取name下的cjson对象");
        item = cJSON_GetObjectItem(item, "name");
        printf("%s\n\n\n\n", cJSON_Print(item));
  
        printf("%s:", item->string);   //看一下cjson对象的结构体中这两个成员的意思
        printf("%s\n", item->valuestring);

	*/     
 
        printf("\n%s\n", "打印json所有最内层键值对:");
        printJson(root);
    }
    return 0;    
}






void printJson(cJSON * root)//以递归的方式打印json的最内层键值对
{
    int i; 
    for(i=0; i<cJSON_GetArraySize(root); i++)   //遍历最外层json键值对
    {
        cJSON * item = cJSON_GetArrayItem(root, i);
        if(cJSON_Object == item->type || cJSON_Array == item->type)     //如果对应键的值仍为cJSON_Object，非数据（对象名称、数组等）就递归调用printJson
            printJson(item);
        else                                                            //值不为json对象就直接打印出键和值
        {
            printf("%s->", item->string);
            printf("%s\n", cJSON_Print(item));
        }
    }
}