//
//  z_semaphore.c
//  SystemCall
//
//  Created by NULL
//

#include "z_semaphore.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_sem_close(z_sem_t *sem)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_close), sem));
}

z_sem_t *z_sem_open(z_int8 *name, z_int32 oflag, z_int32 mode, z_uint32 value)
{
    return (z_sem_t *)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_open), name, oflag, mode, value));
}

z_int32 z_sem_post(z_sem_t *sem)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_post), sem));
}

z_int32 z_sem_trywait(z_sem_t *sem)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_trywait), sem));
}

z_int32 z_sem_unlink(z_int8 *name)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_unlink), name));
}

z_int32 z_sem_wait(z_sem_t *sem)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sem_wait), sem));
}

