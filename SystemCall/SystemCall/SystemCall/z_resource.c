//
//  z_resource.c
//  SystemCall
//
//  Created by NULL
//

#include "z_resource.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_getrlimit(z_int32 resource, struct z_rlimit *rlp)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getrlimit), resource, rlp));
}

z_int32 z_setrlimit(z_int32 resource, struct z_rlimit *rlp)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_setrlimit), resource, rlp));
}


