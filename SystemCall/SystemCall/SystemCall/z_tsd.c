//
//  z_tsd.c
//  SystemCall
//
//  Created by NULL 
//

#include "z_tsd.h"
#include "z_syscall.h"
#include "z_error.h"

/*
 The use of TLS variables requires the cooperation of user programs,
 compilers, C runtime and OS.
*/
z_void z_wrmsr64(z_uint32 msr, z_uint64 val)
{
    __asm__ volatile ("wrmsr" : : "c" (msr), "a" (val & 0xFFFFFFFFUL), "d" ((val >> 32) & 0xFFFFFFFFUL));
}

z_uint64 z_rdmsr64(z_uint32 msr)
{
    z_uint32 lo = 0, hi = 0;
    __asm__ volatile ("rdmsr" : "=a" (lo), "=d" (hi) : "c" (msr));
    return ((z_uint64)hi << 32) | (z_uint64)lo;
}
 
z_uint64 z_tsd_get_value(z_uint64 addr)
{
    z_uint64 value = 0;
    __asm__ volatile ("mov %0, %%rax\n\t"::"r"(addr)) ;
    __asm__ volatile ("mov %gs:(%rax), %rax\n\t") ;
    __asm__ volatile ("mov %%rax, %[value]\n\t" :[value]"=r"(value)) ;
    return value;
}

z_int32 z_tsd_set_direct(z_uint64 slot, z_uint64 val)
{
    __asm__("movq %1, %%gs:%0" : "=m" (*(z_uint64 *)(slot * sizeof(z_uint64))) : "rn" (val));
    return 0;
}

z_uint64 z_tsd_get_direct(z_uint64 slot)
{
    z_uint64 ret;
    __asm__("mov %%gs:%1, %0" : "=r" (ret) : "m" (*(z_uint64 *)(slot * sizeof(z_uint64))));
    return ret;
}

z_uint32 z_tsd_cpu_number(z_void)
{
    struct
    {
        z_uint64 p1, p2;
    } p;
    __asm__("sidt %[p]" : [p] "=&m" (p));
    return (z_uint32)(p.p1 & 0xfff);
}
