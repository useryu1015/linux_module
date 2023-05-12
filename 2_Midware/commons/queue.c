#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include "queue.h"

//pthread_cond_timedwait等待绝对时间，time(0)只能精确到秒级
static void _set_timespec(struct timespec* _timespec, long millisec) {
	struct timeval now;
	gettimeofday(&now, NULL);
	long nsec = now.tv_usec * 1000 + (millisec % 1000) * 1000000;
	_timespec->tv_sec=now.tv_sec + nsec / 1000000000 + millisec / 1000;
	_timespec->tv_nsec=nsec % 1000000000;

//	_timespec->tv_sec = time(0) + millisec / 1000;
//	_timespec->tv_nsec = (millisec % 1000) * 1000;
}

/*队列初始化：创建队列结构体，并根据队列大小开辟空间*/
queue_t* queue_new(int capacity) {
	queue_t* queue;

	if (capacity < 1) {
		capacity = 1;
	} else if (capacity > MAX_QUEUE_CAPACITY) {
		capacity = MAX_QUEUE_CAPACITY;
	}

	queue = malloc(sizeof(queue_t) + sizeof(data_block_t) * capacity);
	if (!queue)
		return 0;

	queue->capacity = capacity;
	queue->begin = 0;
	queue->end = 0;
	pthread_mutex_init(&queue->_lock, 0);
	pthread_cond_init(&queue->_cond_empty, 0);
	pthread_cond_init(&queue->_cond_full, 0);

	return queue;
}

void queue_free(queue_t* queue) {
	if (!queue)
		return;

	pthread_mutex_destroy(&queue->_lock);
	pthread_cond_destroy(&queue->_cond_empty);
	pthread_cond_destroy(&queue->_cond_full);

	free(queue);
}

/*将数据放入队列中*/
_Bool queue_push(queue_t* queue, const void* ptr, int len) {
	void* dst;
	_Bool rc = false;

	if (!queue || !ptr || len > QUEUE_BLOCK_SIZE)
		return false;

	pthread_mutex_lock(&queue->_lock);
	if ((queue->end - queue->begin) >= queue->capacity) {
		pthread_cond_wait(&queue->_cond_full, &queue->_lock);
	}

	if ((queue->end - queue->begin) < queue->capacity) {
		int idx = queue->end % queue->capacity;
		memmove(queue->blocks[idx].values, ptr, len);
		queue->blocks[idx].length = len;
		++queue->end;
		pthread_cond_signal(&queue->_cond_empty);
		rc = true;
	}
	pthread_mutex_unlock(&queue->_lock);

	return rc;
}

/*将数据从队列中取出*/
_Bool queue_pop(queue_t* queue, void* ptr, int* plen) {
	void* src;
	_Bool rc = false;

	if (!queue || !ptr || !plen)
		return rc;

	pthread_mutex_lock(&queue->_lock);
	if (queue->begin >= queue->end) {
		pthread_cond_wait(&queue->_cond_empty, &queue->_lock);
	}

	if (queue->begin < queue->end) {
		int idx = queue->begin % queue->capacity;
		if (*plen >= queue->blocks[idx].length) {
			*plen = queue->blocks[idx].length;
			memmove(ptr, queue->blocks[idx].values, *plen);
			++queue->begin;
			pthread_cond_signal(&queue->_cond_full);
			rc = true;
		}
	}
	pthread_mutex_unlock(&queue->_lock);
	
	return rc;	
}

_Bool queue_pop_waitfor(queue_t* queue, long millisec, void* ptr, int* plen) {
	_Bool rc = false;

	if (!queue || !ptr || !plen)
		return false;

	pthread_mutex_lock(&queue->_lock);
	if (queue->begin >= queue->end) {
		struct timespec abstime;

		_set_timespec(&abstime, millisec);
		pthread_cond_timedwait(&queue->_cond_empty, &queue->_lock, &abstime);
	}

	if (queue->begin < queue->end) {
		int idx = queue->begin % queue->capacity;
		int len = queue->blocks[idx].length;

		if (*plen >= len) {
			*plen = len;
			memmove(ptr, queue->blocks[idx].values, *plen);

			++queue->begin;
			pthread_cond_signal(&queue->_cond_full);

			rc = true;
		}
	}

	pthread_mutex_unlock(&queue->_lock);

	return rc;
}

_Bool queue_empty(queue_t* queue) {
	if (!queue)
		return false;

	pthread_mutex_lock(&queue->_lock);

	queue->begin = 0;

	queue->end = queue->capacity;
	pthread_cond_broadcast(&queue->_cond_full);

	queue->end = 0;
	pthread_cond_broadcast(&queue->_cond_empty);

	pthread_mutex_unlock(&queue->_lock);

	return true;
}

priority_queue_t* priority_queue_new(int capacity) {
	int i;
	priority_queue_t* queue;

	queue = malloc(sizeof(priority_queue_t));
	if (!queue)
		return 0;

	if (capacity < (1 << (PRIORITY_QUEUE_SIZE - 1))) {
		capacity = (1 << (PRIORITY_QUEUE_SIZE - 1));
	} else if (capacity > MAX_QUEUE_CAPACITY) {
		capacity = MAX_QUEUE_CAPACITY;
	}

	pthread_mutex_init(&queue->_lock, 0);
	pthread_cond_init(&queue->_cond_empty, 0);\

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue->prior_queue[i] = queue_new(capacity >> (PRIORITY_QUEUE_SIZE - 1 - i));
	}

	return queue;
}

void priority_queue_free(priority_queue_t* queue) {
	int i;

	if (!queue)
		return;

	pthread_mutex_destroy(&queue->_lock);
	pthread_cond_destroy(&queue->_cond_empty);

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue_free(queue->prior_queue[i]);
	}
}

static int _get_priority_idx(priority_e priority) {
	if (priority < HIGHEST) {
		return HIGHEST;
	} else if (priority > NORMAL) {
		return NORMAL;
	} else {
		return priority;
	}
}

static _Bool _priority_queue_is_empty(priority_queue_t* queue) {
	_Bool rc = true;
	int i;

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue_t* p = queue->prior_queue[i];
		if (p->end != p->begin) {
			rc = false;
			break;
		}
	}

	return rc;
}

_Bool priority_queue_push(priority_queue_t* queue, priority_e priority, void* ptr, int len) {
	_Bool rc;

	if (!queue)
		return false;

	rc = queue_push(queue->prior_queue[_get_priority_idx(priority)], ptr, len);
	if (rc) {
		pthread_cond_signal(&queue->_cond_empty);
	}

	return rc;
}

_Bool priority_queue_pop(priority_queue_t* queue, void* ptr, int* plen) {
	_Bool rc = false;
	int i;

	if (!queue)
		return false;

	pthread_mutex_lock(&queue->_lock);
	if (_priority_queue_is_empty(queue)) {
		pthread_cond_wait(&queue->_cond_empty, &queue->_lock);
	}

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue_t* p = queue->prior_queue[i];
		if (p->begin < p->end) {
			rc = queue_pop(queue->prior_queue[i], ptr, plen);
			break;
		}
	}

	pthread_mutex_unlock(&queue->_lock);

	return rc;
}

_Bool priority_queue_pop_waitfor(priority_queue_t* queue, long millisec, void* ptr, int* plen) {
	_Bool rc = false;
	int i;

	if (!queue || !ptr)
		return false;

	pthread_mutex_lock(&queue->_lock);
	if (_priority_queue_is_empty(queue)) {
		struct timespec abstime;

		_set_timespec(&abstime, millisec);
		pthread_cond_timedwait(&queue->_cond_empty, &queue->_lock, &abstime);
	}

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue_t* p = queue->prior_queue[i];
		if (p->begin < p->end) {
			rc = queue_pop(queue->prior_queue[i], ptr, plen);
			break;
		}
	}

	pthread_mutex_unlock(&queue->_lock);

	return rc;
}

_Bool priority_queue_empty(priority_queue_t* queue) {
	int i;

	pthread_mutex_lock(&queue->_lock);

	for (i = 0; i < PRIORITY_QUEUE_SIZE; ++i) {
		queue_empty(queue->prior_queue[i]);
	}
	pthread_cond_broadcast(&queue->_cond_empty);

	pthread_mutex_unlock(&queue->_lock);

	return true;
}
