#include "z_syscall.h"
#include "z_types.h"
#include "z_error.h"

z_int64 zz_syscall(z_int64 n, z_int64 a1, z_int64 a2, z_int64 a3, z_int64 a4, z_int64 a5, z_int64 a6)
{
    z_int32 ret = 0;
    asm (
         "movq   %rdi, %rax;"
         "movq   %rsi, %rdi;"
         "movq   %rdx, %rsi;"
         "movq   %rcx, %rdx;"
         "movq   %r8, %rcx;"
         "movq   %r9, %r8;"
         "movq   16(%rsp), %r9;"
         "movq   %rcx, %r10;"
         "syscall;"
    );
    asm (
         "movl   %%eax, %0" : "=a" (ret)
    );
    return ret;
}












