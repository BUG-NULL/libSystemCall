//
//  z_pthread.c
//  SystemCall
//
//  Created by NULL
//

#include "z_pthread.h"
#include "z_pthread_tsd.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_string.h"
#include "z_unistd.h"
#include "z_tsd.h"
#include "z_mach_trap.h"
#include "z_stdio.h"
#include "z_sysctl.h"
#include "z_mman.h"
#include "z_resource.h"
#include "z_stdlib.h"
#include "z_semaphore.h"
#include "z_lock.h"
#include "z_malloc.h"

/*
*
*   4G x 4G ---------------> +---------------------------------+ 0xFFFFFFFFFFFFFFFF
*                            |                                 |
*                            |         Empty Memory (*)        |
*                            |                                 |
*                            +---------------------------------+ 0xFFFFA00000000000
*                            |                                 |
*                            :                                 :
*                            |         ~~~~~~~~~~~~~~~~        |
*                            :                                 :
*                            |                                 |
*   Main Thread Stack Top -> +---------------------------------+ 0x00007FF7BFF00000
*                            |   User stack 0x00800000 (8 M)   |
*                            |         ~~~~~~~~~~~~~~~~        |
*                            |                                 |
*                            +---------------------------------+ 0x00007FF7BF700000
*                            |            User Heap            |
*                            |         ~~~~~~~~~~~~~~~~        |
*                            |                                 |
*                            |                                 |
*   Thread 2 End ----------->+---------------------------------+ 0x0000000100132000
*                            |          new thread 2           |
*                            |                                 |
*                            |                                 |
*                            |                                 |
*   Thread 2 Stack Bottom -> +---------------------------------+ 0x0000000100130000
*                            |               |                 |
*                            |               |                 |
*                            |               |                 |
*                            |              \|/                |
*                            +---------------------------------+ 0x00000001000B0000
*                            |        guardsize 0x1000         |
*   Thread 2 Start --------->+---------------------------------+ 0x00000001000AF000
*   Thread 1 End ----------->+---------------------------------+ 0x00000001000AF000
*                            |          new thread 1           |
*                            |                                 |
*                            |                                 |
*                            |                                 |
*   Thread 1 Stack Bottom -> +---------------------------------+ 0x00000001000AD000
*                            |               |                 |
*                            |               |                 |
*                            |               |                 |
*                            |              \|/                |
*                            +---------------------------------+ 0x000000010002D000
*                            |        guardsize 0x1000         |
*   Thread 1 Start --------> +---------------------------------+ 0x000000010002C000
*   MMAP Start ------------> +---------------------------------+ 0x000000010002C000
*                            |            LinkEdit             |
*   LINKEDIT --------------> +---------------------------------+ 0x0000000100018000
*                            |              Data               |
*                            |                                 |
*                            |                                 |
*                            +---------------------------------+ 0x0000000100017000
*                            |        struct main thread       |
*                            |                                 |
*   _main_thread->tsd -----> +                                 + 0x00000001000150E0
*                            |                                 |
*   _main_thread ----------> +---------------------------------+ 0x0000000100015000
*                            |                                 |
*   DATA ------------------> +---------------------------------+ 0x0000000100013000
*                            |              Text               |
*   TEXT ------------------> +---------------------------------+ 0x0000000100000000
*                            |        Invalid Memory (*)       | --/--
*   0 ---------------------> +---------------------------------+ 0x0000000000000000
*
*/

z_void z_thread_start(struct z_pthread_t *thread, z_uint32 kport, z_void *(*fun)(z_void *), z_void *arg, z_uint64 stacksize, z_uint32 flags);
z_void z_start_wqthread(struct z_pthread_t *thread, z_uint32 kport, z_void *stackaddr, z_void *unused, z_int32 reuse);
z_sem_t z_pthread_joiner_prepost_wake(struct z_pthread_t *thread);
z_void z_pthread_joiner_wake(struct z_pthread_t *thread);

extern z_uint64 z_page_size;
extern z_uint64 z_page_mask;

#define USRSTACK64          ((z_uint64)0x00007FFEEFC00000ULL)
#define DFLSSIZ             (8 * 1024 * 1024)           /* initial stack size limit */
#define offsetof(t, d)      __builtin_offsetof(t, d)
#define trunc_page(x)       ((x) & (~(z_page_size - 1)))
#define round_page(x)       trunc_page((x) + (z_page_size - 1))
#define PTHREAD_STACK_MIN   8192
#define PTHREAD_T_OFFSET    0

//#define __PTHREAD_SIZE__            1168
#define __PTHREAD_ATTR_SIZE__       56
#define __PTHREAD_MUTEXATTR_SIZE__  8
#define __PTHREAD_MUTEX_SIZE__      56
#define __PTHREAD_CONDATTR_SIZE__   8
#define __PTHREAD_COND_SIZE__       40
#define __PTHREAD_ONCE_SIZE__       8
#define __PTHREAD_RWLOCK_SIZE__     192
#define __PTHREAD_RWLOCKATTR_SIZE__ 16

#define z_pthread_cleanup_push(func, val) \
   { \
         struct z_pthread_handler_rec __handler; \
         struct z_pthread_t *__self = z_pthread_self(); \
         __handler.__routine = func; \
         __handler.__arg = val; \
         __handler.__next = __self->__cleanup_stack; \
         __self->__cleanup_stack = &__handler;

#define z_pthread_cleanup_pop(execute) \
         /* Note: 'handler' must be in this same lexical context! */ \
         __self->__cleanup_stack = __handler.__next; \
         if (execute) (__handler.__routine)(__handler.__arg); \
   }

z_uint64 _pthread_ptr_munge_token;
z_uint8 default_priority;
z_uint64 __pthread_stack_hint = 0xB0000000;
z_int32 __is_threaded = 0;
z_int32 __pthread_supported_features;
z_int32 __unix_conforming = 0;
z_int32 __pthread_mutex_default_opt_policy = _PTHREAD_MTX_OPT_POLICY_DEFAULT;
struct z_pthread_globals_s __globals;

struct z_pthread_attr_s z_pthread_attr_default =
{
    .sig       = _PTHREAD_ATTR_SIG,
    .stacksize = 0,
    .detached  = PTHREAD_CREATE_JOINABLE,
    .inherit   = _PTHREAD_DEFAULT_INHERITSCHED,
    .policy    = _PTHREAD_DEFAULT_POLICY,
    .defaultguardpage = 1,
    // compile time constant for _pthread_default_priority(0)
    .qosclass  = (1U << (THREAD_QOS_LEGACY - 1 + _PTHREAD_PRIORITY_QOS_CLASS_SHIFT)) | ((z_uint8)-1 & _PTHREAD_PRIORITY_PRIORITY_MASK),
};

struct z_pthread_t _main_thread __attribute__((aligned(64))) = { };

union z_mutex_seq
{
    z_uint32 seq[2];
    struct
    {
        z_uint32 lgenval;
        z_uint32 ugenval;
    };
    struct
    {
        z_uint32 mgen;
        z_uint32 ugen;
    };
    z_uint64 seq_LU;
    z_uint64 atomic_seq_LU;
};

enum z_thread_destruct_special_reply_port_rights
{
    THREAD_SPECIAL_REPLY_PORT_ALL,
    THREAD_SPECIAL_REPLY_PORT_RECEIVE_ONLY,
    THREAD_SPECIAL_REPLY_PORT_SEND_ONLY,
};

struct z_pthread_once_context
{
    struct z_pthread_once_t *pthread_once;
    z_void (*routine)(z_void);
};

z_int32 z_pthread_selfid(z_void)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_thread_selfid)));
}

z_int32 z_bsdthread_register(z_void *thread_start, z_void *wqthread, z_int32 pthsize, z_void *pth_regdata, z_uint64 pth_regdata_len, z_uint64 dpq_offset)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_bsdthread_register), thread_start, wqthread, pthsize, pth_regdata, pth_regdata_len, dpq_offset));
}

z_int32 z_bsdthread_create(z_void *thread_start, z_void *arg, z_void *stack, z_void *pthread, z_uint32 flags)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_bsdthread_create), thread_start, arg, stack, pthread, flags));
}

z_int32 z_bsdthread_terminate(z_void *freeaddr, z_uint64 freesize, z_uint32 kport, z_uint32 joinsem)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_bsdthread_terminate), freeaddr, freesize, kport, joinsem));
}

z_int32 z_pthread_markcancel(z_uint32 kport)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS___pthread_markcancel), kport));
}

z_int32 z_pthread_canceled(z_uint32 kport)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS___pthread_canceled), kport));
}

z_int32 z_disable_threadsignal(z_uint32 kport)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS___disable_threadsignal), kport));
}

struct z_pthread_t *z_main_thread(z_void)
{
    return &_main_thread;
}

z_void z_pthread_set_kernel_thread(struct z_pthread_t *t, z_uint32 p)
{
    t->tsd[_PTHREAD_TSD_SLOT_MACH_THREAD_SELF] = p;
}

z_void z_pthread_set_self(z_void)
{
    struct z_pthread_t *p = z_main_thread();
    p->thread_id = z_pthread_selfid();
    p->tsd[_PTHREAD_TSD_SLOT_PTHREAD_SELF] = (z_uint64)p;
    p->tsd[_PTHREAD_TSD_SLOT_ERRNO] = (z_uint64)&(p->err_no);
    // Set the start address of the main thread TSD
    z_thread_set_tsd_base(p->tsd);
}

struct z_pthread_t *z_pthread_self(z_void)
{
    return (struct z_pthread_t *)z_pthread_getspecific_direct(_PTHREAD_TSD_SLOT_PTHREAD_SELF);
}

z_uint64 z_pthread_selfid_direct(z_void)
{
    return (z_pthread_self())->thread_id;
}

z_void z_pthread_set_reply_port(struct z_pthread_t *t, z_uint32 reply_port)
{
    z_uint64 p = (z_uint64)reply_port;
    if (t == Z_NULL)
    {
        z_pthread_setspecific_direct(_PTHREAD_TSD_SLOT_MIG_REPLY, p);
    }
    else
    {
        t->tsd[_PTHREAD_TSD_SLOT_MIG_REPLY] = p;
    }
}

z_uint64 z_pthread_attr_guardsize(struct z_pthread_attr_s *attr)
{
    return attr->defaultguardpage ? z_page_size : attr->guardsize;
}

z_uint64 z_pthread_attr_stacksize(struct z_pthread_attr_s *attr)
{
    return attr->stacksize ? attr->stacksize : DEFAULT_STACK_SIZE;
}

z_void z_pthread_struct_init(struct z_pthread_t *t, struct z_pthread_attr_s *attrs, z_void *stackaddr, z_uint64 stacksize, z_void *freeaddr, z_uint64 freesize)
{
    t->sig = _PTHREAD_SIG;
    t->tsd[_PTHREAD_TSD_SLOT_PTHREAD_SELF] = (z_uint64)t;
    t->tsd[_PTHREAD_TSD_SLOT_ERRNO] = (z_uint64)&(t->err_no);
    if (attrs->schedset == 0)
    {
        t->tsd[_PTHREAD_TSD_SLOT_PTHREAD_QOS_CLASS] = attrs->qosclass;
    }
    else
    {
//        t->tsd[_PTHREAD_TSD_SLOT_PTHREAD_QOS_CLASS] = _pthread_unspecified_priority();
    }
    t->tl_has_custom_stack = (attrs->stackaddr != Z_NULL);
    t->stackaddr = stackaddr;
    t->stackbottom = stackaddr - stacksize;
    t->freeaddr = freeaddr;
    t->freesize = freesize;
    t->guardsize = z_pthread_attr_guardsize(attrs);
    t->tl_joinable = (attrs->detached == PTHREAD_CREATE_JOINABLE);
    t->inherit = attrs->inherit;
    t->tl_policy = attrs->policy;
    t->schedset = attrs->schedset;
    t->cancel_state = PTHREAD_CANCEL_ENABLE | PTHREAD_CANCEL_DEFERRED;
}

z_void z_pthread_main_thread_init(struct z_pthread_t *p)
{
    z_pthread_set_kernel_thread(p, z_pthread_self_trap());
    z_pthread_set_reply_port(p, z_mach_reply_port());
    p->__cleanup_stack = Z_NULL;
    p->tl_join_ctx = Z_NULL;
    p->tl_exit_gate = Z_NULL;
    p->tsd[__TSD_SEMAPHORE_CACHE] = Z_NULL;
    p->tsd[__TSD_MACH_SPECIAL_REPLY] = 0;
}

z_void z_pthread_bsdthread_init(struct z_pthread_registration_data *data)
{
    z_uint32 pagesize = PTHREAD_SIZE;
    z_memset(data, 0, sizeof(*data));
    data->version = sizeof(struct z_pthread_registration_data);
    data->dispatch_queue_offset = __PTK_LIBDISPATCH_KEY0 * sizeof(z_void *);
    data->return_to_kernel_offset = __TSD_RETURN_TO_KERNEL * sizeof(z_void *);
    data->tsd_offset = offsetof(struct z_pthread_t, tsd);
    data->mach_thread_self_offset = __TSD_MACH_THREAD_SELF * sizeof(z_void *);
      
    /*
     tsd_offset has told the kernel that when a new thread is subsequently created,
     the kernel knows where the tsd start address is and sets the gs register
    */
    z_int32 rv = z_bsdthread_register(z_thread_start, z_start_wqthread, pagesize, (z_void *)data, sizeof(*data), data->dispatch_queue_offset);
    if (rv > 0)
    {
        z_int32 required_features = PTHREAD_FEATURE_FINEPRIO | PTHREAD_FEATURE_BSDTHREADCTL | PTHREAD_FEATURE_SETSELF | PTHREAD_FEATURE_QOS_MAINTENANCE | PTHREAD_FEATURE_QOS_DEFAULT;
        if ((rv & required_features) != required_features)
        {
            z_printf("Missing required kernel support %d\n", rv);
        }
        __pthread_supported_features = rv;
    }
    if (data->stack_addr_hint)
    {
        __pthread_stack_hint = data->stack_addr_hint;
    }
}

z_int32 z_pthread_init(z_int8 *envp[], z_int8 *apple[])
{
    struct z_pthread_t *thread;
    z_void *stackaddr = 0;
    z_int32 stacksize = 0;
    z_void *allocaddr = 0;
    z_int32 allocsize = 0;

    z_int32 len = sizeof(stackaddr);
    z_int32 mib[] = {CTL_KERN, KERN_USRSTACK64};
    if (z_sysctl(mib, 2, &stackaddr, &len, Z_NULL, 0) != 0)
    {
        stackaddr = (z_void *)USRSTACK64;
    }
    stacksize = DFLSSIZ;
    allocaddr = 0;
    allocsize = 0;
    
    thread = (struct z_pthread_t *)z_pthread_getspecific_direct(_PTHREAD_TSD_SLOT_PTHREAD_SELF);
    z_pthread_struct_init(thread, &z_pthread_attr_default, stackaddr, stacksize, allocaddr, allocsize);
    thread->tl_joinable = 1;
    z_pthread_main_thread_init(thread);
    
    struct z_pthread_registration_data registration_data;
    z_pthread_bsdthread_init(&registration_data);
    return 0;
}

void z_pthread_attr_get_schedparam(struct z_pthread_attr_s *attr, struct z_sched_param *param)
{
    if (attr->schedset)
    {
        *param = attr->param;
    }
    else
    {
        param->sched_priority = default_priority;
        param->quantum = 10; /* quantum isn't public yet */
    }
}

struct z_pthread_t *z_pthread_allocate(struct z_pthread_attr_s *attrs, z_void **stack, z_int8 from_mach_thread)
{
    z_uint64 allocaddr = __pthread_stack_hint;
    z_uint64 allocsize, guardsize, stacksize, pthreadoff;
    struct z_pthread_t *t;

    if (attrs->stacksize != 0 && attrs->stacksize < PTHREAD_STACK_MIN)
    {
        return Z_NULL;
    }

    if (((z_uint64)attrs->stackaddr % z_page_size) != 0)
    {
        return Z_NULL;
    }
    
    if (attrs->stackaddr != Z_NULL)
    {
        allocsize = PTHREAD_SIZE;
        guardsize = 0;
        pthreadoff = 0;
        stacksize = attrs->stacksize;
    }
    else
    {
        guardsize = z_pthread_attr_guardsize(attrs);
        stacksize = z_pthread_attr_stacksize(attrs) + PTHREAD_T_OFFSET;
        pthreadoff = stacksize + guardsize;
        allocsize = pthreadoff + PTHREAD_SIZE;
        allocsize = vm_round_page(allocsize);
    }

    allocaddr = (z_uint64)z_mmap(Z_NULL, allocsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (allocaddr == MAP_FAILED || allocaddr == Z_NULL)
    {
        return Z_NULL;
    }

    // The stack grows down.
    // Set the guard page at the lowest address of the
    // newly allocated stack. Return the highest address
    // of the stack.
    if (guardsize)
    {
        z_mprotect((z_void *)allocaddr, (z_int32)guardsize, PROT_NONE);
    }

    // Thread structure resides at the top of the stack (when using a
    // custom stack, allocsize == PTHREAD_SIZE, so places the pthread_t
    // at allocaddr).
    t = (struct z_pthread_t *)(allocaddr + pthreadoff);
    if (attrs->stackaddr)
    {
        *stack = attrs->stackaddr;
    }
    else
    {
        *stack = t;
    }

    z_pthread_struct_init(t, attrs, *stack, stacksize, (z_void *)allocaddr, allocsize);
    return t;
}

z_void z_pthread_deallocate(struct z_pthread_t *t, z_int8 from_mach_thread)
{
    // Don't free the main thread.
    if (t != z_main_thread())
    {
        z_munmap(t->freeaddr, t->freesize);
    }
}

z_int32 z_pthread_create_internal(struct z_pthread_t **thread, struct z_pthread_attr_s *attrs, z_void *(*start_routine)(z_void *), z_void *arg, z_uint32 create_flags)
{
    struct z_pthread_t *t = Z_NULL;
    z_void *stack = Z_NULL;
    z_int8 from_mach_thread = (create_flags & _PTHREAD_CREATE_FROM_MACH_THREAD);

    if (attrs == Z_NULL)
    {
        attrs = &z_pthread_attr_default;
    }
    else if (attrs->sig != _PTHREAD_ATTR_SIG)
    {
        return EINVAL;
    }

    z_uint32 flags = PTHREAD_START_CUSTOM;
    if (attrs->schedset != 0)
    {
        struct z_sched_param p;
        z_pthread_attr_get_schedparam(attrs, &p);
        flags |= PTHREAD_START_SETSCHED;
        flags |= ((attrs->policy & PTHREAD_START_POLICY_MASK) << PTHREAD_START_POLICY_BITSHIFT);
        flags |= (p.sched_priority & PTHREAD_START_IMPORTANCE_MASK);
    }
    else if (attrs->qosclass != 0)
    {
        flags |= PTHREAD_START_QOSCLASS;
        flags |= (attrs->qosclass & PTHREAD_START_QOSCLASS_MASK);
    }
    
    if (create_flags & _PTHREAD_CREATE_SUSPENDED)
    {
        flags |= PTHREAD_START_SUSPENDED;
    }

    __is_threaded = 1;

    t = z_pthread_allocate(attrs, &stack, from_mach_thread);
    if (t == Z_NULL)
    {
        return EAGAIN;
    }

    t->arg = arg;
    t->fun = start_routine;
//    __pthread_add_thread(t, from_mach_thread);
    
    /*
     When the main thread initializes and executes z_bsdthread_register,
     the kernel already knows the thread structure and tsd starting address
     tsd_offset has told the kernel that when a new thread is subsequently created,
     the kernel knows where the tsd start address is and sets the gs register
    */
    z_int32 ret = z_bsdthread_create(start_routine, arg, stack, t, flags);
    if (ret == -1)
    {
        if (z_errno == EMFILE)
        {
            z_printf("Unable to allocate thread port, possible port leak\n");
        }
//        __pthread_undo_add_thread(t, self_kport);
        z_pthread_deallocate(t, from_mach_thread);
        return EAGAIN;
    }
    // n.b. if a thread is created detached and exits, t will be invalid
    *thread = t;
    return 0;
}

z_int32 z_pthread_create(struct z_pthread_t **thread, struct z_pthread_attr_s *attr, z_void *(*start_routine)(z_void *), z_void *arg)
{
    z_uint32 flags = _PTHREAD_CREATE_NONE;
    return z_pthread_create_internal(thread, attr, start_routine, arg, flags);
}

z_void z_pthread_markcancel_if_canceled(struct z_pthread_t * thread, z_uint32 kport)
{
    z_int32 flags = (PTHREAD_CANCEL_ENABLE | _PTHREAD_CANCEL_PENDING);
    z_int32 state = 0;//os_atomic_load2o(thread, cancel_state, relaxed);
    if ((state & flags) == flags && __unix_conforming)
    {
        z_pthread_markcancel(kport);
    }
}

z_int8 z_pthread_is_canceled(struct z_pthread_t *thread)
{
    z_int32 flags = (PTHREAD_CANCEL_ENABLE | _PTHREAD_CANCEL_PENDING);
    z_int32 state = 0;//os_atomic_load2o(thread, cancel_state, seq_cst);
    return (state & flags) == flags;
}

z_void z_pthread_set_self_internal(struct z_pthread_t *p)
{
    p->thread_id = z_pthread_selfid();
}

z_uint64 z_pthread_kernel_thread(struct z_pthread_t *t)
{
    return t->tsd[_PTHREAD_TSD_SLOT_MACH_THREAD_SELF];
}

z_void z_pthread_started_thread(struct z_pthread_t *t)
{

}

z_int32 z_pthread_update_cancel_state(struct z_pthread_t *thread, z_int32 mask, z_int32 state)
{
    z_int32 oldstate, newstate;
//    os_atomic_rmw_loop2o(thread, cancel_state, oldstate, newstate, seq_cst, {
//        newstate = oldstate;
//        newstate &= ~mask;
//        newstate |= state;
//    });
    return oldstate;
}

z_void z_pthread_setcancelstate_exit(struct z_pthread_t *thread, z_void *value_ptr)
{
    z_pthread_update_cancel_state(thread, _PTHREAD_CANCEL_STATE_MASK | _PTHREAD_CANCEL_TYPE_MASK, PTHREAD_CANCEL_DISABLE | PTHREAD_CANCEL_DEFERRED);
}

z_uint64 z_pthread_get_stacksize_np(struct z_pthread_t *t)
{
    z_uint64 size = 0;

    if (t == Z_NULL)
    {
        return ESRCH; // XXX bug?
    }

    if (t == z_main_thread())
    {
        z_uint64 stacksize = t->stackaddr - t->stackbottom;
        if (stacksize + z_page_size != t->freesize)
        {
            // We want to call getrlimit() just once, as it's relatively
            // expensive
            z_uint64 rlimit_stack = 0;

            if (rlimit_stack == 0)
            {
                struct z_rlimit limit;
                z_int32 ret = z_getrlimit(RLIMIT_STACK, &limit);
                if (ret == 0)
                {
                    rlimit_stack = (z_uint64)limit.rlim_cur;
                }
            }

            if (rlimit_stack == 0 || rlimit_stack > t->freesize)
            {
                return stacksize;
            }
            else
            {
                return round_page(rlimit_stack);
            }
        }
    }

    if (t == z_pthread_self() || t == z_main_thread())
    {
        size = t->stackaddr - t->stackbottom;;
        goto out;
    }

out:
    // <rdar://problem/42588315> binary compatibility issues force us to return
    // DEFAULT_STACK_SIZE here when we do not know the size of the stack
    return size ? size : DEFAULT_STACK_SIZE;
}

z_uint64 z_pthread_current_stack_address(z_void)
{
    z_int32 a = 0;
    z_uint64 p = (z_uint64)&a;
    return p;
}

z_uint32 z_pthread_special_reply_port(struct z_pthread_t *t)
{
    z_uint64 p = 0;
    if (t == Z_NULL)
    {
        p = z_pthread_getspecific_direct(_PTHREAD_TSD_SLOT_MACH_SPECIAL_REPLY);
    }
    else
    {
        p = t->tsd[_PTHREAD_TSD_SLOT_MACH_SPECIAL_REPLY];
    }
    return (z_uint32)p;
}

z_uint32 z_pthread_reply_port(struct z_pthread_t *t)
{
    z_uint64 p = 0;
    if (t == Z_NULL)
    {
        p = z_pthread_getspecific_direct(_PTHREAD_TSD_SLOT_MIG_REPLY);
    }
    else
    {
        p = t->tsd[_PTHREAD_TSD_SLOT_MIG_REPLY];
    }
    return (z_uint32)p;
}

z_void z_pthread_dealloc_special_reply_port(struct z_pthread_t *t)
{
    z_uint32 special_reply_port = z_pthread_special_reply_port(t);
    if (special_reply_port != Z_NULL)
    {
//        z_thread_destruct_special_reply_port(special_reply_port, THREAD_SPECIAL_REPLY_PORT_ALL);
    }
}

z_void z_pthread_dealloc_reply_port(struct z_pthread_t *t)
{
    z_uint32 reply_port = z_pthread_reply_port(t);
    if (reply_port != Z_NULL)
    {
//        mig_dealloc_reply_port(reply_port);
    }
}

z_void z_pthread_terminate(struct z_pthread_t *t, z_void *exit_value)
{
    z_uint64 freeaddr = (z_uint64)t->freeaddr;
    z_uint64 freesize = t->freesize;
    z_int8 should_exit = 0;

    // the size of just the stack
    z_uint64 freesize_stack = t->freesize;

    // We usually pass our structure+stack to bsdthread_terminate to free, but
    // if we get told to keep the pthread_t structure around then we need to
    // adjust the free size and addr in the pthread_t to just refer to the
    // structure and not the stack.  If we do end up deallocating the
    // structure, this is useless work since no one can read the result, but we
    // can't do it after the call to pthread_remove_thread because it isn't
    // safe to dereference t after that.
    if ((z_void *)t > t->freeaddr && (z_void *)t < t->freeaddr + t->freesize)
    {
        // Check to ensure the pthread structure itself is part of the
        // allocation described by freeaddr/freesize, in which case we split and
        // only deallocate the area below the pthread structure.  In the event of a
        // custom stack, the freeaddr/size will be the pthread structure itself, in
        // which case we shouldn't free anything (the final else case).
        freesize_stack = trunc_page((z_uint64)t - (z_uint64)freeaddr);

        // describe just the remainder for deallocation when the pthread_t goes away
        t->freeaddr += freesize_stack;
        t->freesize -= freesize_stack;
    }
    else if (t == z_main_thread())
    {
        freeaddr = (z_uint64)(t->stackaddr) - z_pthread_get_stacksize_np(t);
        z_uint64 stackborder = trunc_page((z_uint64)z_pthread_current_stack_address());
        freesize_stack = stackborder - freeaddr;
    }
    else
    {
        freesize_stack = 0;
    }

    z_uint32 kport = (z_uint32)z_pthread_kernel_thread(t);
    z_int8 keep_thread_struct = 0, needs_wake = 0;
    z_uint32 custom_stack_sema = Z_NULL;

    z_pthread_dealloc_special_reply_port(t);
    z_pthread_dealloc_reply_port(t);

    // This piece of code interacts with pthread_join. It will always:
    // - set tl_exit_gate to MACH_PORT_DEAD (thread exited)
    // - set tl_exit_value to the value passed to pthread_exit()
    // - decrement _pthread_count, so that we can exit the process when all
    //   threads exited even if not all of them were joined.
    t->tl_exit_gate = 0;
    t->tl_exit_value = exit_value;
//    should_exit = (--_pthread_count <= 0);

    // If we see a joiner, we prepost that the join has to succeed,
    // and the joiner is committed to finish (even if it was canceled)
    if (t->tl_join_ctx)
    {
        custom_stack_sema = z_pthread_joiner_prepost_wake(t); // unsets tl_joinable
        needs_wake = 1;
    }

    // Joinable threads that have no joiner yet are kept on the thread list
    // so that pthread_join() can later discover the thread when it is joined,
    // and will have to do the pthread_t cleanup.
    if (t->tl_joinable)
    {
        t->tl_joiner_cleans_up = keep_thread_struct = 1;
    }
    else
    {
//        TAILQ_REMOVE(&__pthread_head, t, tl_plist);
    }

    if (needs_wake)
    {
        // When we found a waiter, we want to drop the very contended list lock
        // before we do the syscall in _pthread_joiner_wake(). Then, we decide
        // who gets to cleanup the pthread_t between the joiner and the exiting
        // thread:
        // - the joiner tries to set tl_join_ctx to NULL
        // - the exiting thread tries to set tl_joiner_cleans_up to true
        // Whoever does it first commits the other guy to cleanup the pthread_t
        z_pthread_joiner_wake(t);
        if (t->tl_join_ctx)
        {
            t->tl_joiner_cleans_up = 1;
            keep_thread_struct = 1;
        }
    }

    if (keep_thread_struct || t == z_main_thread())
    {
        // Use the adjusted freesize of just the stack that we computed above.
        freesize = freesize_stack;
    }
    else
    {
//        _pthread_introspection_thread_destroy(t);
    }

    // Check if there is nothing to free because the thread has a custom
    // stack allocation and is joinable.
    if (freesize == 0)
    {
        freeaddr = 0;
    }
    if (should_exit)
    {
        z_exit(0);
    }
    z_bsdthread_terminate((z_void *)freeaddr, freesize, kport, custom_stack_sema);
}

z_void z_pthread_terminate_invoke(struct z_pthread_t *t, z_void *exit_value)
{
    z_pthread_terminate(t, exit_value);
}

z_void z_pthread_exit(struct z_pthread_t *self, z_void *exit_value)
{
    struct z_pthread_handler_rec *handler;

    // Disable signal delivery while we clean up
    z_disable_threadsignal(1);

    // Set cancel state to disable and type to deferred
    z_pthread_setcancelstate_exit(self, exit_value);
    while ((handler = self->__cleanup_stack) != 0)
    {
        (handler->__routine)(handler->__arg);
        self->__cleanup_stack = handler->__next;
    }
    z_pthread_tsd_cleanup(self);
    
    z_pthread_terminate_invoke(self, exit_value);
}

z_void z_pthread_start(struct z_pthread_t *self, z_uint32 kport, z_void *(*fun)(z_void *), z_void *arg, z_uint64 stacksize, z_uint32 pflags)
{
    z_pthread_markcancel_if_canceled(self, kport);
    z_pthread_set_self_internal(self);
    z_pthread_started_thread(self);
    z_pthread_exit(self, (self->fun)(self->arg));
}

z_int32 z_pthread_conformance(z_void)
{
    return PTHREAD_CONFORM_DARWIN_LEGACY;
}

z_void z_pthread_testcancel(z_int32 isconforming)
{
    struct z_pthread_t *self = z_pthread_self();
    if (z_pthread_is_canceled(self))
    {
        self->canceled = (isconforming != PTHREAD_CONFORM_DARWIN_LEGACY);
        z_pthread_exit(self, isconforming ? PTHREAD_CANCELED : Z_NULL);
    }
}

z_void *z_pthread_get_exit_value(struct z_pthread_t *thread)
{
    if (__unix_conforming && z_pthread_is_canceled(thread))
    {
        return PTHREAD_CANCELED;
    }
    return thread->tl_exit_value;
}

z_int8 z_pthread_joiner_abort_wait(struct z_pthread_t *thread, struct z_pthread_join_context_s *ctx)
{
    z_int8 aborted = 0;

    if (!ctx->detached && thread->tl_exit_gate != 0)
    {
        /*
         * _pthread_joiner_prepost_wake() didn't happen
         * allow another thread to join
         */
        thread->tl_join_ctx = Z_NULL;
        thread->tl_exit_gate = Z_NULL;
        aborted = 1;
    }

    return aborted;
}

z_int32 z_pthread_joiner_wait(struct z_pthread_t *thread, struct z_pthread_join_context_s *ctx, z_int32 conforming)
{
    z_uint32 *exit_gate = &thread->tl_exit_gate;
    z_int32 ulock_op = UL_UNFAIR_LOCK | ULF_NO_ERRNO;

    if (conforming == PTHREAD_CONFORM_UNIX03_CANCELABLE)
    {
        ulock_op |= ULF_WAIT_CANCEL_POINT;
    }

    for (;;)
    {
        z_uint32 cur = __sync_add_and_fetch(exit_gate, Z_NULL);
        if (cur == ((z_uint32)~0))
        {
            break;
        }

        if (cur != ctx->kport)
        {
            break;;
        }
        
        z_int32 ret = z_ulock_wait(ulock_op, exit_gate, ctx->kport, 0);
        switch (-ret)
        {
            case 0:
            case EFAULT:
                break;
            case EINTR:
                /*
                 * POSIX says:
                 *
                 *   As specified, either the pthread_join() call is canceled, or it
                 *   succeeds, but not both. The difference is obvious to the
                 *   application, since either a cancellation handler is run or
                 *   pthread_join() returns.
                 *
                 * When __ulock_wait() returns EINTR, we check if we have been
                 * canceled, and if we have, we try to abort the wait.
                 *
                 * If we can't, it means the other thread finished the join while we
                 * were being canceled and commited the waiter to return from
                 * pthread_join(). Returning from the join then takes precedence
                 * over the cancelation which will be acted upon at the next
                 * cancelation point.
                 */
                if (conforming == PTHREAD_CONFORM_UNIX03_CANCELABLE
                    && z_pthread_is_canceled(ctx->waiter))
                {
                    if (z_pthread_joiner_abort_wait(thread, ctx))
                    {
                        ctx->waiter->canceled = 1;
                        z_pthread_exit(z_pthread_self(), PTHREAD_CANCELED);
                    }
                }
                break;
        }
    }

    z_int8 cleanup = 0;

    // If pthread_detach() was called, we can't safely dereference the thread,
    // else, decide who gets to deallocate the thread (see _pthread_terminate).
    if (!ctx->detached)
    {
        thread->tl_join_ctx = Z_NULL;
        cleanup = thread->tl_joiner_cleans_up;
    }
    if (cleanup)
    {
        z_pthread_deallocate(thread, 0);
    }
    
    return 0;
}

z_int32 z_pthread_join_internal(struct z_pthread_t *thread, z_void **value_ptr, z_int32 conforming)
{
    struct z_pthread_t *self = z_pthread_self();
    struct z_pthread_join_context_s ctx = {
        .waiter = self,
        .value_ptr = value_ptr,
        .kport = Z_NULL,
        .custom_stack_sema = Z_NULL,
    };
    z_int32 res = 0;
    z_int32 kr = 0;

    if (!thread->tl_joinable || (thread->tl_join_ctx != Z_NULL))
    {
        res = EINVAL;
    }
    else if (thread == self || (self->tl_join_ctx && self->tl_join_ctx->waiter == thread))
    {
        res = EDEADLK;
    }
    else if (thread->tl_exit_gate == ((z_uint32)~0))
    {
        thread->tl_joinable = 0;
        if (value_ptr)
        {
            *value_ptr = z_pthread_get_exit_value(thread);
        }
    }
    else
    {
        ctx.kport = (z_uint32)z_pthread_kernel_thread(thread);
        thread->tl_exit_gate = ctx.kport;
        thread->tl_join_ctx = &ctx;
        if (thread->tl_has_custom_stack)
        {
//            ctx.custom_stack_sema = (semaphore_t)os_get_cached_semaphore();
        }
    }

    if (res == 0)
    {
        if (ctx.kport == Z_NULL)
        {
            z_pthread_deallocate(thread, 0);
        }
        else
        {
            res = z_pthread_joiner_wait(thread, &ctx, conforming);
        }
    }
    
    if (res == 0 && ctx.custom_stack_sema && !ctx.detached)
    {
        // threads with a custom stack need to make sure _pthread_terminate
        // returned before the joiner is unblocked, the joiner may quickly
        // deallocate the stack with rather dire consequences.
        //
        // When we reach this point we know the pthread_join has to succeed
        // so this can't be a cancelation point.
        do {
//            kr = __semwait_signal_nocancel(ctx.custom_stack_sema, 0, 0, 0, 0, 0);
        } while (kr != 0);
    }
    if (ctx.custom_stack_sema)
    {
//        os_put_cached_semaphore(ctx.custom_stack_sema);
    }
    return res;
}

z_int32 z_pthread_join(struct z_pthread_t *thread, z_void **value_ptr)
{
    z_int32 conforming = z_pthread_conformance();
    if (conforming == PTHREAD_CONFORM_UNIX03_CANCELABLE)
    {
        z_pthread_testcancel(conforming);
    }
    return z_pthread_join_internal(thread, value_ptr, conforming);
}


z_int8 z_pthread_is_valid(struct z_pthread_t *thread, z_uint32 *portp)
{
    z_uint32 kport = Z_NULL;
    z_int8 valid;

    if (thread == z_pthread_self())
    {
        valid = 1;
        kport = (z_uint32)z_pthread_kernel_thread(thread);
    }
    else
    {
        kport = (z_uint32)z_pthread_kernel_thread(thread);
        valid = 1;
    }

    if (portp != Z_NULL)
    {
        *portp = kport;
    }
    return valid;
}

z_int32 z_pthread_cancel(struct z_pthread_t *thread)
{
    if (__unix_conforming == 0)
        __unix_conforming = 1;

    if (!z_pthread_is_valid(thread, Z_NULL))
    {
        return (ESRCH);
    }

    /* if the thread is a workqueue thread, then return error */
    if (thread->wqthread != 0)
    {
        return (ENOTSUP);
    }

    thread->cancel_state |= _PTHREAD_CANCEL_PENDING;
    return (0);
}

z_void z_pthread_exit_if_canceled(z_int32 error)
{
    if (((error & 0xff) == EINTR) && __unix_conforming && (z_pthread_canceled(0) == 0))
    {
        struct z_pthread_t *self = z_pthread_self();
        self->cancel_error = error;
        self->canceled = 1;
        z_pthread_exit(self, PTHREAD_CANCELED);
    }
}

z_sem_t z_pthread_joiner_prepost_wake(struct z_pthread_t *thread)
{
    struct z_pthread_join_context_s *ctx = thread->tl_join_ctx;
    z_sem_t sema = Z_NULL;

    if (thread->tl_joinable)
    {
        sema = ctx->custom_stack_sema;
        thread->tl_joinable = 0;
    }
    else
    {
        ctx->detached = 1;
        thread->tl_join_ctx = Z_NULL;
    }
    
    if (ctx->value_ptr)
    {
        *ctx->value_ptr = z_pthread_get_exit_value(thread);
    }
    return sema;
}

z_void z_pthread_joiner_wake(struct z_pthread_t *thread)
{
    z_uint32 *exit_gate = &thread->tl_exit_gate;

    for (;;)
    {
        z_int32 ret = z_ulock_wake(UL_UNFAIR_LOCK | ULF_NO_ERRNO, exit_gate, 0);
        if (ret == 0 || ret == -ENOENT)
        {
            return;
        }
        if (ret != -EINTR)
        {

        }
    }
}

z_int32 z_pthread_detach(struct z_pthread_t *thread)
{
    z_int32 res = 0;
    z_int8 join = 0, wake = 9;
    
    if (!thread->tl_joinable)
    {
        res = EINVAL;
    }
    else if (thread->tl_exit_gate == ((z_uint32)~0))
    {
        // Join the thread if it's already exited.
        join = 1;
    }
    else
    {
        thread->tl_joinable = 0; // _pthread_joiner_prepost_wake uses this
        if (thread->tl_join_ctx)
        {
            (z_void)z_pthread_joiner_prepost_wake(thread);
            wake = 1;
        }
    }

    if (join)
    {
        z_pthread_join(thread, Z_NULL);
    }
    else if (wake)
    {
        z_pthread_joiner_wake(thread);
    }
    return res;
}

z_int32 z_pthread_equal(struct z_pthread_t *t1, struct z_pthread_t *t2)
{
    return (t1 == t2);
}

z_int32 z_pthread_main_np(z_void)
{
    return z_pthread_self() == &_main_thread;
}

z_void z_pthread_yield_np(z_void)
{
    z_swtch_pri(0);
}

z_int64 z_pthread_cpu_number_np(z_uint32 *cpu_id)
{
    if (cpu_id == Z_NULL)
    {
        z_errno = EINVAL;
        return z_errno;
    }
    *cpu_id = z_tsd_cpu_number();
    return 0;
}

z_void z_pthread_atfork_prepare_handlers(z_void)
{
    struct z_pthread_globals_s *globals = &__globals;

    z_uint64 idx;
    for (idx = globals->atfork_count; idx > 0; --idx)
    {
        struct z_pthread_atfork_entry *e = &globals->atfork[idx - 1];
        if (e->prepare != Z_NULL)
        {
            e->prepare();
        }
    }
}

z_void z_pthread_atfork_prepare(z_void)
{
    struct z_pthread_globals_s *globals = &__globals;
    globals->psaved_self = z_pthread_self();
}

z_void z_pthread_atfork_parent(z_void)
{

}

z_void z_pthread_atfork_parent_handlers(z_void)
{
    struct z_pthread_globals_s *globals = &__globals;

    z_uint64 idx;
    for (idx = 0; idx < globals->atfork_count; ++idx)
    {
        struct z_pthread_atfork_entry *e = &globals->atfork[idx];
        if (e->parent != Z_NULL)
        {
            e->parent();
        }
    }
}

z_void z_pthread_main_thread_postfork_init(struct z_pthread_t *p)
{
    z_pthread_main_thread_init(p);
    z_pthread_set_self_internal(p);
}

z_void z_pthread_atfork_child(z_void)
{
    struct z_pthread_globals_s *globals = &__globals;
    __is_threaded = 0;
    z_pthread_main_thread_postfork_init(globals->psaved_self);
    struct z_pthread_registration_data registration_data;
    z_pthread_bsdthread_init(&registration_data);
}

z_void z_pthread_atfork_child_handlers(z_void)
{
    struct z_pthread_globals_s *globals = &__globals;
    z_uint64 idx;
    for (idx = 0; idx < globals->atfork_count; ++idx)
    {
        struct z_pthread_atfork_entry *e = &globals->atfork[idx];
        if (e->child != Z_NULL)
        {
            e->child();
        }
    }
}
