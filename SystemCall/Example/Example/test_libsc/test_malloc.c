//
//  test_malloc.c
//  Example
//
//  Created by NULL
//

#include "test_malloc.h"

z_void test_malloc(z_void)
{
    z_int32 i = 0;
    z_void  *p = Z_NULL;

    for (i = 1; i <= 1000; i++)
    {
        p = z_malloc(i * 4);
        if (p == Z_NULL)
        {
            z_printf("malloc failed\n");
            break;
        }
        z_free(p, i * 4);
    }

    z_printf("malloc test success\n");
    return;
}
