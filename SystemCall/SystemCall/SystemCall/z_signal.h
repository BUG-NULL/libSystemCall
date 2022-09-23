//
//  z_signal.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_signal_h
#define z_signal_h

#include <SystemCall/z_types.h>

#define SIGHUP  1       /* hangup */
#define SIGINT  2       /* interrupt */
#define SIGQUIT 3       /* quit */
#define SIGILL  4       /* illegal instruction (not reset when caught) */
#define SIGTRAP 5       /* trace trap (not reset when caught) */
#define SIGABRT 6       /* abort() */
#if  (defined(_POSIX_C_SOURCE) && !defined(_DARWIN_C_SOURCE))
#define SIGPOLL 7       /* pollable event ([XSR] generated, not supported) */
#else   /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
#define SIGIOT  SIGABRT /* compatibility */
#define SIGEMT  7       /* EMT instruction */
#endif  /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
#define SIGFPE  8       /* floating point exception */
#define SIGKILL 9       /* kill (cannot be caught or ignored) */
#define SIGBUS  10      /* bus error */
#define SIGSEGV 11      /* segmentation violation */
#define SIGSYS  12      /* bad argument to system call */
#define SIGPIPE 13      /* write on a pipe with no one to read it */
#define SIGALRM 14      /* alarm clock */
#define SIGTERM 15      /* software termination signal from kill */
#define SIGURG  16      /* urgent condition on IO channel */
#define SIGSTOP 17      /* sendable stop signal not from tty */
#define SIGTSTP 18      /* stop signal from tty */
#define SIGCONT 19      /* continue a stopped process */
#define SIGCHLD 20      /* to parent on child stop or exit */
#define SIGTTIN 21      /* to readers pgrp upon background tty read */
#define SIGTTOU 22      /* like TTIN for output if (tp->t_local&LTOSTOP) */
#if  (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
#define SIGIO   23      /* input/output possible signal */
#endif
#define SIGXCPU 24      /* exceeded CPU time limit */
#define SIGXFSZ 25      /* exceeded file size limit */
#define SIGVTALRM 26    /* virtual time alarm */
#define SIGPROF 27      /* profiling time alarm */
#if  (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
#define SIGWINCH 28     /* window size changes */
#define SIGINFO 29      /* information request */
#endif
#define SIGUSR1 30      /* user defined signal 1 */
#define SIGUSR2 31      /* user defined signal 2 */

#define SIG_DFL         (z_void (*)(z_int32))0
#define SIG_IGN         (z_void (*)(z_int32))1
#define SIG_HOLD        (z_void (*)(z_int32))5
#define SIG_ERR         ((z_void (*)(z_int32))-1)

/*
 * Structure used in sigaltstack call.
 */

#define SS_ONSTACK      0x0001  /* take signal on signal stack */
#define SS_DISABLE      0x0004  /* disable taking signals on alternate stack */
#define MINSIGSTKSZ     32768   /* (32K)minimum allowable stack */
#define SIGSTKSZ        131072  /* (128K)recommended stack size */

/*
 * Flags for sigprocmask:
 */
#define SIG_BLOCK       1       /* block specified signal set */
#define SIG_UNBLOCK     2       /* unblock specified signal set */
#define SIG_SETMASK     3       /* set specified signal set */

union z_sigval
{
    z_int32 sival_int;
    z_void  *sival_ptr;
};

struct z_siginfo
{
    z_int32         si_signo;       /* signal number */
    z_int32         si_errno;       /* errno association */
    z_int32         si_code;        /* signal code */
    z_int32         si_pid;         /* sending process */
    z_uint32        si_uid;         /* sender's ruid */
    z_int32         si_status;      /* exit value */
    z_void          *si_addr;       /* faulting instruction */
    union z_sigval  si_value;       /* signal value */
    z_int64         si_band;        /* band event for SIGPOLL */
    z_uint64        __pad[7];       /* Reserved for Future Use */
};

union z_sigaction_u
{
    z_void (*__sa_handler)(z_int32);
    z_void (*__sa_sigaction)(z_int32, struct z_siginfo *, z_void *);
};

struct z_sigaction
{
    union z_sigaction_u __sigaction_u;  /* signal handler */
    z_void (*sa_tramp)(union z_sigaction_u, z_int32, z_int32, struct z_siginfo *, z_void *, z_void *);
    z_uint32            sa_mask;        /* signal mask to apply */
    z_int32             sa_flags;       /* see signal options below */
};

struct z_stack_t
{
    z_void      *ss_sp;         /* signal stack base */
    z_uint64    ss_size;        /* signal stack length */
    z_int32     ss_flags;       /* SA_DISABLE and/or SA_ONSTACK */
};

typedef z_uint32    z_sigset_t;

z_int32 z_sigaction(z_int32 signum, struct z_sigaction *act, struct z_sigaction *oldact);
z_int32 z_kill(z_int32 pid, z_int32 sig);
z_int32 z_sigaltstack(struct z_stack_t *ss, struct z_stack_t *oss);
z_int32 z_sigprocmask(z_int32 how, z_sigset_t *set, z_sigset_t *oset);

#endif /* z_signal_h */
