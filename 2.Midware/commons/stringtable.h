#ifndef _STRING_TABLE_H_
#define _STRING_TABLE_H_

#define MAX_TABLE_NAME 64
#define MAX_COLWIDTH   32

enum tb_attribute{
    TB_LINE_SEPARATOR = 0,
    TB_COLUMN_SEPARATOR = 1,
    TB_INTERSECTION_SYMBOL = 2,
    TB_STRING_END_SYMBOL = 3,

    TB_ATTRIBUTE_MAX
};

typedef struct s_table_matedate{
    char name[MAX_TABLE_NAME];
    int  table_type;        /*< 表格换行符类型（win/linux）,默认Linux*/
    int table_x;            /*< 总列数据*/
    int table_y;            /*< 总行数数据*/
    int mem_num;            /*< 表格内存总大小*/
    int colwidth;           /*< 列宽*/
    int rel_line_num;       /*< 一行的总字符个数*/
    //int rowheight;        /*< 列高*/
    char *data;
    char table[0];
}s_t_matedate;

/* 函数功能，计算一个表格需要的存储空间大小，为s_table_create提供的内存必须要大于等于该值
 * 参数：
 * para[int] :  x   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  colwidth  列宽
 * 返回值：
 * return :  返回表空间大小
 */
int s_table_size_compute(int x, int y, int colwidth);

/* 函数功能，创建一个表，外部提供表的存储空间
 * 参数：
 * para[int] :  x   列的个数
 * para[int] :  y   行的个数
 * para[int] :  colwidth  列宽
 * para[int] :  memspace  存储表格的内存空间
 * para[int] :  num       内存空间的大小
 * 返回值：
 * return :  返回表地址
 */
char * s_table_create(char *table_name, int x, int y, int colwidth, char *memspace, int num);

/* 函数功能，创建一个表，接口内部提供存储空间
 * 参数：
 * para[int] :  x   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  colwidth  列宽
 * 返回值：
 * return :  返回表地址
 * note：
 *     表存储空间由内部malloc申请，不使用时用户需要手动free
 */
char * s_table_create_0(char *table_name, int x, int y, int colwidth);

/* 函数功能，修改表格的属性
 * 参数：
 * para[int] :  x   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  colwidth  列宽
 * 返回值：
 * return :  返回表地址
 * note：
 *     表存储空间由内部malloc申请，不使用时用户需要手动free
 */
int s_table_attribute_set();

/* 函数功能，一次性设置一行连续多个值
 * 参数：
 * para[int] :  x   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  value  一个字符串，使用separ分隔 
 * para[int] :  separ  分隔符
 * 返回值：
 * return :  返回成功设置的个数
 */
int s_table_valueset(char *table, unsigned int x, unsigned int y, char *value);

/* 函数功能，一次性设置一行连续多个值
 * 参数：
 * para[int] :  table  表指针
 * para[int] :  x   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  value  一个字符串，使用separ分隔 
 * para[int] :  separ  分隔符
 * 返回值：
 * return :  返回成功设置的个数
 */
int s_table_valuesetxs(char *table, int x, int s, char *values, char separ);

/* 函数功能，一次性设置一列连续多个值
 * 参数：
 * para[int] :  table  表指针
 * para[int] :  y   行号
 * para[int] :  s   从行的第几个开始设置
 * para[int] :  value  一个字符串，使用separ分隔 
 * para[int] :  separ  分隔符
 * 返回值：
 * return    :  返回成功设置的个数
 */
int s_table_valuesetys(char *table, int y, int s, char *values, char separ);

/* 函数功能, 查询表中的值
 * 参数：
 * para[int] :  table  表指针
 * para[int] :  y   行号
 * para[int] :  s   从行的第几个开始设置
 * 返回值：
 * return    :  返回成功表格中的值，失败返回 NULL
 */
char * s_table_valueget(int x, int y);

/* 函数功能,打印表格
 * 参数：
 * para[int] :  table  表指针
 * 返回值：
 * return    :  返回成功表格数据区域地址，失败返回 NULL
 */
const char * s_table_string_get(char *s_table);

#endif