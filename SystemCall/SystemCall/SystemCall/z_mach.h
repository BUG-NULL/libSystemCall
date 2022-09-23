//
//  z_mach.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_mach_h
#define z_mach_h

#include <SystemCall/z_types.h>

#define z_cpuid(in, a, b, c, d) \
    do {asm volatile ("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(in)); } while(0)

z_void z_mach_init(z_void);
z_void z_get_processor_info(z_void);
z_uint64 z_get_cpu_capabilities(z_void);

#endif /* z_mach_h */
