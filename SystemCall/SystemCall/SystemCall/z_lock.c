//
//  z_lock.c
//  SystemCall
//
//  Created by NULL 
//

#include "z_lock.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_tsd.h"

struct _z_lock_s
{
    z_uint32 oul_value;
};

z_int32 z_ulock_wait(z_uint32 operation, z_void *addr, z_uint64 value, z_uint32 timeout)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_ulock_wait), operation, addr, value, timeout));
}

z_int32 z_ulock_wait2(z_uint32 operation, z_void *addr, z_uint64 value, z_uint64 timeout, z_uint64 value2)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_ulock_wait2), operation, addr, value, timeout, value2));
}

z_int32 z_ulock_wake(z_uint32 operation, z_void *addr, z_uint64 wake_value)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_ulock_wake), operation, addr, wake_value));
}

z_uint32 z_lock_owner_get_self(z_void)
{
    z_uint32 self;
    self = (z_uint32)z_tsd_get_direct(__TSD_MACH_THREAD_SELF);
    return self;
}

z_void z_lock_lock_slow(struct z_lock_s *lock, z_uint32 self, z_uint32 options)
{
    struct _z_lock_s *l = (struct _z_lock_s *)lock;
    z_uint32 allow_anonymous_owner = options & OS_UNFAIR_LOCK_ALLOW_ANONYMOUS_OWNER;
    options &= ~OS_UNFAIR_LOCK_ALLOW_ANONYMOUS_OWNER;
    z_uint32 current, new, waiters_mask = 0;
    while ((current = l->oul_value) != Z_NULL)
    {
_retry:
        if ((current == (self)) && (!(allow_anonymous_owner) || current != 0))
        {
            return;
        }
        new = current & ~1;
        if (current != new)
        {
            if (!__sync_bool_compare_and_swap(&(l->oul_value), current, new, &current))
            {
                continue;
            }
            current = new;
        }
        z_int32 ret = z_ulock_wait(UL_UNFAIR_LOCK | ULF_NO_ERRNO | options, l, current, 0);
        if (ret < 0)
        {
            switch (-ret)
            {
                case EINTR:
                case EFAULT:
                    continue;
                case EOWNERDEAD:
                    break;
                default:
                    break;
            }
        }
        if (ret > 0)
        {
            // If there are more waiters, unset nowaiters bit when acquiring lock
            waiters_mask = 1;
        }
    }
    new = self & ~waiters_mask;
    z_int8 r = __sync_bool_compare_and_swap(&(l->oul_value), Z_NULL, new, &current);
    if (!r)
        goto _retry;
}

z_void z_lock_lock(struct z_lock_s *lock)
{
    struct _z_lock_s *l = (struct _z_lock_s *)lock;
    z_uint32 self = z_lock_owner_get_self();
    z_int8 r = __sync_bool_compare_and_swap(&(l->oul_value), Z_NULL, self);
    if (r)
        return;
    return z_lock_lock_slow(lock, self, 0);
}

z_int8 z_lock_trylock(struct z_lock_s *lock)
{
    struct _z_lock_s *l = (struct _z_lock_s *)lock;
    z_uint32 self = z_lock_owner_get_self();
    z_int8 r = __sync_bool_compare_and_swap(&(l->oul_value), Z_NULL, self);
    return r;
}

z_void z_lock_unlock(struct z_lock_s *lock)
{
    struct _z_lock_s *l = (struct _z_lock_s *)lock;
    z_uint32 self = z_lock_owner_get_self();
    z_uint32 current = __sync_fetch_and_and(&(l->oul_value), Z_NULL);
    if (current == self)
        return;
    
    for (;;)
    {
        z_int32 ret = z_ulock_wake(UL_UNFAIR_LOCK | ULF_NO_ERRNO, l, 0);
        if (ret < 0)
        {
            switch (-ret)
            {
                case EINTR:
                    continue;
                case ENOENT:
                    break;
                default:
                    break;
            }
        }
        break;
    }
}

