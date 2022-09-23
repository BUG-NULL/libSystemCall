//
//  z_ptrace.c
//  SystemCall
//
//  Created by NULL
//

#include "z_ptrace.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_ptrace(z_int32 request, z_int32 pid, z_void *addr, z_int64 data)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_ptrace), request, pid, addr, data));
}

