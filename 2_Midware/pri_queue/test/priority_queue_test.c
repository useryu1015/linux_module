#include <stdio.h>
#include <unistd.h>
#include "queue.h"

static void* thread_push_a(void* args) {
	priority_queue_t* queue = (priority_queue_t*) args;
	int a = 0;
	char str[32];

	while (1) {
		int i;
		for (i = 0; i < 20; ++i) {
			int len = sprintf(str, "thread_push_a: %d", ++a);
			priority_queue_push(queue, NORMAL, str, len);
			puts(str);
			usleep(10 * 1000);
		}
		sleep(5);
	}

	return 0;
}

static void* thread_push_b(void* args) {
	priority_queue_t* queue = (priority_queue_t*) args;
	int b = 0;
	char str[32];

	while (1) {
		int len = sprintf(str, "thread_push_b: %d", ++b);
		priority_queue_push(queue, HIGHEST, str, len);
		puts(str);
		usleep(400 * 1000);
	}

	return 0;
}

static void* thread_pop(void* args) {
	priority_queue_t* queue = (priority_queue_t*) args;
	int len;
	char buf[1024];

	while (1) {
		priority_queue_pop(queue, buf, &len);
		buf[len] = '\0';
		printf("thread_pop: %s\n", buf);
		usleep(100 * 1000);
	}

	return 0;
}

int priority_queue_main(int argc, const char* argv[]) {
	void* rc;
	pthread_t pthread;
	priority_queue_t* queue = priority_queue_new(4);

	pthread_create(&pthread, 0, thread_push_a, queue);
	pthread_create(&pthread, 0, thread_push_b, queue);
	pthread_create(&pthread, 0, thread_pop, queue);

	pthread_join(pthread, &rc);

	return 0;
}
