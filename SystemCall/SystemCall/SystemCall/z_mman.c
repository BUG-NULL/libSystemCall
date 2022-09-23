//
//  z_mman.c
//  SystemCall
//
//  Created by NULL
//

#include "z_mman.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_mprotect(z_void *addr, z_int32 length, z_int32 prot)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_mprotect), addr, length, prot));
}

z_void *z_mmap(z_void *addr, z_int64 length, z_int32 prot, z_int32 flags, z_int32 fd, z_int64 offset)
{
    return (z_void *)(z_syscall_unix(SYSCALL_UNIX(SYS_mmap), addr, length, prot, flags, fd, offset));
}

z_int32 z_munmap(z_void *addr, z_int64 length)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_munmap), addr, length));
}
