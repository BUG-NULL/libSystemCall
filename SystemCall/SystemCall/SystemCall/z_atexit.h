//
//  z_atexit.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_atexit_h
#define z_atexit_h

#include <SystemCall/z_types.h>

#define ATEXIT_SIZE     32

struct z_cxa_range_t
{
    z_void      *addr;
    z_uint64    length;
};

struct z_atexit_fn
{
    z_int32 fn_type;            /* ATEXIT_? from above */
    union
    {
        z_void (*std_func)(z_void);
        z_void (*cxa_func)(z_void *);
#ifdef __BLOCKS__
        z_void (^block)(z_void);
#endif /* __BLOCKS__ */
    } fn_ptr;                   /* function pointer */
    z_void *fn_arg;             /* argument for CXA callback */
    z_void *fn_dso;             /* shared module handle */
};

struct z_atexit
{
    struct z_atexit *next;                  /* next in list */
    z_int32 ind;                            /* next index in this table */
    struct z_atexit_fn fns[ATEXIT_SIZE];    /* the table itself */
};

z_void z_cxa_finalize(z_void *dso);
z_void z_cxa_finalize_ranges(struct z_cxa_range_t ranges[], z_uint32 count);
z_void z_atexit_init(z_void);
z_int32 z_atexit(z_void (*func)(z_void));

#endif /* z_atexit_h */
