//
//  z_atexit.c
//  SystemCall
//
//  Created by NULL
//

#include "z_atexit.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_malloc.h"
#include "z_stdlib.h"
#include "z_string.h"
#include "z_pthread_tlv.h"

#define ATEXIT_FN_EMPTY     0
#define ATEXIT_FN_STD       1
#define ATEXIT_FN_CXA       2

struct z_atexit *_z_atexit;        /* points to head of LIFO stack */
z_int32 _z_atexit_new_registration;

z_int8 z_cxa_in_range(struct z_cxa_range_t ranges[], z_uint32 count, z_void *fn)
{
    z_uint64 addr = (z_uint64)fn;
    z_uint32 i;
    
    for (i = 0; i < count; ++i)
    {
        struct z_cxa_range_t *r = &ranges[i];
        if (addr < (z_uint64)r->addr)
        {
            continue;
        }
        if (addr < ((z_uint64)r->addr + r->length))
        {
            return 1;
        }
    }
    return 0;
}

z_void z_cxa_finalize_ranges(struct z_cxa_range_t ranges[], z_uint32 count)
{
    struct z_atexit *p;
    struct z_atexit_fn *fn;
    z_int32 n;

restart:
    for (p = _z_atexit; p; p = p->next)
    {
        for (n = p->ind; --n >= 0;)
        {
            fn = &p->fns[n];
            if (fn->fn_type == ATEXIT_FN_EMPTY)
            {
                continue; // already been called
            }

            // Verify that the entry is within the range being unloaded.
            if (count > 0)
            {
                if (fn->fn_type == ATEXIT_FN_CXA)
                {
                    // for __cxa_atexit(), call if *dso* is in range be unloaded
                    if (!z_cxa_in_range(ranges, count, fn->fn_dso))
                    {
                        continue; // not being unloaded yet
                    }
                }
                else if (fn->fn_type == ATEXIT_FN_STD)
                {
                    // for atexit, call if *function* is in range be unloaded
                    if (!z_cxa_in_range(ranges, count,  fn->fn_ptr.std_func))
                    {
                        continue; // not being unloaded yet
                    }
                }
            }

            // Clear the entry to indicate that this handler has been called.
            z_int32 fn_type = fn->fn_type;
            fn->fn_type = ATEXIT_FN_EMPTY;

            // Detect recursive registrations.
            _z_atexit_new_registration = 0;

            // Call the handler.
            if (fn_type == ATEXIT_FN_CXA)
            {
                fn->fn_ptr.cxa_func(fn->fn_arg);
            }
            else if (fn_type == ATEXIT_FN_STD)
            {
                fn->fn_ptr.std_func();
            }

            // Call any recursively registered handlers.
            if (_z_atexit_new_registration)
            {
                goto restart;
            }
        }
    }
}

z_void z_cxa_finalize(z_void *dso)
{
    if (dso != Z_NULL)
    {
        // Note: this should not happen as only dyld should be calling
        // this and dyld has switched to call __cxa_finalize_ranges directly.
        struct z_cxa_range_t range;
        range.addr = dso;
        range.length = 1;
        z_cxa_finalize_ranges(&range, 1);
    }
    else
    {
        z_cxa_finalize_ranges(Z_NULL, 0);
    }
}

z_void z_cxa_thread_atexit(z_void(*f)(z_void *), z_void *arg)
{
    z_pthread_tlv_atexit(f, arg);
}

z_void z_atexit_init(z_void)
{
    _z_atexit = z_malloc(sizeof(struct z_atexit));
    z_memset(_z_atexit, 0, sizeof(struct z_atexit));
}

z_int32 z_atexit_register(struct z_atexit_fn *fptr)
{
    struct z_atexit *p = _z_atexit;
    while (p->ind >= ATEXIT_SIZE)
    {
        struct z_atexit *old__atexit;
        old__atexit = _z_atexit;
        if ((p = (struct z_atexit *)z_malloc(sizeof(*p))) == Z_NULL)
            return (-1);
        if (old__atexit != _z_atexit)
        {
            z_free(p, 0);
            p = _z_atexit;
            continue;
        }
        p->ind = 0;
        p->next = _z_atexit;
        _z_atexit = p;
    }
    p->fns[p->ind++] = *fptr;
    _z_atexit_new_registration = 1;
    return 0;
}

z_int32 z_atexit(z_void (*func)(z_void))
{
    struct z_atexit_fn fn;
    z_int32 error;

    fn.fn_type = ATEXIT_FN_STD;
    fn.fn_ptr.std_func = func;
    fn.fn_arg = Z_NULL;
    fn.fn_dso = Z_NULL;
    error = z_atexit_register(&fn);
    return error;
}

