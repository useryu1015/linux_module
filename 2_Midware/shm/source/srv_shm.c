#include "srv_shm.h"
#include "realtime.h"
#include "sys_error.h"
#include "sevenbits.h"
// #include "shm_realtime.h"

shm_srv_t *g_sink_shm;
static pthread_mutex_t _shm_lock = PTHREAD_MUTEX_INITIALIZER;

static inline void _SHM_LOCK(void)
{
    pthread_mutex_lock(&_shm_lock);
}

static inline void _SHM_UNLOCK(void)
{
    pthread_mutex_unlock(&_shm_lock);
}

static int _shm_open_init(shm_handle_t *phandle, char *f_name, int id, uint32_t size, bool create)
{
    char cmd[256];

    zlog_info("phandle:%p  id = %d", phandle, id);

    SYS_PARA_CHECK(phandle && f_name && id && size);

    sprintf(phandle->f_key, "%s%s%s", SHM_FILE_PATH, f_name, SHM_KEY_TAIL);
    sprintf(phandle->f_lock, "%s%s%s", SHM_FILE_PATH, f_name, SHM_LOCK_TAIL);
    if (create && (0 != access(phandle->f_key, F_OK))) // 文件不存在
    {
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "touch %s", phandle->f_key);
        system(cmd);
    }
    if (create && (0 != access(phandle->f_lock, F_OK))) // 文件不存在
    {
        memset(cmd, 0, sizeof(cmd));
        sprintf(cmd, "touch %s", phandle->f_lock);
        system(cmd);
    }

    return shmem_open(phandle, size, id, create);
}

static int _shm_creat_snapshot(bool create)
{
    return _shm_open_init(&g_sink_shm->shm_real, SHM_SNAPSHOT, SHM_SNAPSHOT_ID, sizeof(snapshot_t), create);
}

int shm_creat_init(bool create)
{
    int ret = SYS_ERR_NONE;
    _SHM_LOCK();
    if (g_sink_shm && g_sink_shm->shm_real.paddr)
    {
        ret = SYS_ERR_NONE;
        goto END;
    }
    if (!g_sink_shm)
    {
        g_sink_shm = calloc(1, sizeof(shm_srv_t));
    }
    if (!g_sink_shm)
    {
        ret = SYS_ERR_MEM;
        goto END;
    }
    ret = _shm_creat_snapshot(create);
    if (ret)
    {
        zlog_error("error:%d", ret);
        goto END;
    }
END:
    _SHM_UNLOCK();
    return ret;
}
