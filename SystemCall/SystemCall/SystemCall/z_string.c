//
//  z_string.c
//  SystemCall
//
//  Created by NULL
//

#include "z_string.h"
#include "z_error.h"
#include "z_malloc.h"

#define NSIG            32
#define TMPSIZE         (19 + 1 + 2 + 1)
#define UPREFIX         "Unknown error"
#define EBUFSIZE        (20 + 2 + sizeof(UPREFIX))
#define TLOOP(s)        if (t) TLOOP1(s)
#define TLOOP1(s)       do { s; } while (--t)
#define wsize           sizeof(z_uint32)
#define wmask           (wsize - 1)

z_int8 *z_sys_siglist[NSIG] =
{
    "Signal 0",
    "Hangup",            /* SIGHUP */
    "Interrupt",            /* SIGINT */
    "Quit",                /* SIGQUIT */
    "Illegal instruction",        /* SIGILL */
    "Trace/BPT trap",        /* SIGTRAP */
    "Abort trap",            /* SIGABRT */
    "EMT trap",            /* SIGEMT */
    "Floating point exception",    /* SIGFPE */
    "Killed",            /* SIGKILL */
    "Bus error",            /* SIGBUS */
    "Segmentation fault",        /* SIGSEGV */
    "Bad system call",        /* SIGSYS */
    "Broken pipe",            /* SIGPIPE */
    "Alarm clock",            /* SIGALRM */
    "Terminated",            /* SIGTERM */
    "Urgent I/O condition",        /* SIGURG */
    "Suspended (signal)",        /* SIGSTOP */
    "Suspended",            /* SIGTSTP */
    "Continued",            /* SIGCONT */
    "Child exited",            /* SIGCHLD */
    "Stopped (tty input)",        /* SIGTTIN */
    "Stopped (tty output)",        /* SIGTTOU */
    "I/O possible",            /* SIGIO */
    "Cputime limit exceeded",    /* SIGXCPU */
    "Filesize limit exceeded",    /* SIGXFSZ */
    "Virtual timer expired",    /* SIGVTALRM */
    "Profiling timer expired",    /* SIGPROF */
    "Window size changes",        /* SIGWINCH */
    "Information request",        /* SIGINFO */
    "User defined signal 1",    /* SIGUSR1 */
    "User defined signal 2"        /* SIGUSR2 */
};

z_int32 z_strsignal_r(z_int32 num, z_int8 *strsignalbuf, z_int32 buflen)
{
    z_int32 retval = 0;
    z_int8 tmp[TMPSIZE];
    z_uint64 n = 0;
    z_int32 signum = 0;
    z_int8 *t, *p;

    z_memset(tmp, 0, TMPSIZE);
    signum = num;
    if (num < 0)
    {
        signum = -signum;
    }

    t = tmp;
    do {
        *t++ = "0123456789"[signum % 10];
    } while (signum /= 10);
    
    if (num < 0)
    {
        *t++ = '-';
    }
    
    z_int32 suffixlen = z_strlen(tmp) + 2;

    if (num > 0 && num < NSIG)
    {
        n = (z_uint64)z_strncpy(strsignalbuf, z_sys_siglist[num], buflen);
        if (n >= (buflen - suffixlen))
        {
            retval = ERANGE;
        }
    }
    else
    {
        n = (z_uint64)z_strncpy(strsignalbuf, UPREFIX, buflen);
        retval = EINVAL;
    }

    if (n < (buflen - suffixlen))
    {
        p = (strsignalbuf + n);
        *p++ = ':';
        *p++ = ' ';

        for (;;)
        {
            *p++ = *--t;
            if (t <= tmp)
                break;
        }
        *p = '\0';
    }

    return retval;
}

z_int8 *__strsignal_ebuf = Z_NULL;

z_int8 *z_strsignal(z_int32 num)
{
    if (__strsignal_ebuf == Z_NULL)
    {
        __strsignal_ebuf = z_malloc(NL_TEXTMAX);
        if (__strsignal_ebuf == Z_NULL)
        {
            return Z_NULL;
        }
    }

    if (z_strsignal_r(num, __strsignal_ebuf, EBUFSIZE))
    {
        z_errno = EINVAL;
    }

    return __strsignal_ebuf;
}

z_int8 *z_sys_errlist[] =
{
    "Undefined error: 0",            /*  0 - ENOERROR */
    "Operation not permitted",        /*  1 - EPERM */
    "No such file or directory",        /*  2 - ENOENT */
    "No such process",            /*  3 - ESRCH */
    "Interrupted system call",        /*  4 - EINTR */
    "Input/output error",            /*  5 - EIO */
    "Device not configured",        /*  6 - ENXIO */
    "Argument list too long",        /*  7 - E2BIG */
    "Exec format error",            /*  8 - ENOEXEC */
    "Bad file descriptor",            /*  9 - EBADF */
    "No child processes",            /* 10 - ECHILD */
    "Resource deadlock avoided",        /* 11 - EDEADLK */
    "Cannot allocate memory",        /* 12 - ENOMEM */
    "Permission denied",            /* 13 - EACCES */
    "Bad address",                /* 14 - EFAULT */
    "Block device required",        /* 15 - ENOTBLK */
    "Resource busy",            /* 16 - EBUSY */
    "File exists",                /* 17 - EEXIST */
    "Cross-device link",            /* 18 - EXDEV */
    "Operation not supported by device",    /* 19 - ENODEV */
    "Not a directory",            /* 20 - ENOTDIR */
    "Is a directory",            /* 21 - EISDIR */
    "Invalid argument",            /* 22 - EINVAL */
    "Too many open files in system",    /* 23 - ENFILE */
    "Too many open files",            /* 24 - EMFILE */
    "Inappropriate ioctl for device",    /* 25 - ENOTTY */
    "Text file busy",            /* 26 - ETXTBSY */
    "File too large",            /* 27 - EFBIG */
    "No space left on device",        /* 28 - ENOSPC */
    "Illegal seek",                /* 29 - ESPIPE */
    "Read-only file system",        /* 30 - EROFS */
    "Too many links",            /* 31 - EMLINK */
    "Broken pipe",                /* 32 - EPIPE */

/* math software */
    "Numerical argument out of domain",    /* 33 - EDOM */
    "Result too large",            /* 34 - ERANGE */

/* non-blocking and interrupt i/o */
    "Resource temporarily unavailable",    /* 35 - EAGAIN */
                        /* 35 - EWOULDBLOCK */
    "Operation now in progress",        /* 36 - EINPROGRESS */
    "Operation already in progress",    /* 37 - EALREADY */

/* ipc/network software -- argument errors */
    "Socket operation on non-socket",    /* 38 - ENOTSOCK */
    "Destination address required",        /* 39 - EDESTADDRREQ */
    "Message too long",            /* 40 - EMSGSIZE */
    "Protocol wrong type for socket",    /* 41 - EPROTOTYPE */
    "Protocol not available",        /* 42 - ENOPROTOOPT */
    "Protocol not supported",        /* 43 - EPROTONOSUPPORT */
    "Socket type not supported",        /* 44 - ESOCKTNOSUPPORT */
    "Operation not supported",        /* 45 - ENOTSUP */
    "Protocol family not supported",    /* 46 - EPFNOSUPPORT */
                        /* 47 - EAFNOSUPPORT */
    "Address family not supported by protocol family",
    "Address already in use",        /* 48 - EADDRINUSE */
    "Can't assign requested address",    /* 49 - EADDRNOTAVAIL */

/* ipc/network software -- operational errors */
    "Network is down",            /* 50 - ENETDOWN */
    "Network is unreachable",        /* 51 - ENETUNREACH */
    "Network dropped connection on reset",    /* 52 - ENETRESET */
    "Software caused connection abort",    /* 53 - ECONNABORTED */
    "Connection reset by peer",        /* 54 - ECONNRESET */
    "No buffer space available",        /* 55 - ENOBUFS */
    "Socket is already connected",        /* 56 - EISCONN */
    "Socket is not connected",        /* 57 - ENOTCONN */
    "Can't send after socket shutdown",    /* 58 - ESHUTDOWN */
    "Too many references: can't splice",    /* 59 - ETOOMANYREFS */
    "Operation timed out",            /* 60 - ETIMEDOUT */
    "Connection refused",            /* 61 - ECONNREFUSED */

    "Too many levels of symbolic links",    /* 62 - ELOOP */
    "File name too long",            /* 63 - ENAMETOOLONG */

/* should be rearranged */
    "Host is down",                /* 64 - EHOSTDOWN */
    "No route to host",            /* 65 - EHOSTUNREACH */
    "Directory not empty",            /* 66 - ENOTEMPTY */

/* quotas & mush */
    "Too many processes",            /* 67 - EPROCLIM */
    "Too many users",            /* 68 - EUSERS */
    "Disc quota exceeded",            /* 69 - EDQUOT */

/* Network File System */
    "Stale NFS file handle",        /* 70 - ESTALE */
    "Too many levels of remote in path",    /* 71 - EREMOTE */
    "RPC struct is bad",            /* 72 - EBADRPC */
    "RPC version wrong",            /* 73 - ERPCMISMATCH */
    "RPC prog. not avail",            /* 74 - EPROGUNAVAIL */
    "Program version wrong",        /* 75 - EPROGMISMATCH */
    "Bad procedure for program",        /* 76 - EPROCUNAVAIL */

    "No locks available",            /* 77 - ENOLCK */
    "Function not implemented",        /* 78 - ENOSYS */
    "Inappropriate file type or format",    /* 79 - EFTYPE */
    "Authentication error",            /* 80 - EAUTH */
    "Need authenticator",            /* 81 - ENEEDAUTH */

    "Device power is off",            /* 82 - EPWROFF */
    "Device error",                         /* 83 - EDEVERR */
    "Value too large to be stored in data type",    /* 84 - EOVERFLOW */

/* program loading errors */
    "Bad executable (or shared library)",   /* 85 - EBADEXEC */
    "Bad CPU type in executable",        /* 86 - EBADARCH */
    "Shared library version mismatch",    /* 87 - ESHLIBVERS */
    "Malformed Mach-o file",        /* 88 - EBADMACHO */
    "Operation canceled",            /* 89 - ECANCELED */
    "Identifier removed",            /* 90 - EIDRM */
    "No message of desired type",        /* 91 - ENOMSG */
    "Illegal byte sequence",        /* 92 - EILSEQ */
    "Attribute not found",            /* 93 - ENOATTR */
    "Bad message",                /* 94 - EBADMSG */
    "EMULTIHOP (Reserved)",            /* 95 - EMULTIHOP */
    "No message available on STREAM",    /* 96 - ENODATA */
    "ENOLINK (Reserved)",            /* 97 - ENOLINK */
    "No STREAM resources",            /* 98 - ENOSR */
    "Not a STREAM",                /* 99 - ENOSTR */
    "Protocol error",            /* 100 - EPROTO */
    "STREAM ioctl timeout",            /* 101 - ETIME */
    "Operation not supported on socket",    /* 102 - EOPNOTSUPP */
    "Policy not found",            /* 103 - ENOPOLICY */
    "State not recoverable",        /* 104 - ENOTRECOVERABLE */
    "Previous owner died",            /* 105 - EOWNERDEAD */

    "Interface output queue is full",    /* 106 - EQFULL */
};

z_int32 z_sys_nerr = sizeof(z_sys_errlist) / sizeof(z_sys_errlist[0]);
z_int8 *__strerror_ebuf = Z_NULL;

z_void z_errstr(z_int32 num, z_int8 *uprefix, z_int8 *buf, z_int32 len)
{
    z_int8 *t;
    z_uint8 uerr;
    z_int8 tmp[EBUFSIZE];

    t = tmp + sizeof(tmp);
    *--t = '\0';
    uerr = (num >= 0) ? num : -num;
    do {
        *--t = "0123456789"[uerr % 10];
    } while (uerr /= 10);
    
    if (num < 0)
        *--t = '-';
    *--t = ' ';
    *--t = ':';
    z_strncpy(buf, uprefix, len);
    z_strncat(buf, t, len);
}

z_int32 z_strerror_r(z_int32 errnum, z_int8 *strerrbuf, z_int32 buflen)
{
    z_int32 retval = 0;
    if (errnum < 0 || errnum >= z_sys_nerr)
    {
        z_errstr(errnum, UPREFIX, strerrbuf, buflen);
        retval = EINVAL;
    }
    else
    {
        if ((z_int64)z_strncpy(strerrbuf, z_sys_errlist[errnum], buflen) >= buflen)
            retval = ERANGE;
    }
    
    return (retval);
}

z_int8 *z_strerror(z_int32 num)
{
    if (num >= 0 && num < z_sys_nerr)
    {
        return (z_int8 *)z_sys_errlist[num];
    }
    
    if (__strerror_ebuf == Z_NULL)
    {
        __strerror_ebuf = z_malloc(NL_TEXTMAX);
        if (__strerror_ebuf == Z_NULL)
        {
            return Z_NULL;
        }
    }

    if (z_strerror_r(num, __strerror_ebuf, NL_TEXTMAX) != 0)
    {
        z_errno = EINVAL;
    }
    return __strerror_ebuf;
}

z_int32 z_strncmp(z_int8 *s1, z_int8 *s2, z_uint64 n)
{
    if (!n)
        return(0);
    
    while (--n && *s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    
    return (*s1 - *s2);
}

z_int8 *z_strncpy(z_int8 *dest, z_int8 *src, z_uint64 count)
{
    z_int8 *tmp = dest;

    while (count)
    {
        if ((*tmp = *src) != 0)
            src++;
        tmp++;
        count--;
    }
    
    return dest;
}

z_int8 *z_strncat(z_int8 *dest, z_int8 *str, z_uint64 n)
{
    z_int8 *cp = dest;
 
    while (*cp != '\0')
        ++cp;
 
    while (n && (*cp++ = *str++) != '\0')
    {
        --n;
    }
 
    return dest;
}

z_void *z_memset(z_void *s, z_int32 c, z_uint64 n)
{
    z_uint8 *p = s, *e = p + n;
    while (p < e)
        *p++ = c;
    return s;
}

z_void *z_memcpy(z_void *dest, z_void *src, z_uint64 n)
{
    z_uint8 *d = dest;
    z_uint8 *p = src, *e = p + n;
    while (p < e)
        *d++ = *p++;
    return dest;
}

z_int32 z_strlen(z_int8 *s)
{
    z_int8 *p = s;
    while (*p != '\0')
        p++;
    return (z_int32)(p - s);
}

z_int32 z_memcmp(z_void *s1, z_void *s2, z_int32 n)
{
    if (n != 0)
    {
        z_uint32 *p1 = s1, *p2 = s2;
        do {
            if (*p1++ != *p2++)
                return (*--p1 - *--p2);
        } while (--n != 0);
    }
    return 0;
}

z_int8 *z_strdup(z_int8 *str)
{
    z_int32 len;
    z_int8 *copy;

    len = z_strlen(str) + 1;
    if ((copy = z_malloc(len)) == Z_NULL)
        return Z_NULL;
    
    z_memcpy(copy, str, len);
    return copy;
}

z_void *z_memmove(z_void *dst0, z_void *src0, z_int32 length)
{
    z_int8 *dst = dst0;
    z_int8 *src = src0;
    z_uint64 t;

    if (length == 0 || dst == src)        /* nothing to do */
        goto done;
    
    if ((z_uint64)dst < (z_uint64)src)
    {
        /*
         * Copy forward.
         */
        t = (z_uint64)src;    /* only need low bits */
        if ((t | (z_uint64)dst) & wmask)
        {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (z_uint64)dst) & wmask || length < wsize)
            {
                t = length;
            }
            else
            {
                t = wsize - (t & wmask);
            }
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(z_int64 *)dst = *(z_int64 *)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    }
    else
    {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (z_uint64)src;
        if ((t | (z_uint64)dst) & wmask)
        {
            if ((t ^ (z_uint64)dst) & wmask || length <= wsize)
            {
                t = length;
            }
            else
            {
                t &= wmask;
            }
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(z_int64 *)dst = *(z_int64 *)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }
done:
    return dst0;
}
