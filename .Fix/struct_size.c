#include <stdio.h>

typedef struct __test

{

char c_x; //1 byte

float f_y; //4 bytes

} test_type1_t; //此时字节对齐为8



typedef struct __test2

{

char c_x; //1 byte

double d_y; //8 bytes

} test_type2_t; //此时字节对齐，为16



typedef struct __test3

{

//char c_x; //1 byte

double d_y; //8 bytes

char c_x;

} test_type3_t; //此时字节对齐，仍然是16



typedef struct __test4

{

char c_x;

double d_y;

} __attribute__((packed)) test_type4_t; //此处已取消字节对齐



int sturct_size()
{

    test_type1_t test1_temp;
    test_type2_t test2_temp;
    test_type3_t test3_temp;
    test_type4_t test4_temp;

    printf("the bytes count of 'char + float' is :%d\n",sizeof(test1_temp));
    printf("the bytes count of 'char + double' is :%d\n",sizeof(test2_temp));
    printf("the bytes count of 'double + char' is :%d\n",sizeof(test3_temp));
    printf("the bytes count of 'char + double +attribute(packed)' is :%d\n",sizeof(test4_temp));


    struct DATA1
    {
        short 	flag1;
        int	  	data;
        short 	flag2;
    }DATA1;

    #pragma pack(push, 2)       // 两字节对齐
    struct DATA0
    {
        short 	flag1;
        int	  	data;
        short 	flag2;
    }DATA0;
    #pragma pack(pop)

    struct DATA2
    {
        short 	flag1;
        short 	flag2;
        int     data;
    }DATA2;  

    printf("DATA1 size:%d\n", sizeof(DATA1));
    printf("DATA0 size:%d\n", sizeof(DATA0));
    printf("DATA2 size:%d\n", sizeof(DATA2));

    return 0;

}