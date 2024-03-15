#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#define SEM_KEY 12341
#define TIMEOUT_SEC 2

int main() {
    // 获取信号量集
    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    // 设置超时时间
    struct timespec timeout;
    timeout.tv_sec = time(NULL) + TIMEOUT_SEC;
    timeout.tv_nsec = 0;

    // 等待信号量通知或超时
    printf("Waiting for semaphore notification...\n");
    struct sembuf sem_buf = {0, -1, 0};
    if (semtimedop(sem_id, &sem_buf, 1, &timeout) == -1) {
        if (errno == EAGAIN) {
            printf("Timeout reached (2 seconds).\n");
        } else {
            perror("semtimedop");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Received semaphore notification.\n");
    }

    return 0;
}
