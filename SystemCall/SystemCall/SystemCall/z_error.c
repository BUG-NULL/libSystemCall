//
//  z_error.c
//  SystemCall
//
//  Created by NULL
//

#include "z_error.h"
#include "z_tsd.h"
#include "z_pthread.h"

#define READ_RFLAGS_LOW16() \
({ \
    z_uint16 dst;\
    asm("pushfw; popw %%ax":"=a"(dst):); \
    dst;\
})

z_int64 __errno;

z_int64 check_error(z_int64 rc)
{
    z_int64 ret = rc;
    
    z_uint16 rflags = READ_RFLAGS_LOW16();
    if ((rflags & 0x01) == 1)
    {
        z_errno = ret;
        ret = -1;
    }

    return ret;
}

z_int64 *z_perrno(z_void)
{
    z_void *ptr = (z_void *)z_tsd_get_direct(__TSD_ERRNO);
    if (ptr != Z_NULL)
    {
        return (z_int64 *)ptr;
    }
    return &__errno;
}

z_uint64 z_cerror_nocancel(z_int32 err)
{
    __errno = err;
    z_int32 *tsderrno = (z_int32 *)z_tsd_get_direct(__TSD_ERRNO);
    if (tsderrno)
    {
        *tsderrno = err;
    }
    return -1;
}

z_uint64 z_cerror(z_int32 err)
{
    z_pthread_exit_if_canceled(err);
    return z_cerror_nocancel(err);
}
