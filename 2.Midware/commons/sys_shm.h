#ifndef __SYS_SHM_H__
#define __SYS_SHM_H__

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

typedef struct _shm_handle_t {
	char f_key[256];
	char f_lock[256];
	int shm_id;			//共享内存的ID
	void *paddr;		//共享内存的首地址

	uint32_t size;		//共享内存的大小

	pthread_rwlock_t *prwlock;	//共享内存的读写锁
} shm_handle_t;

#ifdef __cplusplus
extern "C" {
#endif

/*打开共享内存*/
int shmem_open(shm_handle_t *phandle, uint32_t size, int id, _Bool create);

/*关闭共享内存*/
void shmem_close(shm_handle_t *phandle);

/*向共享内存中写入数据*/
int shmem_write(shm_handle_t *phandle, uint32_t offset, uint8_t *pdata, uint32_t len);

/*从共享内存中读取数据*/
int shmem_read(shm_handle_t *phandle, uint32_t offset, uint8_t *pbuf, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif
