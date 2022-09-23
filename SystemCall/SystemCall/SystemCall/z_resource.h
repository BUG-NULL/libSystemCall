//
//  z_resource.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_resource_h
#define z_resource_h

#include <SystemCall/z_types.h>

/*
 * Symbolic constants for resource limits; since all limits are representable
 * as a type rlim_t, we are permitted to define RLIM_SAVED_* in terms of
 * RLIM_INFINITY.
 */
#define RLIM_INFINITY   (((__uint64_t)1 << 63) - 1)     /* no limit */
#define RLIM_SAVED_MAX  RLIM_INFINITY   /* Unrepresentable hard limit */
#define RLIM_SAVED_CUR  RLIM_INFINITY   /* Unrepresentable soft limit */

/*
 * Possible values of the first parameter to getrlimit()/setrlimit(), to
 * indicate for which resource the operation is being performed.
 */
#define RLIMIT_CPU      0               /* cpu time per process */
#define RLIMIT_FSIZE    1               /* file size */
#define RLIMIT_DATA     2               /* data segment size */
#define RLIMIT_STACK    3               /* stack size */
#define RLIMIT_CORE     4               /* core file size */
#define RLIMIT_AS       5               /* address space (resident set size) */
#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
#define RLIMIT_RSS      RLIMIT_AS       /* source compatibility alias */
#define RLIMIT_MEMLOCK  6               /* locked-in-memory address space */
#define RLIMIT_NPROC    7               /* number of processes */
#endif  /* __DARWIN_C_LEVEL >= __DARWIN_C_FULL */
#define RLIMIT_NOFILE   8               /* number of open files */
#if __DARWIN_C_LEVEL >= __DARWIN_C_FULL
#define RLIM_NLIMITS    9               /* total number of resource limits */
#endif  /* __DARWIN_C_LEVEL >= __DARWIN_C_FULL */
#define _RLIMIT_POSIX_FLAG      0x1000  /* Set bit for strict POSIX */

struct z_rlimit
{
    z_uint64    rlim_cur;               /* current (soft) limit */
    z_uint64    rlim_max;               /* maximum value for rlim_cur */
};

z_int32 z_getrlimit(z_int32 resource, struct z_rlimit *rlp);
z_int32 z_setrlimit(z_int32 resource, struct z_rlimit *rlp);

#endif /* z_resource_h */
