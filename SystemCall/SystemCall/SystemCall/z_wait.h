//
//  z_wait.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_wait_h
#define z_wait_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_time.h>

/*
 * Option bits for the third argument of wait4.  WNOHANG causes the
 * wait to not hang if there are no stopped or terminated processes, rather
 * returning an error indication in this case (pid==0).  WUNTRACED
 * indicates that the caller should receive status about untraced children
 * which stop due to signals.  If children are stopped and a wait without
 * this option is done, it is as though they were still running... nothing
 * about them is returned.
 */
#define WNOHANG         0x00000001  /* [XSI] no hang in wait/no child to reap */
#define WUNTRACED       0x00000002  /* [XSI] notify on stop, untraced child */

/*
 * Macros to test the exit status returned by wait
 * and extract the relevant values.
 */
#if defined(_POSIX_C_SOURCE) && !defined(_DARWIN_C_SOURCE)
#define _W_INT(i)       (i)
#else
#define _W_INT(w)       (*(z_int32 *)&(w))  /* convert union wait to int */
#define WCOREFLAG       0200
#endif /* (_POSIX_C_SOURCE && !_DARWIN_C_SOURCE) */

/* These macros are permited, as they are in the implementation namespace */
#define _WSTATUS(x)     (_W_INT(x) & 0177)
#define _WSTOPPED       0177            /* _WSTATUS if process is stopped */

/*
 * [XSI] The <sys/wait.h> header shall define the following macros for
 * analysis of process status values
 */
#if __DARWIN_UNIX03
#define WEXITSTATUS(x)  ((_W_INT(x) >> 8) & 0x000000ff)
#else /* !__DARWIN_UNIX03 */
#define WEXITSTATUS(x)  (_W_INT(x) >> 8)
#endif /* !__DARWIN_UNIX03 */
/* 0x13 == SIGCONT */
#define WSTOPSIG(x)     (_W_INT(x) >> 8)
#define WIFCONTINUED(x) (_WSTATUS(x) == _WSTOPPED && WSTOPSIG(x) == 0x13)
#define WIFSTOPPED(x)   (_WSTATUS(x) == _WSTOPPED && WSTOPSIG(x) != 0x13)
#define WIFEXITED(x)    (_WSTATUS(x) == 0)
#define WIFSIGNALED(x)  (_WSTATUS(x) != _WSTOPPED && _WSTATUS(x) != 0)
#define WTERMSIG(x)     (_WSTATUS(x))
#if (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
#define WCOREDUMP(x)    (_W_INT(x) & WCOREFLAG)

#define W_EXITCODE(ret, sig)    ((ret) << 8 | (sig))
#define W_STOPCODE(sig)         ((sig) << 8 | _WSTOPPED)
#endif /* (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)) */

struct z_rusage
{
    struct z_timeval ru_utime;          /* user time used (PL) */
    struct z_timeval ru_stime;          /* system time used (PL) */
#if __DARWIN_C_LEVEL < __DARWIN_C_FULL
    z_int64     ru_opaque[14];          /* implementation defined */
#else
    z_int64    ru_maxrss;              /* max resident set size (PL) */
    z_int64    ru_ixrss;               /* integral shared memory size (NU) */
    z_int64    ru_idrss;               /* integral unshared data (NU)  */
    z_int64    ru_isrss;               /* integral unshared stack (NU) */
    z_int64    ru_minflt;              /* page reclaims (NU) */
    z_int64    ru_majflt;              /* page faults (NU) */
    z_int64    ru_nswap;               /* swaps (NU) */
    z_int64    ru_inblock;             /* block input operations (atomic) */
    z_int64    ru_oublock;             /* block output operations (atomic) */
    z_int64    ru_msgsnd;              /* messages sent (atomic) */
    z_int64    ru_msgrcv;              /* messages received (atomic) */
    z_int64    ru_nsignals;            /* signals received (atomic) */
    z_int64    ru_nvcsw;               /* voluntary context switches (atomic) */
    z_int64    ru_nivcsw;              /* involuntary " */
#endif  /* __DARWIN_C_LEVEL >= __DARWIN_C_FULL */
};

z_int32 z_wait4(z_int32 pid, z_int32 *status, z_int32 options, struct z_rusage *rusage);

#endif /* z_wait_h */
