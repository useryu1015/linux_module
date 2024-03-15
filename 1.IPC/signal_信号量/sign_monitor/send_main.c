#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <errno.h>

#define SEM_KEY 12341

int main() {
    // 创建或获取信号量集
    int sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        printf("semget failed with error code %d\n", errno);
        perror("Error in semget");
        exit(EXIT_FAILURE);
    }

    // 设置信号量的值为1，表示可用
    if (semctl(sem_id, 0, SETVAL, 1) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    // 发送信号量通知
    printf("Sending semaphore notification...\n");
    struct sembuf sem_buf = {0, 1, 0};
    if (semop(sem_id, &sem_buf, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    // 关闭信号量
    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    return 0;
}
