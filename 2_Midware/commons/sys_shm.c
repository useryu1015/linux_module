#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include "sys_shm.h"
#include "sys_error.h"

/*打开共享内存*/
int shmem_open(shm_handle_t *phandle, uint32_t size, int id, _Bool create)
{
    key_t key = ftok(phandle->f_key, id);

    if (key < 0)
    {
        fprintf(stderr, "ftok key failed!\n");
        return SYS_ERR_FILE;
    }

    phandle->shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (phandle->shm_id < 0)
    {
        fprintf(stderr, "shmget failed!\n");
        return SYS_ERR_MEM;
    }

    phandle->size = size;
    phandle->paddr = shmat(phandle->shm_id, NULL, 0);

    /*设置进程共享的读写锁*/
    int fd = open(phandle->f_lock, O_CREAT | O_RDWR, 0777);
    ftruncate(fd, sizeof(pthread_rwlock_t));

    phandle->prwlock = mmap(NULL, sizeof(pthread_rwlock_t),
                            PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    zlog_info("open f_lock:%s fd:%d", phandle->f_lock, fd);
    if (create)
    {
        pthread_rwlockattr_t rwlock_attr;
        pthread_rwlockattr_init(&rwlock_attr);

        pthread_rwlockattr_setpshared(&rwlock_attr, PTHREAD_PROCESS_SHARED);
        pthread_rwlock_init(phandle->prwlock, &rwlock_attr);
    }

    return SYS_ERR_NONE;
}

/*关闭共享内存*/
void shmem_close(shm_handle_t *phandle)
{
    if (!phandle)
        return;

    shmctl(phandle->shm_id, IPC_RMID, NULL);

    pthread_rwlock_destroy(phandle->prwlock);
    munmap(phandle->prwlock, sizeof(pthread_rwlock_t));
}

/*向共享内存中写入数据*/
int shmem_write(shm_handle_t *phandle, uint32_t offset, uint8_t *pdata, uint32_t len)
{
    if (!phandle || !pdata || !len)
        return -1;

    if (!phandle->paddr || !phandle->prwlock)
        return -1;

    if (phandle->size < (offset + len))
        return -1;

    pthread_rwlock_wrlock(phandle->prwlock);
    memcpy(phandle->paddr + offset, pdata, len);
    pthread_rwlock_unlock(phandle->prwlock);

    return len;
}

/*从共享内存中读取数据*/
int shmem_read(shm_handle_t *phandle, uint32_t offset, uint8_t *pbuf, uint32_t len)
{
    if (!phandle || !pbuf || !len)
        return -1;

    if (!phandle->paddr || !phandle->prwlock)
        return -1;

    if (phandle->size < (offset + len))
        return -1;

    pthread_rwlock_rdlock(phandle->prwlock);
    memcpy(pbuf, phandle->paddr + offset, len);
    pthread_rwlock_unlock(phandle->prwlock);

    return len;
}
