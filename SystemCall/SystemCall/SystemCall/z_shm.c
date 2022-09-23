//
//  z_shm.c
//  SystemCall
//
//  Created by NULL
//

#include "z_shm.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_shmget(z_int32 key, z_int32 size, z_int32 shmflg)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_shmget), key, size, shmflg));
}

z_void *z_shmat(z_int32 shm_id, z_void *shm_addr, z_int32 shmflg)
{
    return (z_void *)(z_syscall_unix(SYSCALL_UNIX(SYS_shmat), shm_id, shm_addr, shmflg));
}

z_int32 z_shmdt(z_void *shmaddr)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_shmdt), shmaddr));
}

z_int32 z_shmctl(z_int32 shm_id, z_int32 command, struct z_shmid_ds *buf)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_shmctl), shm_id, command, buf));
}

