//
//  z_pthread.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_pthread_h
#define z_pthread_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_lock.h>
#include <SystemCall/z_tsd.h>
#include <SystemCall/z_machO.h>

/*
 * POSIX scheduling policies
 */
#define SCHED_OTHER                 1
#define SCHED_FIFO                  4
#define SCHED_RR                    2

#define _PTHREAD_TSD_SLOT_PTHREAD_SELF              __TSD_THREAD_SELF
#define _PTHREAD_TSD_SLOT_ERRNO                     __TSD_ERRNO
#define _PTHREAD_TSD_SLOT_MIG_REPLY                 __TSD_MIG_REPLY
#define _PTHREAD_TSD_SLOT_MACH_THREAD_SELF          __TSD_MACH_THREAD_SELF
#define _PTHREAD_TSD_SLOT_PTHREAD_QOS_CLASS         __TSD_THREAD_QOS_CLASS
#define _PTHREAD_TSD_SLOT_RETURN_TO_KERNEL          __TSD_RETURN_TO_KERNEL
#define _PTHREAD_TSD_SLOT_PTR_MUNGE                 __TSD_PTR_MUNGE
#define _PTHREAD_TSD_SLOT_MACH_SPECIAL_REPLY        __TSD_MACH_SPECIAL_REPLY
#define _PTHREAD_TSD_SLOT_SEMAPHORE_CACHE           __TSD_SEMAPHORE_CACHE

#define _PTHREAD_TSD_SLOT_PTHREAD_SELF_TYPE         struct z_pthread_t *
#define _PTHREAD_TSD_SLOT_ERRNO_TYPE                z_int32 *
#define _PTHREAD_TSD_SLOT_MIG_REPLY_TYPE            z_uint32
#define _PTHREAD_TSD_SLOT_MACH_THREAD_SELF_TYPE     z_uint32
#define _PTHREAD_TSD_SLOT_PTHREAD_QOS_CLASS_TYPE    z_uint64
#define _PTHREAD_TSD_SLOT_RETURN_TO_KERNEL_TYPE     z_uint64
#define _PTHREAD_TSD_SLOT_PTR_MUNGE_TYPE            z_uint64
#define _PTHREAD_TSD_SLOT_MACH_SPECIAL_REPLY_TYPE   z_uint32
#define _PTHREAD_TSD_SLOT_SEMAPHORE_CACHE_TYPE      z_uint32

#define z_pthread_tsd_slot(th, name) \
    (*(_PTHREAD_TSD_SLOT_##name##_TYPE *)(z_uint64 *)&(th)->tsd[_PTHREAD_TSD_SLOT_##name])

#define PTHREAD_CREATE_JOINABLE      1
#define PTHREAD_CREATE_DETACHED      2

#define PTHREAD_INHERIT_SCHED        1
#define PTHREAD_EXPLICIT_SCHED       2

#define PTHREAD_CANCEL_ENABLE        0x01  /* Cancel takes place at next cancellation point */
#define PTHREAD_CANCEL_DISABLE       0x00  /* Cancel postponed */
#define PTHREAD_CANCEL_DEFERRED      0x02  /* Cancel waits until cancellation point */
#define PTHREAD_CANCEL_ASYNCHRONOUS  0x00  /* Cancel occurs immediately */

#define vm_round_page(x)    (((z_uint64)(x) + z_page_mask) & ~((signed)z_page_mask))
#define PTHREAD_SIZE        ((z_uint32)vm_round_page(sizeof(struct z_pthread_t)))
#define DEFAULT_STACK_SIZE  (z_uint64)(512 * 1024)

#define _PTHREAD_CREATE_NONE                0x0
#define _PTHREAD_CREATE_FROM_MACH_THREAD    0x1
#define _PTHREAD_CREATE_SUSPENDED           0x2

#define _EXTERNAL_POSIX_THREAD_KEYS_MAX     512
#define _INTERNAL_POSIX_THREAD_KEYS_MAX     256
#define _INTERNAL_POSIX_THREAD_KEYS_END     768

/*
 * Mutex protocol attributes
 */
#define PTHREAD_PRIO_NONE           0
#define PTHREAD_PRIO_INHERIT        1
#define PTHREAD_PRIO_PROTECT        2

/*
 * Flags filed passed to bsdthread_create and back in pthread_start
 * 31  <---------------------------------> 0
 * _________________________________________
 * | flags(8) | policy(8) | importance(16) |
 * -----------------------------------------
 */
#define PTHREAD_START_CUSTOM            0x01000000 // <rdar://problem/34501401>
#define PTHREAD_START_SETSCHED          0x02000000
// was PTHREAD_START_DETACHED           0x04000000
#define PTHREAD_START_QOSCLASS          0x08000000
#define PTHREAD_START_TSD_BASE_SET      0x10000000
#define PTHREAD_START_SUSPENDED         0x20000000
#define PTHREAD_START_QOSCLASS_MASK     0x00ffffff
#define PTHREAD_START_POLICY_BITSHIFT   16
#define PTHREAD_START_POLICY_MASK       0xff
#define PTHREAD_START_IMPORTANCE_MASK   0xffff

#define _PTHREAD_NO_SIG                         0x00000000
#define _PTHREAD_MUTEX_ATTR_SIG                 0x4D545841  /* 'MTXA' */
#define _PTHREAD_MUTEX_SIG                      0x4D555458  /* 'MUTX' */
#define _PTHREAD_MUTEX_SIG_fast                 0x4D55545A  /* 'MUTZ' */
#define _PTHREAD_MUTEX_SIG_MASK                 0xfffffffd
#define _PTHREAD_MUTEX_SIG_CMP                  0x4D555458  /* _PTHREAD_MUTEX_SIG & _PTHREAD_MUTEX_SIG_MASK */
#define _PTHREAD_MUTEX_SIG_init                 0x32AAABA7  /* [almost] ~'MUTX' */
#define _PTHREAD_ERRORCHECK_MUTEX_SIG_init      0x32AAABA1
#define _PTHREAD_RECURSIVE_MUTEX_SIG_init       0x32AAABA2
#define _PTHREAD_FIRSTFIT_MUTEX_SIG_init        0x32AAABA3
#define _PTHREAD_MUTEX_SIG_init_MASK            0xfffffff0
#define _PTHREAD_MUTEX_SIG_init_CMP             0x32AAABA0
#define _PTHREAD_COND_ATTR_SIG                  0x434E4441  /* 'CNDA' */
#define _PTHREAD_COND_SIG                       0x434F4E44  /* 'COND' */
#define _PTHREAD_COND_SIG_init                  0x3CB0B1BB  /* [almost] ~'COND' */
#define _PTHREAD_COND_SIG_pristine              0x434F4E44  /* 'COND' */
#define _PTHREAD_COND_SIG_psynch                0x434F4E45  /* 'COND' + 0b01: 'CONE' */
#define _PTHREAD_COND_SIG_ulock                 0x434F4E46  /* 'COND' + 0b10: 'CONF' */
#define _PTHREAD_ATTR_SIG                       0x54484441  /* 'THDA' */
#define _PTHREAD_ONCE_SIG                       0x4F4E4345  /* 'ONCE' */
#define _PTHREAD_ONCE_SIG_init                  0x30B1BCBA  /* [almost] ~'ONCE' */
#define _PTHREAD_SIG                            0x54485244  /* 'THRD' */

#define _PTHREAD_DEFAULT_INHERITSCHED   PTHREAD_INHERIT_SCHED
#define _PTHREAD_DEFAULT_PROTOCOL       PTHREAD_PRIO_NONE
#define _PTHREAD_DEFAULT_PRIOCEILING    0
#define _PTHREAD_DEFAULT_POLICY         SCHED_OTHER
#define _PTHREAD_DEFAULT_STACKSIZE      0x80000    /* 512K */
#define _PTHREAD_DEFAULT_PSHARED        PTHREAD_PROCESS_PRIVATE

#define _PTHREAD_CANCEL_STATE_MASK      0x01
#define _PTHREAD_CANCEL_TYPE_MASK       0x02
#define _PTHREAD_CANCEL_PENDING         0x10  /* pthread_cancel() has been called for this thread */
#define _PTHREAD_CANCEL_EXITING         0x20

/*
 * THREAD_QOS_POLICY:
 */
#define THREAD_QOS_POLICY               9
#define THREAD_QOS_POLICY_OVERRIDE      10

#define THREAD_QOS_UNSPECIFIED          0
#define THREAD_QOS_DEFAULT              THREAD_QOS_UNSPECIFIED  /* Temporary rename */
#define THREAD_QOS_MAINTENANCE          1
#define THREAD_QOS_BACKGROUND           2
#define THREAD_QOS_UTILITY              3
#define THREAD_QOS_LEGACY               4       /* i.e. default workq threads */
#define THREAD_QOS_USER_INITIATED       5
#define THREAD_QOS_USER_INTERACTIVE     6
#define THREAD_QOS_LAST                 7
#define THREAD_QOS_MIN_TIER_IMPORTANCE  (-15)

#define _PTHREAD_PRIORITY_QOS_CLASS_MASK                0x003fff00
#define _PTHREAD_PRIORITY_VALID_QOS_CLASS_MASK          0x00003f00
#define _PTHREAD_PRIORITY_QOS_CLASS_SHIFT               (8ull)
#define _PTHREAD_PRIORITY_PRIORITY_MASK                 0x000000ff
#define _PTHREAD_PRIORITY_PRIORITY_SHIFT                (0)

#define _PTHREAD_REG_DEFAULT_POLICY_MASK                0xff
#define _PTHREAD_REG_DEFAULT_USE_ULOCK                  0x100
#define _PTHREAD_REG_DEFAULT_USE_ADAPTIVE_SPIN          0x200

/* Keys 20-29 for libdispatch usage */
#define __PTK_LIBDISPATCH_KEY0        20
#define __PTK_LIBDISPATCH_KEY1        21
#define __PTK_LIBDISPATCH_KEY2        22
#define __PTK_LIBDISPATCH_KEY3        23
#define __PTK_LIBDISPATCH_KEY4        24
#define __PTK_LIBDISPATCH_KEY5        25
#define __PTK_LIBDISPATCH_KEY6        26
#define __PTK_LIBDISPATCH_KEY7        27
#define __PTK_LIBDISPATCH_KEY8        28
#define __PTK_LIBDISPATCH_KEY9        29

/* pthread userspace SPI feature checking, these constants are returned from bsdthread_register,
 * as a bitmask, to inform userspace of the supported feature set. Old releases of OS X return
 * from this call either zero or -1, allowing us to return a positive number for feature bits.
 */
#define PTHREAD_FEATURE_DISPATCHFUNC    0x01        /* same as WQOPS_QUEUE_NEWSPISUPP, checks for dispatch function support */
#define PTHREAD_FEATURE_FINEPRIO        0x02        /* are fine grained prioirities available */
#define PTHREAD_FEATURE_BSDTHREADCTL    0x04        /* is the bsdthread_ctl syscall available */
#define PTHREAD_FEATURE_SETSELF         0x08        /* is the BSDTHREAD_CTL_SET_SELF command of bsdthread_ctl available */
#define PTHREAD_FEATURE_QOS_MAINTENANCE 0x10        /* is QOS_CLASS_MAINTENANCE available */
#define PTHREAD_FEATURE_RESERVED        0x20        /* burnt, shipped in OSX 10.11 & iOS 9 with partial kevent delivery support */
#define PTHREAD_FEATURE_KEVENT          0x40        /* supports direct kevent delivery */
#define PTHREAD_FEATURE_WORKLOOP        0x80        /* supports workloops */
#define PTHREAD_FEATURE_QOS_DEFAULT     0x40000000  /* the kernel supports QOS_CLASS_DEFAULT */

/*
 * Mutex type attributes
 */
#define PTHREAD_MUTEX_NORMAL            0
#define PTHREAD_MUTEX_ERRORCHECK        1
#define PTHREAD_MUTEX_RECURSIVE         2
#define PTHREAD_MUTEX_DEFAULT           PTHREAD_MUTEX_NORMAL

#define _PTHREAD_MTX_OPT_POLICY_FAIRSHARE   1
#define _PTHREAD_MTX_OPT_POLICY_FIRSTFIT    2
#define _PTHREAD_MTX_OPT_POLICY_DEFAULT     _PTHREAD_MTX_OPT_POLICY_FIRSTFIT

/* Value returned from pthread_join() when a thread is canceled */
#define PTHREAD_CANCELED            ((void *) 1)

/* We only support PTHREAD_SCOPE_SYSTEM */
#define PTHREAD_SCOPE_SYSTEM        1
#define PTHREAD_SCOPE_PROCESS       2

#define PTHREAD_PROCESS_SHARED      1
#define PTHREAD_PROCESS_PRIVATE     2

// L word
#define PTH_RWL_KBIT        0x01    // cannot acquire in user mode
#define PTH_RWL_EBIT        0x02    // exclusive lock in progress
#define PTH_RWL_WBIT        0x04    // write waiters pending in kernel
#define PTH_RWL_PBIT        0x04    // prepost (cv) pending in kernel

#define PTH_RWL_MTX_WAIT    0x20    // in cvar in mutex wait
#define PTH_RWL_UBIT        0x40    // lock is unlocked (no readers or writers)
#define PTH_RWL_MBIT        0x40    // overlapping grants from kernel (only in updateval)
#define PTH_RWL_IBIT        0x80    // lock reset, held until first successful unlock

#define PTHRW_RWL_INIT      PTH_RWL_IBIT    // reset on the lock bits (U)
#define PTHRW_RWLOCK_INIT   (PTH_RWL_IBIT | PTH_RWL_UBIT)   // reset on the lock bits (U)

// S word
#define PTH_RWS_SBIT        0x01    // kernel transition seq not set yet
#define PTH_RWS_IBIT        0x02    // Sequence is not set on return from kernel

#define PTH_RWS_CV_CBIT     PTH_RWS_SBIT    // kernel has cleared all info w.r.s.t CV
#define PTH_RWS_CV_PBIT     PTH_RWS_IBIT    // kernel has prepost/fake structs only,no waiters
#define PTH_RWS_CV_BITSALL  (PTH_RWS_CV_CBIT | PTH_RWS_CV_PBIT)
#define PTH_RWS_CV_MBIT     PTH_RWL_MBIT    // to indicate prepost return from kernel
#define PTH_RWS_CV_RESET_PBIT   ((z_uint32)~PTH_RWS_CV_PBIT)

#define PTH_RWS_WSVBIT        0x04    // save W bit

#define PTHRW_RWS_SAVEMASK    (PTH_RWS_WSVBIT)    // save bits mask

#define PTHRW_RWS_INIT        PTH_RWS_SBIT    // reset on the lock bits (U)

// rw_flags
#define PTHRW_KERN_PROCESS_SHARED       0x10
#define PTHRW_KERN_PROCESS_PRIVATE      0x20

#define PTHREAD_MTX_TID_SWITCHING (z_uint64)-1

// L word tests
#define is_rwl_ebit_set(x) (((x) & PTH_RWL_EBIT) != 0)
#define is_rwl_wbit_set(x) (((x) & PTH_RWL_WBIT) != 0)
#define is_rwl_ebit_clear(x) (((x) & PTH_RWL_EBIT) == 0)
#define is_rwl_readoverlap(x) (((x) & PTH_RWL_MBIT) != 0)

// S word tests
#define is_rws_sbit_set(x) (((x) & PTH_RWS_SBIT) != 0)
#define is_rws_unlockinit_set(x) (((x) & PTH_RWS_IBIT) != 0)
#define is_rws_savemask_set(x) (((x) & PTHRW_RWS_SAVEMASK) != 0)
#define is_rws_pbit_set(x) (((x) & PTH_RWS_CV_PBIT) != 0)

// kwe_flags
#define KWE_FLAG_LOCKPREPOST    0x1 // cvwait caused a lock prepost

#define PTHRW_COUNT_SHIFT       8
#define PTHRW_INC               (1 << PTHRW_COUNT_SHIFT)
#define PTHRW_BIT_MASK          ((1 << PTHRW_COUNT_SHIFT) - 1)
#define PTHRW_COUNT_MASK        ((z_uint32)~PTHRW_BIT_MASK)
#define PTHRW_MAX_READERS       PTHRW_COUNT_MASK

#define ECVCLEARED              0x100
#define ECVPREPOST              0x200

#define PTHREAD_CONFORM_DARWIN_LEGACY       0
#define PTHREAD_CONFORM_UNIX03_NOCANCEL     1
#define PTHREAD_CONFORM_UNIX03_CANCELABLE   2

#define PTHREAD_ATFORK_INLINE_MAX           10

struct z_sched_param
{
    z_int32 sched_priority;
    z_int32 quantum;
};

struct z_pthread_handler_rec
{
    z_void (*__routine)(z_void *);              // Routine to call
    z_void *__arg;                            // Argument to pass
    struct z_pthread_handler_rec *__next;
};

struct z_pthread_plist
{                                                                \
    struct z_pthread_t *tqe_next;  /* next element */
    struct z_pthread_t **tqe_prev; /* address of previous next element */
};

struct z_pthread_join_context_s
{
    struct z_pthread_t  *waiter;
    z_void              **value_ptr;
    z_uint32            kport;
    z_uint32            custom_stack_sema;
    z_int8              detached;
};

struct z_pthread_t
{
    z_int64                         sig;
    struct z_pthread_handler_rec    *__cleanup_stack;
    struct z_pthread_plist          tl_plist;              // global thread list [aligned]
    struct z_pthread_join_context_s *tl_join_ctx;
    z_void                          *tl_exit_value;
    z_uint32                        tl_policy               :8;
    z_uint32                        tl_joinable             :1;
    z_uint32                        tl_joiner_cleans_up     :1;
    z_uint32                        tl_has_custom_stack     :1;
    z_uint32                        __tl_pad                :21;
    // MACH_PORT_NULL if no joiner
    // tsd[_PTHREAD_TSD_SLOT_MACH_THREAD_SELF] when has a joiner
    // MACH_PORT_DEAD if the thread exited
    z_uint32                        tl_exit_gate;
    struct z_sched_param            tl_param;
    z_void                          *__unused_padding;
    struct z_lock_s                 lock;
    z_uint16                        max_tsd_key;
    z_uint16                        inherit                 :8;
    z_uint16                        kernalloc               :1;
    z_uint16                        schedset                :1;
    z_uint16                        wqthread                :1;
    z_uint16                        wqkillset               :1;
    z_uint16                        __flags_pad             :4;
    z_int8                          pthread_name[64];   // includes NUL [aligned]
    z_void                          *(*fun)(z_void *);  // thread start routine
    z_void                          *arg;           // thread start routine argument
    z_int32                         wq_nevents;    // wqthreads (workloop / kevent)
    z_uint8                         wq_outsideqos;
    z_uint8                         canceled;      // 4597450 set if conformant cancelation happened
    z_uint16                        cancel_state;   // whether the thread can be canceled [atomic]
    z_int32                         cancel_error;
    z_int32                         err_no;         // thread-local errno
    z_void                          *stackaddr;     // base of the stack (page aligned)
    z_void                          *stackbottom;   // stackaddr - stacksize
    z_void                          *freeaddr;      // stack/thread allocation base address
    z_uint64                        freesize;       // stack/thread allocation size
    z_uint64                        guardsize;      // guard page size in bytes
    // tsd-base relative accessed elements
    __attribute__((aligned(8)))
    z_uint64                        thread_id;      // 64-bit unique thread id
    /* Thread Specific Data slots
     *
     * The offset of this field from the start of the structure is difficult to
     * change on OS X because of a thorny bitcompat issue: mono has hard coded
     * the value into their source.  Newer versions of mono will fall back to
     * scanning to determine it at runtime, but there's lots of software built
     * with older mono that won't.  We will have to break them someday...
     */
    __attribute__ ((aligned (16)))
    z_uint64 tsd[_EXTERNAL_POSIX_THREAD_KEYS_MAX + _INTERNAL_POSIX_THREAD_KEYS_MAX];
};

struct z_pthread_attr_s
{
    z_int64     sig;
    z_uint64    guardsize;      // size in bytes of stack overflow guard area
    z_void      *stackaddr;     // stack base; vm_page_size aligned
    z_uint64    stacksize;      // stack size; multiple of vm_page_size and >= PTHREAD_STACK_MIN
    union {
        struct z_sched_param param; // [aligned]
        z_uint64 qosclass;      // pthread_priority_t
    };
    z_uint32    detached            :8;
    z_uint32    inherit             :8;
    z_uint32    policy              :8;
    z_uint32    schedset            :1;
    z_uint32    qosset              :1;
    z_uint32    policyset           :1;
    z_uint32    cpupercentset       :1;
    z_uint32    defaultguardpage    :1;
    z_uint32    unused              :3;
    z_uint32    cpupercent          :8;
    z_uint32    refillms            :24;
    z_uint32    _reserved[4];
};

struct z_pthread_registration_data
{
    /*
     * version == sizeof(struct _pthread_registration_data)
     *
     * The structure can only grow, so we use its size as the version.
     * Userspace initializes this to the size of its structure and the kext
     * will copy out the version that was actually consumed.
     *
     * n.b. you must make sure the size of this structure isn't LP64-dependent
     */
    z_uint64    version;
    z_uint64    dispatch_queue_offset;      /* copy-in */
    z_uint64    main_qos;                   /* copy-out */ /* pthread_priority_t */
    z_uint32    tsd_offset;                 /* copy-in */
    z_uint32    return_to_kernel_offset;    /* copy-in */
    z_uint32    mach_thread_self_offset;    /* copy-in */
    z_uint64    stack_addr_hint;            /* copy-out */
    z_uint32    mutex_default_policy;       /* copy-out */
    z_uint32    joinable_offset_bits;       /* copy-in */
} __attribute__ ((packed));

struct z_pthread_atfork_entry
{
    z_void (*prepare)(z_void);
    z_void (*parent)(z_void);
    z_void (*child)(z_void);
};

struct z_pthread_globals_s
{
    struct z_pthread_t *psaved_self;
    struct z_lock_s psaved_self_global_lock;
    struct z_lock_s pthread_atfork_lock;
    z_uint64 atfork_count;
    struct z_pthread_atfork_entry atfork_storage[PTHREAD_ATFORK_INLINE_MAX];
    struct z_pthread_atfork_entry *atfork;
    z_uint16 qmp_logical[THREAD_QOS_LAST];
    z_uint16 qmp_physical[THREAD_QOS_LAST];
};

z_void z_pthread_set_self(z_void);
struct z_pthread_t *z_pthread_self(z_void);
z_uint64 z_pthread_selfid_direct(z_void);
z_int32 z_pthread_selfid(z_void);
z_int32 z_pthread_main_np(z_void);
z_int32 z_pthread_init(z_int8 *envp[], z_int8 *apple[]);
z_int32 z_pthread_create(struct z_pthread_t **thread, struct z_pthread_attr_s *attr, z_void *(*start_routine)(z_void *), z_void *arg);
z_int32 z_pthread_cancel(struct z_pthread_t *thread);
z_int32 z_pthread_join(struct z_pthread_t *thread, z_void **value_ptr);
z_int32 z_pthread_detach(struct z_pthread_t *thread);
z_void z_pthread_exit(struct z_pthread_t *thread, z_void *exit_value);
z_void z_pthread_exit_if_canceled(z_int32 error);
z_int32 z_pthread_equal(struct z_pthread_t *t1, struct z_pthread_t *t2);
z_void z_pthread_yield_np(z_void);
z_int64 z_pthread_cpu_number_np(z_uint32 *cpu_id);
z_void z_pthread_atfork_prepare_handlers(z_void);
z_void z_pthread_atfork_prepare(z_void);
z_void z_pthread_atfork_parent(z_void);
z_void z_pthread_atfork_parent_handlers(z_void);
z_void z_pthread_atfork_child(z_void);
z_void z_pthread_atfork_child_handlers(z_void);

#endif /* z_pthread_h */


