//
//  test_signal.c
//  Example
//
//  Created by NULL
//

#include "test_signal.h"

//z_void signalHandler(z_int32 signo)
z_void signalHandler(z_int32 signo, struct z_siginfo *info, z_void *arg)
{
    struct z_timeval time;
    struct z_timezone zone;

    z_gettimeofday(&time, &zone);
    switch (signo)
    {
        case SIGALRM:
            z_printf("sec %d, usec %d caught the SIGALRM signal!\n", time.tv_sec, time.tv_usec);
            break;
        case SIGUSR1:
            z_printf("sec %d, usec %d caught the SIGUSR1 signal!\n", time.tv_sec, time.tv_usec);
            break;
        case SIGUSR2:
            z_printf("sec %d, usec %d caught the SIGUSR2 signal!\n", time.tv_sec, time.tv_usec);
            break;
        default:
            z_printf("sec %d, usec %d caught the %d signal!\n", time.tv_sec, time.tv_usec, signo);
            break;
    }
}

z_void test_sigaction(z_void)
{
    struct z_sigaction newact;

    newact.__sigaction_u.__sa_sigaction = signalHandler;
    newact.sa_mask = 0;
    newact.sa_flags = 0;
    z_sigaction(SIGALRM, &newact, 0);

    struct z_itimerval new_value;
    struct z_itimerval old_value;

    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 1;
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 200000;
    z_setitimer(ITIMER_REAL, &new_value, &old_value);

    z_sleep(5);

    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_usec = 0;
    z_setitimer(ITIMER_REAL, &new_value, &old_value);
}
