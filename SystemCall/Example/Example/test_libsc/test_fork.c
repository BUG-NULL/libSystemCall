//
//  test_fork.c
//  Example
//
//  Created by NULL
//

#include "test_fork.h"

z_void test_fork(z_void)
{
    z_int32 pid = 0;
    z_int32 count = 0;

    pid = z_fork();
    if (pid == 0)
    {
        while (count < 3)
        {
            count++;
            z_printf("I'm child gaga .......\n");
            z_sleep(3);
        }
        z_printf("Child exit .......\n");
    }
    else
    {
        while (count < 3)
        {
            count++;
            z_printf("I am father .......\n");
            z_sleep(3);
        }
        z_printf("Father exit .......\n");
    }
}
