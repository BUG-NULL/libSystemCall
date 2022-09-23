//
//  z_select.c
//  SystemCall
//
//  Created by NULL
//

#include "z_select.h"
#include "z_syscall.h"
#include "z_error.h"

#define z_NFDBITS        (sizeof(z_int32) * 8) /* bits per mask */

z_int32 z_check_fd_set(z_int32 a, z_void *b)
{
    return 1;
}

z_void z_fd_set(z_int32 fd, struct z_fd_set *p)
{
    if (z_check_fd_set(fd, (z_void *)p))
    {
        (p->fds_bits[(z_uint64)fd / z_NFDBITS] |= ((z_int32)(((z_uint64)1) << ((z_uint64)fd % z_NFDBITS))));
    }
}

z_int32 z_fd_isset(z_int32 _fd, struct z_fd_set *_p)
{
    return _p->fds_bits[(z_uint64)_fd / z_NFDBITS] & ((z_int32)(((z_uint64)1) << ((z_uint64)_fd % z_NFDBITS)));
}

z_int32 z_select(z_int32 maxfdp, struct z_fd_set *readset, struct z_fd_set *writeset, struct z_fd_set *exceptset, struct z_timeval *timeout)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_select_nocancel), maxfdp, readset, writeset, exceptset, timeout));
}

