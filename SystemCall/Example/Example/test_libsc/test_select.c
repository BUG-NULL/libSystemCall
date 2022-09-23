//
//  test_select.c
//  Example
//
//  Created by NULL
//

#include "test_select.h"

z_void test_select(z_void)
{
    struct z_fd_set rd;
    struct z_timeval tv;
    z_int32 err;

    z_memset(&rd, 0, sizeof(rd));
//    FD_SET(0, &rd);

    tv.tv_sec = 3;
    tv.tv_usec = 0;

    err = z_select(1, &rd, 0, 0, &tv);
    if (err == 0)
    {
        z_printf("select time out!\n");
    }
    else if (err == -1)
    {
        z_printf("fail to select!\n");
    }
    else
    {
        z_printf("data is available!\n");
    }

    return;
}
