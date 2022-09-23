//
//  z_mount.c
//  SystemCall
//
//  Created by NULL 
//

#include "z_mount.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_string.h"

z_int32 z_statfs(z_int8 *path, struct z_statfs64 *buf)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_statfs64), path, buf));
}

z_int32 z_fstatfs(z_int32 fd, struct z_statfs64 *buf)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_fstatfs64), fd, buf));
}

