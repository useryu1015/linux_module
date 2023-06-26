#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/******************************
 * 循环队列
 *  https://baike.baidu.com/item/循环队列/3685773?fr=aladdin
 */





// 队列的顺序存储结构(循环队列)
#define MAX_QSIZE 5 // 最大队列长度+1
typedef struct {
    int *base; // 初始化的动态分配存储空间
    int front; // 头指针，若队列不空，指向队列头元素
    int rear; // 尾指针，若队列不空，指向队列尾元素的下一个位置
} SqQueue;
 
 
// 构造一个空队列Q
SqQueue* Q_Init() {
    SqQueue *Q = (SqQueue*)malloc(sizeof(SqQueue));
    // 存储分配失败
    if (!Q){
        exit(OVERFLOW);
    }
    Q->base = (int *)malloc(MAX_QSIZE * sizeof(int));
    // 存储分配失败
    if (!Q->base){
        exit(OVERFLOW);
    }
    Q->front = Q->rear = 0;
    return Q;
}
 
// 销毁队列Q，Q不再存在
void Q_Destroy(SqQueue *Q) {
    if (Q->base)
        free(Q->base);
    Q->base = NULL;
    Q->front = Q->rear = 0;
    free(Q);
}
 
// 将Q清为空队列
void Q_Clear(SqQueue *Q) {
    Q->front = Q->rear = 0;
}
 
// 若队列Q为空队列，则返回1；否则返回-1
int Q_Empty(SqQueue Q) {
    if (Q.front == Q.rear) // 队列空的标志
        return 1;
    else
        return -1;
}
 
// 返回Q的元素个数，即队列的长度
int Q_Length(SqQueue Q) {
    return (Q.rear - Q.front + MAX_QSIZE) % MAX_QSIZE;
}
 
// 若队列不空，则用e返回Q的队头元素，并返回OK；否则返回ERROR
int Q_GetHead(SqQueue Q, int &e) {
    if (Q.front == Q.rear) // 队列空
        return -1;
    e = Q.base[Q.front];
    return 1;
}
 
// 打印队列中的内容
void Q_Print(SqQueue Q) {
    int p = Q.front;
    while (Q.rear != p) {
        cout << Q.base[p] << endl;
        p = (p + 1) % MAX_QSIZE;
    }
}
 
// 插入元素e为Q的新的队尾元素
int Q_Put(SqQueue *Q, int e) {
    if ((Q->rear + 1) % MAX_QSIZE == Q->front) // 队列满
        return -1;
    Q->base[Q->rear] = e;
    Q->rear = (Q->rear + 1) % MAX_QSIZE;
    return 1;
}
 
// 若队列不空，则删除Q的队头元素，用e返回其值，并返回1；否则返回-1
int Q_Poll(SqQueue *Q, int &e) {
    if (Q->front == Q->rear) // 队列空
        return -1;
    e = Q->base[Q->front];
    Q->front = (Q->front + 1) % MAX_QSIZE;
    return 1;
}


int main()
{
    SqQueue *queue = Q_Init();


    Q_Destroy(queue);
}
