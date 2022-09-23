//
//  z_signal.c
//  SystemCall
//
//  Created by NULL
//

#include "z_signal.h"
#include "z_syscall.h"
#include "z_error.h"

#define UC_FLAVOR       30

z_int32 z_sigreturn(z_void *uctx, z_int32 ctxstyle, z_void *token)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sigreturn), uctx, ctxstyle, token));
}

z_void z_sigtramp(union z_sigaction_u u, z_int32 sigstyle, z_int32 sig, struct z_siginfo *sinfo, z_void *uctx, z_void *token)
{
    z_int32 ctxstyle = UC_FLAVOR;
    u.__sa_sigaction(sig, sinfo, uctx);
    z_sigreturn(uctx, ctxstyle, token);
}

z_int32 z_sigaction(z_int32 signum, struct z_sigaction *act, struct z_sigaction *oldact)
{
    act->sa_tramp = z_sigtramp;
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sigaction), signum, act, oldact));
}

z_int32 z_kill(z_int32 pid, z_int32 sig)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_kill), pid, sig));
}

z_int32 z_sigaltstack(struct z_stack_t *ss, struct z_stack_t *oss)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sigaltstack), ss, oss));
}

z_int32 z_sigprocmask(z_int32 how, z_sigset_t *set, z_sigset_t *oset)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sigprocmask), set, oset));
}

