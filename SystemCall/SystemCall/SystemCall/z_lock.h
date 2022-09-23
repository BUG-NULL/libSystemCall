//
//  z_lock.h
//  SystemCall
//
//  Created by NULL 
//

#ifndef z_lock_h
#define z_lock_h

#include <SystemCall/z_types.h>

#define OS_UNFAIR_LOCK_ALLOW_ANONYMOUS_OWNER    0x01000000u

#define UL_COMPARE_AND_WAIT             1
#define UL_UNFAIR_LOCK                  2
#define UL_COMPARE_AND_WAIT_SHARED      3
#define UL_UNFAIR_LOCK64_SHARED         4
#define UL_COMPARE_AND_WAIT64           5
#define UL_COMPARE_AND_WAIT64_SHARED    6
/* obsolete names */
#define UL_OSSPINLOCK                   UL_COMPARE_AND_WAIT
#define UL_HANDOFFLOCK                  UL_UNFAIR_LOCK
/* These operation code are only implemented in (DEVELOPMENT || DEBUG) kernels */
#define UL_DEBUG_SIMULATE_COPYIN_FAULT  253
#define UL_DEBUG_HASH_DUMP_ALL          254
#define UL_DEBUG_HASH_DUMP_PID          255

#define ULF_WAKE_ALL                    0x00000100
#define ULF_WAKE_THREAD                 0x00000200
#define ULF_WAKE_ALLOW_NON_OWNER        0x00000400

#define ULF_WAIT_WORKQ_DATA_CONTENTION  0x00010000
#define ULF_WAIT_CANCEL_POINT           0x00020000
#define ULF_WAIT_ADAPTIVE_SPIN          0x00040000

#define ULF_NO_ERRNO                    0x01000000

struct z_lock_s
{
    z_uint32 z_lock_opaque;
};

z_uint32 z_lock_owner_get_self(z_void);
z_void z_lock_lock(struct z_lock_s *lock);
z_int8 z_lock_trylock(struct z_lock_s *lock);
z_void z_lock_unlock(struct z_lock_s *lock);
z_int32 z_ulock_wait(z_uint32 operation, z_void *addr, z_uint64 value, z_uint32 timeout);
z_int32 z_ulock_wait2(z_uint32 operation, z_void *addr, z_uint64 value, z_uint64 timeout, z_uint64 value2);
z_int32 z_ulock_wake(z_uint32 operation, z_void *addr, z_uint64 wake_value);
    
#endif /* z_lock_h */
