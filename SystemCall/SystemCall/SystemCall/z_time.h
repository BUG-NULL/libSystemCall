//
//  z_time.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_time_h
#define z_time_h

#include <SystemCall/z_types.h>

#define ITIMER_REAL         0
#define ITIMER_VIRTUAL      1
#define ITIMER_PROF         2

#define TIMEVAL_TO_TIMESPEC(tv, ts) {       \
    (ts)->tv_sec = (tv)->tv_sec;            \
    (ts)->tv_nsec = (tv)->tv_usec * 1000;   \
}

struct z_timeval
{
    z_int64     tv_sec;         /* seconds */
    z_int32     tv_usec;        /* and microseconds */
};

struct z_timezone
{
    z_int32     tz_minuteswest; /* minutes west of Greenwich */
    z_int32     tz_dsttime;     /* type of dst correction */
};

struct z_timespec
{
    z_int64    tv_sec;
    z_int64    tv_nsec;
};

struct z_itimerval
{
    struct z_timeval    it_interval;    /* timer interval */
    struct z_timeval    it_value;       /* current value */
};

z_int32 z_gettimeofday(struct z_timeval *tv, struct z_timezone *tz);
z_int32 z_getitimer(z_int32 which, struct z_itimerval *value);
z_int32 z_setitimer(z_int32 which, struct z_itimerval *value, struct z_itimerval *ovalue);

#endif /* z_time_h */
