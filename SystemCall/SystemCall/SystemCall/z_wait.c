//
//  z_wait.c
//  SystemCall
//
//  Created by NULL
//

#include "z_wait.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_wait4(z_int32 pid, z_int32 *status, z_int32 options, struct z_rusage *rusage)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_wait4), pid, status, options, rusage));
}

