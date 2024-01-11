#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <pthread.h>

#define MAX_QUEUE_CAPACITY (512)
#define QUEUE_BLOCK_SIZE (1020)

/*消息数据存储区，最大QUEUE_BLOCK_SIZE*/
typedef struct _data_block_t {
	int length;
	unsigned char values[QUEUE_BLOCK_SIZE];
} data_block_t;

/*队列数组定义*/
typedef struct _queue_t {
	pthread_mutex_t _lock;
	pthread_cond_t _cond_empty/*空*/, _cond_full/*满*/;

	//Fixme:begin和end可能存在越界的情况
	int begin;				//队头偏移
	int end;				//队尾偏移

	unsigned int capacity;	//队列节点数量
	data_block_t blocks[];	
} queue_t;

queue_t* queue_new(int capacity);
void queue_free(queue_t* queue);
_Bool queue_empty(queue_t* queue);

_Bool queue_push(queue_t* queue, const void* ptr, int len);
_Bool queue_pop(queue_t* queue, void* ptr, int* plen);
_Bool queue_pop_waitfor(queue_t* queue, long millisec, void* ptr, int* plen);


typedef enum _queue_priority_e {
	HIGHEST, HIGHER, NORMAL
} priority_e;

#define PRIORITY_QUEUE_SIZE (3)

//优先级队列数组定义，3*queue_t
typedef struct _priority_queue_t {
	pthread_mutex_t _lock;
	pthread_cond_t _cond_empty;

	queue_t* prior_queue[PRIORITY_QUEUE_SIZE];
} priority_queue_t;

priority_queue_t* priority_queue_new(int capacity);
void priority_queue_free(priority_queue_t* queue);
_Bool priority_queue_empty(priority_queue_t* queue);

_Bool priority_queue_push(priority_queue_t* queue, priority_e priority, void* ptr, int len);
_Bool priority_queue_pop(priority_queue_t* queue, void* ptr, int* plen);
_Bool priority_queue_pop_waitfor(priority_queue_t* queue, long millisec, void* ptr, int* plen);

#endif /* QUEUE_H_ */
