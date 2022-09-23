//
//  test_time.c
//  Example
//
//  Created by NULL
//

#include "test_time.h"

z_void test_time(z_void)
{
    struct z_timeval time;
    struct z_timezone zone;

    z_gettimeofday(&time, &zone);
    z_printf("sec %d, usec %d, minuteswest %d, dsttime %d\n", time.tv_sec, time.tv_usec, zone.tz_minuteswest, zone.tz_dsttime);
}
