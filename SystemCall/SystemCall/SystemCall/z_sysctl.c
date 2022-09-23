//
//  z_sysctl.c
//  SystemCall
//
//  Created by NULL
//

#include "z_sysctl.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_sysctl(z_int32 *name, z_uint32 namelen, z_void *oldp, z_int32 *oldlenp, z_void *newp, z_int32 newlen)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sysctl), name, namelen, oldp, oldlenp, newp, newlen));
}

z_int32 z_sysctlbyname(z_int8 *name, z_void *oldp, z_int32 *oldlenp, z_void *newp, z_int32 newlen)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sysctlbyname), name, oldp, oldlenp, newp, newlen));
}
