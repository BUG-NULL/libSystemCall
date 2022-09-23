//
//  test_sysctl.c
//  Example
//
//  Created by NULL
//

#include "test_sysctl.h"

z_void test_sysctl(z_void)
{
    z_int32 mib[2];
    z_int32 value = 0;
    z_int32 size = 0;

    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;
    size = sizeof value;
    if (z_sysctl(mib, 2, &value, &size, Z_NULL, 0) == -1)
    {
        return;
    }

    z_printf("test sysctl page size %d\n", value);
}
