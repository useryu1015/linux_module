#ifndef _SRV_SHM_H_
#define _SRV_SHM_H_

#include <stdbool.h>
#include <stdint.h>
#include <sys/queue.h>

#include "sys_shm.h"

#define SHM_FILE_PATH "/xlian/secret/ipc/"
#define SHM_SNAPSHOT "snapshot"
#define SHM_KEY_TAIL "_key"
#define SHM_LOCK_TAIL "_lock"
#define SHM_SNAPSHOT_ID (1)

typedef struct _shm_srv
{
    shm_handle_t shm_real;
} shm_srv_t;

extern shm_srv_t *g_sink_shm;

int shm_creat_init(bool create);
int shm_update_sink();

#endif