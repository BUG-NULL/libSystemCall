//
//  test_protect.c
//  Example
//
//  Created by NULL
//

#include "test_protect.h"

#define PAGE_SIZE   0x1000

//z_void signal_handler(z_int32 signo)
z_void signal_handler(z_int32 signo, struct z_siginfo *info, z_void *arg)
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

z_void test_protect(z_void)
{
    z_int8 *p;
    z_int8 c;

    struct z_sigaction newact;

    newact.__sigaction_u.__sa_sigaction = signal_handler;
    newact.sa_mask = 0;
    newact.sa_flags = 0;
    z_sigaction(SIGSEGV, &newact, 0);

    /* Allocate a buffer; it will have the default
    protection of PROT_READ|PROT_WRITE. */
    p = z_malloc(1024 + PAGE_SIZE - 1);
    if (!p)
    {
        z_perror("Couldn’t malloc(1024)");
        return;
    }

    // protect 的地址必须是 PAGE_SIZE 对齐
    p = (z_int8 *)(((z_int64)p + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));

    c = p[666];         /* Read; ok */
    p[666] = 42;        /* Write; ok */

    // buffer 设置为只读
    if (z_mprotect(p, 1024, PROT_READ))// | PROT_WRITE))
    {
        z_perror("Couldn’t mprotect");
        return;
    }

    c = p[666];         /* Read; ok */
    p[666] = 42;        /* Write; program dies on SIGSEGV */

//    z_free(p);
    z_printf("test protect ok\n");
}
