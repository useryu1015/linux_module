#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stringtable.h"

//char g_tb_default_value[TB_ATTRIBUTE_MAX] = {'|', '-', '+', '\0'};
char g_tb_default_value[TB_ATTRIBUTE_MAX] = {'|', '=', '=', '\0'};

char g_line_break[2][2] = {{' ', '\n'}, {'\r', '\n'}};

int s_table_size_compute(int x, int y, int colwidth)
{
    int table_info = sizeof(s_t_matedate);
    int table_mate_size = sizeof(char *)*x*y;
    int table_wide = (colwidth+ 1)*x + 1 + 2;            /**<实际位宽+分隔符+结束符*/
    int table_size = table_wide*(2*y+3) + 1;

    return table_info + table_mate_size + table_wide + table_size;
}

char **s_table_get(s_t_matedate *md, int x, int y)
{
    char **tmp_p = (char **)md->table;
    return (char **)(tmp_p+(y*md->table_y) + x);
}

static int s_table_tableinit(s_t_matedate *md)
{
    char tmp = '-';
    int  colwidth = md->rel_line_num;   /*多一个结尾符号和换行字符*/
    char *point_spcae = NULL;
    char *data_spcae = NULL;
    char *tmp_data_spcae = NULL;

    point_spcae = (char *)md->table;
    data_spcae = point_spcae + sizeof(char *)*md->table_x*md->table_y;
    
    /*初始化第一阶段*/
    tmp_data_spcae = data_spcae;
    for(int y = 0; y < md->table_y*2 + 3 ; y++)
    {
        tmp = y%2 == 0 ? (g_tb_default_value[TB_COLUMN_SEPARATOR]):(' ');
        memset(tmp_data_spcae, tmp, colwidth);
        tmp_data_spcae[colwidth-2] =  g_line_break[md->table_type][0];
        tmp_data_spcae[colwidth-1] =  g_line_break[md->table_type][1];
        tmp_data_spcae += colwidth;
    }
    tmp_data_spcae[0] = g_tb_default_value[TB_STRING_END_SYMBOL];

    /*初始化第二阶段*/
    tmp_data_spcae = data_spcae;
    for(int y = 0; y < md->table_y*2 + 3; y++)
    {
        int x = 0;
        tmp = y%2 == 0?(g_tb_default_value[TB_INTERSECTION_SYMBOL]):(g_tb_default_value[TB_LINE_SEPARATOR]);
        for(x = 0 ; x < colwidth - 2 ; x++)
        {
            if(x%(md->colwidth+1) == 0){
                tmp_data_spcae[x] = tmp;
            }
        }

        tmp_data_spcae += colwidth;
    }

    /*初始化第三阶段--初始化表头*/
    tmp_data_spcae = data_spcae;
    tmp_data_spcae += colwidth + 1;
    memset(&tmp_data_spcae[1], ' ', colwidth - 5);
    memcpy(&tmp_data_spcae[((colwidth-2)/2) - (strlen(md->name)/2)], md->name, strlen(md->name));

    /*初始化第四阶段--初始化matedata指针*/
    tmp_data_spcae = data_spcae;
    tmp_data_spcae += 2*colwidth;
    char **tmp_p = NULL;
    for(int y = 0; y< md->table_y; y++)
    {
        tmp_data_spcae += colwidth;
        for(int x = 0; x < md->table_x; x++)
        {
            tmp_p = s_table_get(md, y , x);
            *tmp_p = &tmp_data_spcae[(x*(md->colwidth+1))+1];
        }
        tmp_data_spcae += colwidth;
    }

    /*初始化第五阶段*/
    md->data = data_spcae;

    return 0;
}

/*创建一个空的表, 表的所有空间由外部提供*/
char * s_table_create(char *table_name, int x, int y, int grid_wide, char *memspace, int num)
{
    int name_len = strlen(table_name);
    s_t_matedate *md = NULL;
    
    if (memspace == NULL){
        return NULL;
    }

    if (num < (sizeof(s_t_matedate) + sizeof(void*)*x*y + (x*(grid_wide+1)+1)*(y+2))){
        return NULL;
    }

    memset(memspace, 0x00, num);
    md = (s_t_matedate *)memspace;

    md->table_type = 0;
    md->table_x = x;
    md->table_y = y;
    md->mem_num = num;
    md->colwidth = grid_wide;
    md->rel_line_num = (md->colwidth+ 1)*x + 1 + 2;

    if ((name_len - 1 >  MAX_TABLE_NAME) || (name_len - 1 > md->rel_line_num - 5)){
        return NULL;
    }
    memcpy(md->name, table_name, name_len);

    s_table_tableinit(md);

    return memspace;
}

char * s_table_create_0(char *table_name, int x, int y, int colwidth)
{
    int tb_size = s_table_size_compute(x, y, colwidth);
    char *table_mem = (char *)malloc(tb_size);
    if (table_mem == NULL){
        return NULL;
    }

    return s_table_create(table_name, x, y, colwidth, table_mem, tb_size);
}

/*设置表的值,*/
int s_table_valueset(char *table, unsigned int x, unsigned int y, char *value)
{
    int cp_len = 0; 
    char **tmp_p = NULL;

    if(table == NULL || value == NULL){
        return 0;
    }

    s_t_matedate *md = (s_t_matedate *)table;
    tmp_p = s_table_get(md, y , x);

    if(x >= md->table_x || y >= md->table_y){
        return 0;
    }

    if(strlen(value) > md->colwidth){
        return 0;
    }

    tmp_p = s_table_get(md, y , x);
    memset(*tmp_p, ' ', md->colwidth);
    memcpy(*tmp_p, value, strlen(value));
    
    return 1;
}

static inline char *s_table_values_get(char *values, char separ, char *value)
{
    int i = 0;
    while(values[i] != separ && values[i] != '\0')
    {
        value[i] = values[i];
        i++;
    }

    value[i] = '\0';

    /*避免输入多个分隔符的情况*/
    while(values[i] == separ)
    {
        i++;
    }

    if(values[i] == '\0'){
        return NULL;
    }

    return &values[i];
}

static int s_table_valuesetxs_ex(char *table, int x_y, int s, char *values, char separ, int type)
{
    int ret = 0;
    int sum = 0;
    char *value_next = values;
    s_t_matedate *md = (s_t_matedate *)table;
    char **tmp_p = NULL;
    char tmp_value[MAX_COLWIDTH + 1] = {0};
    int wide = 0;

    if(type){
        wide = md->table_y;
    }else{
        wide = md->table_x;
    }

    for ( ; s < wide ; s++)
    {
        memset(tmp_value, 0x00, sizeof(tmp_value));
        value_next = s_table_values_get(value_next, separ, tmp_value);
        if(type){
            ret = s_table_valueset(table, x_y, s, tmp_value);
        }else{
            ret = s_table_valueset(table, s, x_y, tmp_value);
        }
        if (ret == 0){
            break;
        }
        sum += ret;

        if (value_next == NULL){
            break;
        }
    }

    return sum;
}

int s_table_valuesetxs(char *table, int x, int s, char *values, char separ)       /*一次设置一个列*/
{
    return s_table_valuesetxs_ex(table, x, s, values, separ, 0);
}

int s_table_valuesetys(char *table, int y, int s, char *values, char separ)      /*一次设置一个列*/
{
    return s_table_valuesetxs_ex(table, y, s, values, separ, 1);
}

/*查询表中的值*/
char * s_table_valueget(int x, int y)
{
    return NULL;
}

/*打印表*/
const char * s_table_string_get(char *table_name)
{
    if(table_name == NULL){
        return NULL;
    }
    s_t_matedate *md = (s_t_matedate *)table_name;

    return md->data;
}