//
//  z_shm.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_shm_h
#define z_shm_h

#include <SystemCall/z_types.h>

/* Mode bits */
#define IPC_CREAT       001000          /* Create entry if key does not exist */
#define IPC_EXCL        002000          /* Fail if key exists */
#define IPC_NOWAIT      004000          /* Error if request must wait */

/* Control commands */
#define IPC_RMID        0               /* Remove identifier */
#define IPC_SET         1               /* Set options */
#define IPC_STAT        2               /* Get options */

struct z_ipc_perm
{
    z_uint32    uid;            /* [XSI] Owner's user ID */
    z_uint32    gid;            /* [XSI] Owner's group ID */
    z_uint32    cuid;           /* [XSI] Creator's user ID */
    z_uint32    cgid;           /* [XSI] Creator's group ID */
    z_uint16    mode;           /* [XSI] Read/write permission */
    z_uint16    _seq;           /* Reserved for internal use */
    z_int32     _key;           /* Reserved for internal use */
};

struct z_shmid_ds
{
    struct z_ipc_perm   shm_perm;       /* [XSI] Operation permission value */
    z_int32             shm_segsz;      /* [XSI] Size of segment in bytes */
    z_int32             shm_lpid;       /* [XSI] PID of last shared memory op */
    z_int32             shm_cpid;       /* [XSI] PID of creator */
    z_uint16            shm_nattch;     /* [XSI] Number of current attaches */
    z_int64             shm_atime;      /* [XSI] Time of last shmat() */
    z_int64             shm_dtime;      /* [XSI] Time of last shmdt() */
    z_int64             shm_ctime;      /* [XSI] Time of last shmctl() change */
    z_void              *shm_internal;  /* reserved for kernel use */
};

z_int32 z_shmget(z_int32 key, z_int32 size, z_int32 shmflg);
z_void *z_shmat(z_int32 shm_id, z_void *shm_addr, z_int32 shmflg);
z_int32 z_shmdt(z_void *shmaddr);
z_int32 z_shmctl(z_int32 shm_id, z_int32 command, struct z_shmid_ds *buf);

#endif /* z_shm_h */
