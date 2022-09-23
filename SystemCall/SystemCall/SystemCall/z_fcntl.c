//
//  z_fcntl.c
//  SystemCall
//
//  Created by NULL
//

#include "z_fcntl.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_open(z_int8 *pathname, z_int32 flags, z_int32 mode)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_open), pathname, flags, mode));
}

z_int32 z_fcntl(z_int32 fd, z_int32 cmd, z_int64 arg)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_fcntl), fd, cmd, arg));
}

z_int32 z_openat(z_int32 fd, z_int8 *path, z_int32 oflag)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_openat), fd, path, oflag));
}
