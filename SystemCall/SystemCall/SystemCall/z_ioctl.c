//
//  z_ioctl.c
//  SystemCall
//
//  Created by NULL
//

#include "z_ioctl.h"
#include "z_syscall.h"
#include "z_error.h"
#include <stdarg.h>

z_int32 _z_ioctl(z_int32 fildes, z_uint64 request, z_void *arg)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_ioctl), fildes, request, arg));
}

z_int32 z_ioctl(z_int32 fildes, z_uint64 request, ...)
{
    va_list ap;
    z_void *arg;

    va_start(ap, request);
    arg = va_arg(ap, z_void *);
    va_end(ap);
    return _z_ioctl(fildes, request, arg);
}

