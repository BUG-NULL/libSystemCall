//
//  main.c
//  TestBoot
//
//  Created by NULL
//

#include <SystemCall/SystemCall.h>
#include "test_syscall.h"
#include "test_file.h"
#include "test_time.h"
#include "test_sem.h"
#include "test_mmap.h"
#include "test_signal.h"
#include "test_select.h"
#include "test_fork.h"
#include "test_protect.h"
#include "test_socket.h"
#include "test_sysctl.h"
#include "test_malloc.h"
#include "test_shm.h"
#include "test_termios.h"
#include "test_lock.h"

#define MAX_LISTEN_NUM      5
#define SEND_BUF_SIZE       128
#define RECV_BUF_SIZE       1024
#define SERVER_PORT         80

z_void test_pid_uid(z_void)
{
    z_printf("pid %d, ppid %d, uid %d\n", z_getpid(), z_getppid(), z_getuid());
}

z_void test_endian(z_void)
{
    z_printf("char %d, short %d, int %d, long %d, long int %d, int long %d, long long %d\n", sizeof(char), sizeof(short), sizeof(int), sizeof(long), sizeof(long int), sizeof(int long), sizeof(long long));

    union
    {
        z_uint32 i;
        z_uint8 a[4];
    } u;

    u.i = 1;
    if (u.a[0] == 1)
    {
        z_printf("little endian\n");
    }
    else
    {
        z_printf("big endian\n");
    }
}

z_void test_dl(z_void)
{
//    test_dlopen();
//    test_my_dlopen();
}

struct args
{
    z_int32 n;
};

struct z_ucontext uctx_main00;
struct z_ucontext uctx_func00;

z_void func00(z_void)
{
    z_printf("func00: started\n");
}

z_void test_coroutine_1(z_void)
{
    // = {0} 这个写法，编译器会在编译期间自动加上 memset 来清零
    z_int8 func00_stack[1024 * 1024]; // 131072

    z_memset(func00_stack, 0, 1024 * 1024);
    if (z_getcontext(&uctx_func00) == -1)
    {
        z_printf("getcontext error\n");
    }

    uctx_func00.uc_stack.ss_sp = func00_stack;
    uctx_func00.uc_stack.ss_size = sizeof(func00_stack);
    uctx_func00.uc_link = &uctx_main00;
    z_makecontext(&uctx_func00, func00, 0);

    z_printf("main: swapcontext(&uctx_main, &uctx_func00)\n");
    if (z_swapcontext(&uctx_main00, &uctx_func00) == -1)
    {
        z_printf("swapcontext error\n");
    }

    z_printf("main: exiting\n");
    return;
}

void stderr_tty_init(void);

z_void exit_func(z_void)
{
    z_printf("exit_func\n");
}

z_int32 z_main(z_int32 argc, z_int8 *argv[], z_int8 *envp[], z_int8 *xxx[])
{
    z_printf("Test start...\n");
    
    test_endian();
    test_file();
    test_pid_uid();
    test_time();
    test_fork();
    test_sem();
    test_mmap();
    test_sigaction();
    test_shm();
    test_select();
    test_sysctl();
    test_dl();
//    test_protect();
    test_socket();
    test_malloc();
    test_lock();
    test_cas();
    z_atexit(exit_func);
    
    z_printf("Test end...\n");
    return 0;
}
