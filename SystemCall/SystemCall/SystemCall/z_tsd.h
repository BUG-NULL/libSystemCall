//
//  z_tsd.h
//  SystemCall
//
//  Created by NULL 
//

#ifndef z_tsd_h
#define z_tsd_h

#include <SystemCall/z_types.h>

/* The low nine slots of the TSD are reserved for libsyscall usage. */
#define __TSD_RESERVED_BASE         0
#define __TSD_RESERVED_MAX          9

#define __TSD_THREAD_SELF           0
#define __TSD_ERRNO                 1
#define __TSD_MIG_REPLY             2
#define __TSD_MACH_THREAD_SELF      3
#define __TSD_THREAD_QOS_CLASS      4
#define __TSD_RETURN_TO_KERNEL      5
/* slot 6 is reserved for Windows/WINE compatibility reasons */
#define __TSD_PTR_MUNGE             7
#define __TSD_MACH_SPECIAL_REPLY    8
#define __TSD_SEMAPHORE_CACHE       9

#define MSR_IA32_FS_BASE            0xC0000100
#define MSR_IA32_GS_BASE            0xC0000101
#define MSR_IA32_KERNEL_GS_BASE     0xC0000102
#define MSR_IA32_TSC_AUX            0xC0000103

z_int32 z_tsd_set_direct(z_uint64 slot, z_uint64 val);
z_uint64 z_tsd_get_direct(z_uint64 slot);
z_uint64 z_tsd_get_value(z_uint64 addr);
z_uint32 z_tsd_cpu_number(z_void);

#endif /* z_tsd_h */
