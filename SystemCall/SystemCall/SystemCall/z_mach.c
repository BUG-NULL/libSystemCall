//
//  z_mach.c
//  SystemCall
//
//  Created by NULL
//

#include "z_mach.h"
#include "z_mach_trap.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_stdio.h"
#include "z_string.h"

#define I386_PGSHIFT                            12
#define COMM_PAGE_KERNEL_PAGE_SHIFT_MIN_VERSION 14
#define _COMM_PAGE64_BASE_ADDRESS       (0x00007fffffe00000)
#define _COMM_PAGE_KERNEL_PAGE_SHIFT    (_COMM_PAGE64_BASE_ADDRESS + 0x04D)
#define _COMM_PAGE_VERSION              (_COMM_PAGE64_BASE_ADDRESS + 0x01E)
#define _COMM_PAGE_USER_PAGE_SHIFT_64   (_COMM_PAGE64_BASE_ADDRESS + 0x04E)

z_uint32 z_mach_task_self = Z_NULL;
z_uint32 z_task_reply_port = Z_NULL;

z_uint64 z_kernel_page_size = 0;
z_uint64 z_kernel_page_mask = 0;
z_int32 z_kernel_page_shift = 0;

z_uint64 z_page_size = 0;
z_uint64 z_page_mask = 0;
z_int32 z_page_shift = 0;
z_uint64 z_cpu_capabilities = 0;

z_void z_mach_init(z_void)
{
    z_mach_task_self = z_task_self_trap();
    z_task_reply_port = z_mach_reply_port();

    if (z_kernel_page_shift == 0)
    {
        if ((*((z_uint16 *)_COMM_PAGE_VERSION) >= COMM_PAGE_KERNEL_PAGE_SHIFT_MIN_VERSION))
        {
            z_kernel_page_shift = *(z_uint8 *)_COMM_PAGE_KERNEL_PAGE_SHIFT;
        }
        else
        {
            z_kernel_page_shift = I386_PGSHIFT;
        }
        z_kernel_page_size = 1 << z_kernel_page_shift;
        z_kernel_page_mask = z_kernel_page_size - 1;
    }

    if (z_page_shift == 0)
    {
        if ((*((z_uint16 *)_COMM_PAGE_VERSION) >= COMM_PAGE_KERNEL_PAGE_SHIFT_MIN_VERSION))
        {
            z_page_shift = *(z_uint8 *)_COMM_PAGE_USER_PAGE_SHIFT_64;
        }
        else
        {
            z_page_shift = z_kernel_page_shift;
        }
        z_page_size = 1 << z_page_shift;
        z_page_mask = z_page_size - 1;
    }

    z_cpu_capabilities = z_get_cpu_capabilities();
}

z_void z_get_processor_info(z_void)
{
    z_int32 cpu_model = 0;
    z_int32 cpu_family = 0;
    z_int8 cpu_model_name[48];
    z_uint32 brand[12];
    z_int8 *cpu_manufacturer = Z_NULL;
    z_uint64 a = 0;
    z_uint64 b = 0;
    z_uint64 unused = 0;
    
    z_cpuid(0, unused, b, unused, unused);
    if (b == 0x756e6547)
    {
        z_cpuid(1, a, b, unused, unused);
        cpu_manufacturer = "Intel";
        cpu_model = (a >> 4) & 0x0F;
        cpu_family = (a >> 8) & 0x0F;
    }
    else if (b == 0x68747541)
    {
        z_cpuid(1, a, unused, unused, unused);
        cpu_manufacturer = "AMD";
        cpu_model = (a >> 4) & 0x0F;
        cpu_family = (a >> 8) & 0x0F;
    }

    /* See if we can get a long manufacturer strings */
    z_cpuid(0x80000000, a, unused, unused, unused);
    if (a >= 0x80000004)
    {
        z_cpuid(0x80000002, brand[0], brand[1], brand[2], brand[3]);
        z_cpuid(0x80000003, brand[4], brand[5], brand[6], brand[7]);
        z_cpuid(0x80000004, brand[8], brand[9], brand[10], brand[11]);
        z_memcpy(cpu_model_name, brand, 48);
        z_printf("%s\n", cpu_model_name);
    }
}
