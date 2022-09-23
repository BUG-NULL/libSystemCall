//
//  z_time.c
//  SystemCall
//
//  Created by NULL
//

#include "z_time.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_gettimeofday(struct z_timeval *tv, struct z_timezone *tz)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_gettimeofday), tv, tz));
}

z_int32 z_getitimer(z_int32 which, struct z_itimerval *value)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getitimer), which, value));
}

z_int32 z_setitimer(z_int32 which, struct z_itimerval *value, struct z_itimerval *ovalue)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_setitimer), which, value, ovalue));
}

