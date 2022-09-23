//
//  z_stat.c
//  SystemCall
//
//  Created by NULL
//

#include "z_stat.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_fstat(z_int32 fd, struct z_stat *stat)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_fstat64), fd, stat));
}

z_int32 z_mkfifo(z_int8 *name, z_uint16 mode)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_mkfifo), name, mode));
}

z_int32 z_lstat(z_int8 *path, struct z_stat *buf)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_lstat64), path, buf));
}
