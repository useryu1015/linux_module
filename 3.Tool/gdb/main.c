#include <stdio.h>
#include <stdlib.h>

// 函数1：动态分配内存并返回指针
int* allocateMemory(int size) {
    int *ptr = (int *)malloc(size * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    return ptr;
}

// 函数2：释放动态分配的内存
void deallocateMemory(int *ptr) {
    free(ptr);
}

int main() {
    int size = 10;
    int *arr = allocateMemory(size); // 调用函数1，动态分配内存
    
    int i; // 在循环外部声明变量 i
    for (i = 0; i < size; ++i) {
        arr[i] = i; // 对分配的内存进行写操作
    }

    // 打印分配的内存内容
    printf("Allocated memory content: ");
    for (i = 0; i < size; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    deallocateMemory(arr); // 调用函数2，释放内存

    return 0;
}
