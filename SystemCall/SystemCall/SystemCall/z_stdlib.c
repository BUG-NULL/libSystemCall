//
//  z_stdlib.c
//  SystemCall
//
//  Created by NULL
//

#include "z_stdlib.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_time.h"
#include "z_fcntl.h"
#include "z_unistd.h"
#include "z_ioctl.h"
#include "z_termios.h"
#include "z_stat.h"
#include "z_malloc.h"
#include "z_string.h"
#include "z_dirent.h"
#include "z_atexit.h"
#include "z_signal.h"

#define RAND_MAX            0x7fffffff
#define PTSNAME_MAX_SIZE    128
#define MAXNAMLEN           255
#define _PATH_DEV           "/dev/"

#define ISIZE sizeof(z_int32)
#define PSIZE sizeof(z_uint8 *)

#define ICOPY_LIST(src, dst, last)                \
    do                            \
    *(z_int32*)dst = *(z_int32*)src, src += ISIZE, dst += ISIZE;    \
    while(src < last)

#define ICOPY_ELT(src, dst, i)                    \
    do                            \
    *(z_int32*) dst = *(z_int32*) src, src += ISIZE, dst += ISIZE;    \
    while (i -= ISIZE)

#define CCOPY_LIST(src, dst, last)        \
    do                    \
        *dst++ = *src++;        \
    while (src < last)

#define CCOPY_ELT(src, dst, i)            \
    do                    \
        *dst++ = *src++;        \
    while (i -= 1)

#define EVAL(p) (z_uint8 **)                        \
    ((z_uint8 *)0 +                            \
        (((z_uint8 *)p + PSIZE - 1 - (z_uint8 *) 0) & ~(PSIZE - 1)))

#define swap(a, b) {        \
    s = b;                    \
    i = size;                \
    do {                    \
        tmp = *a; *a++ = *s; *s++ = tmp; \
    } while (--i);                \
    a -= size;                \
}

z_void (*z_cleanup)(z_void);

z_int32 _z_exit(z_int32 status)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_exit), status));
}

z_void z_exit(z_int32 status)
{
    z_cxa_finalize(Z_NULL);
    if (z_cleanup)
        (*z_cleanup)();
    _z_exit(status);
}

z_int8 *z_findenv(z_int8 *name, z_int64 *offset, z_int8 **environ)
{
    z_int64 len, i;
    z_int8 *np;
    z_int8 **p, *cp;

    if (name == Z_NULL || environ == Z_NULL)
        return Z_NULL;
    
    for (np = name; *np && *np != '='; ++np)
        continue;
    
    len = np - name;
    for (p = environ; (cp = *p) != Z_NULL; ++p)
    {
        for (np = name, i = len; i && *cp; i--)
        {
            if (*cp++ != *np++)
                break;
        }
        
        if (i == 0 && *cp++ == '=')
        {
            *offset = p - environ;
            return (cp);
        }
    }
    return Z_NULL;
}

z_int8 *z_getenv(z_int8 *name, z_int8 **environ)
{
    z_int64 offset;
    z_int8 *result;
    result = z_findenv(name, &offset, environ);
    return result;
}

z_uint64 next = 1;

z_void z_srand(z_uint32 seed)
{
    next = seed;
}

z_int32 z_do_rand(z_uint64 *ctx)
{
#ifdef  USE_WEAK_SEEDING
/*
 * Historic implementation compatibility.
 * The random sequences do not vary much with the seed,O
 * even with overflowing.
 */
    return ((*ctx = *ctx * 1103515245 + 12345) % ((z_uint64)RAND_MAX + 1));
#else   /* !USE_WEAK_SEEDING */
/*
 * Compute x = (7^5 * x) mod (2^31 - 1)
 * without overflowing 31 bits:
 *      (2^31 - 1) = 127773 * (7^5) + 2836
 * From "Random number generators: good ones are hard to find",
 * Park and Miller, Communications of the ACM, vol. 31, no. 10,
 * October 1988, p. 1195.
 */
    z_int64 hi, lo, x;

    /* Can't be initialized with 0, so use another value. */
    if (*ctx == 0)
        *ctx = 123459876;
    
    hi = *ctx / 127773;
    lo = *ctx % 127773;
    x = 16807 * lo - 2836 * hi;
    if (x < 0)
        x += 0x7fffffff;
    
    return ((*ctx = x) % ((z_uint64)RAND_MAX + 1));
#endif  /* !USE_WEAK_SEEDING */
}

z_int32 z_rand(z_void)
{
    return z_do_rand(&next);
}

z_int32 z_rand_r(z_uint32 *ctx)
{
    z_uint64 val = (z_uint64)*ctx;
    z_int32 r = z_do_rand(&val);
    *ctx = (z_uint32)val;
    return r;
}

z_void z_sranddev(z_void)
{
    z_int32 fd = 0, done = 0;
    
    fd = z_open("/dev/random", O_RDONLY | O_CLOEXEC, 0);
    if (fd >= 0)
    {
        if (z_read(fd, (void *) &next, sizeof(next)) == sizeof(next))
        {
            done = 1;
        }
        z_close(fd);
    }

    if (!done)
    {
        struct z_timeval tv;
        z_gettimeofday(&tv, Z_NULL);
        z_srand((z_uint32)((z_getpid() << 16) ^ tv.tv_sec ^ tv.tv_usec));
    }
}

z_int32 z_grantpt(z_int32 fd)
{
    return z_ioctl(fd, TIOCPTYGRANT);
}

z_int32 z_ptsname_r(z_int32 fd, z_int8 *buffer, z_int32 buflen)
{
    z_int32 error = 0;
    struct z_stat sbuf;
    z_int8 ptsnamebuf[PTSNAME_MAX_SIZE];
    z_int32 filefd = 0;
    
    if (!buffer)
    {
        z_errno = EINVAL;
        return -1;
    }

    error = z_ioctl(fd, TIOCPTYGNAME, ptsnamebuf);
    if (error)
    {
        return -1;
    }

    /*
     * XXX TSD
     *
     * POSIX: Handle device rename test case, which is expected
     * to fail if the pty has been renamed.
     */
    filefd = z_open(ptsnamebuf, O_RDONLY | O_CLOEXEC, 0);
    error = z_fstat(filefd, &sbuf);
    if (error)
    {
        return -1;
    }

    z_int32 len = z_strlen(ptsnamebuf) + 1;
    if (buflen < len)
    {
        z_errno = ERANGE;
        return -1;
    }

    z_memcpy(buffer, ptsnamebuf, len);
    return 0;
}

z_int8 *z_ptsname(z_int32 fd)
{
    z_int8 *ptsnamebuf = z_malloc(PTSNAME_MAX_SIZE);
    z_int32 error = z_ptsname_r(fd, ptsnamebuf, PTSNAME_MAX_SIZE);
    return error ? Z_NULL : ptsnamebuf;
}

z_int32 z_unlockpt(z_int32 fd)
{
    return z_ioctl(fd, TIOCPTYUNLK);
}

z_int32 z_posix_openpt(z_int32 flags)
{
    z_int32 fd = z_open("/dev/ptmx", flags, 0);
    if (fd >= 0)
    {
        return fd;
    }
    return -1;
}

z_void z_insertionsort(z_uint8 *a, z_int32 n, z_int32 size, z_int32 (*cmp)(z_void *, z_void *))
{
    z_uint8 *ai, *s, *t, *u, tmp;
    z_int32 i;

    for (ai = a + size; --n >= 1; ai += size)
        for (t = ai; t > a; t -= size)
        {
            u = t - size;
            if (cmp(u, t) <= 0)
                break;
            swap(u, t);
        }
}

z_void z_setup(z_uint8 *list1, z_uint8 *list2, z_int32 n, z_int32 size, z_int32 (*cmp)(z_void *, z_void *))
{
    z_int32 i, size2;
    z_int32 tmp;
    z_uint8 *f1, *s, *last, *p2;

    size2 = size * 2;
    if (n <= 5)
    {
        z_insertionsort(list1, n, size, cmp);
        *EVAL(list2) = (z_uint8 *) list2 + n*size;
        return;
    }
    /*
     * Avoid running pointers out of bounds; limit n to evens
     * for simplicity.
     */
    i = 4 + (n & 1);
    z_insertionsort(list1 + (n - i) * size, i, size, cmp);
    last = list1 + size * (n - i);
    *EVAL(list2 + (last - list1)) = list2 + n * size;

#ifdef NATURAL
    p2 = list2;
    f1 = list1;
    sense = (cmp(f1, f1 + size) > 0);
    for (; f1 < last; sense = !sense)
    {
        length = 2;
                    /* Find pairs with same sense. */
        for (f2 = f1 + size2; f2 < last; f2 += size2)
        {
            if ((cmp(f2, f2+ size) > 0) != sense)
                break;
            length += 2;
        }
        if (length < THRESHOLD)
        {
            /* Pairwise merge */
            do {
                p2 = *EVAL(p2) = f1 + size2 - list1 + list2;
                if (sense > 0)
                    swap(f1, f1 + size);
            } while ((f1 += size2) < f2);
        }
        else
        {
            /* Natural merge */
            l2 = f2;
            for (f2 = f1 + size2; f2 < l2; f2 += size2)
            {
                if ((cmp(f2-size, f2) > 0) != sense)
                {
                    p2 = *EVAL(p2) = f2 - list1 + list2;
                    if (sense > 0)
                        reverse(f1, f2-size);
                    f1 = f2;
                }
            }
            if (sense > 0)
                reverse (f1, f2-size);
            f1 = f2;
            if (f2 < last || cmp(f2 - size, f2) > 0)
                p2 = *EVAL(p2) = f2 - list1 + list2;
            else
                p2 = *EVAL(p2) = list2 + n*size;
        }
    }
#else        /* pairwise merge only. */
    for (f1 = list1, p2 = list2; f1 < last; f1 += size2)
    {
        p2 = *EVAL(p2) = p2 + size2;
        if (cmp(f1, f1 + size) > 0)
            swap(f1, f1 + size);
    }
#endif /* NATURAL */
}

z_int32 z_mergesort(z_void *base, z_int32 nmemb, z_int32 size, z_int32 (*cmp)(z_void *, z_void *))
{
    z_int32 i;
    z_int32 sense;
    z_int32 big, iflag;
    z_uint8 *f1, *f2, *t, *b, *tp2, *q, *l1, *l2;
    z_uint8 *list2, *list1, *p2, *p, *last, **p1;

    if (size < PSIZE / 2)
    {
        /* Pointers must fit into 2 * size. */
        z_errno = EINVAL;
        return (-1);
    }

    if (nmemb == 0)
        return (0);

    /*
     * XXX
     * Stupid subtraction for the Cray.
     */
    iflag = 0;
    if (!(size % ISIZE) && !(((z_int8 *)base - (z_int8 *)0) % ISIZE))
        iflag = 1;

    if ((list2 = z_malloc(nmemb * size + PSIZE)) == Z_NULL)
        return (-1);

    list1 = base;
    z_setup(list1, list2, nmemb, size, cmp);
    last = list2 + nmemb * size;
    i = big = 0;
    while (*EVAL(list2) != last)
    {
        l2 = list1;
        p1 = EVAL(list1);
        for (tp2 = p2 = list2; p2 != last; p1 = EVAL(l2))
        {
            p2 = *EVAL(p2);
            f1 = l2;
            f2 = l1 = list1 + (p2 - list2);
            if (p2 != last)
                p2 = *EVAL(p2);
            l2 = list1 + (p2 - list2);
            while (f1 < l1 && f2 < l2)
            {
                if ((*cmp)(f1, f2) <= 0)
                {
                    q = f2;
                    b = f1;
                    t = l1;
                    sense = -1;
                }
                else
                {
                    q = f1;
                    b = f2;
                    t = l2;
                    sense = 0;
                }
                if (!big)
                {    /* here i = 0 */
                    while ((b += size) < t && cmp(q, b) >sense)
                        if (++i == 6)
                        {
                            big = 1;
                            goto EXPONENTIAL;
                        }
                }
                else
                {
EXPONENTIAL:        for (i = size; ; i <<= 1)
                        if ((p = (b + i)) >= t)
                        {
                            if ((p = t - size) > b &&
                            (*cmp)(q, p) <= sense)
                                t = p;
                            else
                                b = p;
                            break;
                        }
                        else if ((*cmp)(q, p) <= sense)
                        {
                            t = p;
                            if (i == size)
                                big = 0;
                            goto FASTCASE;
                        }
                        else
                            b = p;
                    
                    while (t > b + size)
                    {
                        i = (z_int32)((((t - b) / size) >> 1) * size);
                        if ((*cmp)(q, p = b + i) <= sense)
                            t = p;
                        else
                            b = p;
                    }
                    goto COPY;
FASTCASE:           while (i > size)
                        if ((*cmp)(q, p = b + (i >>= 1)) <= sense)
                            t = p;
                        else
                            b = p;
COPY:                    b = t;
                }
                i = size;
                if (q == f1)
                {
                    if (iflag)
                    {
                        ICOPY_LIST(f2, tp2, b);
                        ICOPY_ELT(f1, tp2, i);
                    }
                    else
                    {
                        CCOPY_LIST(f2, tp2, b);
                        CCOPY_ELT(f1, tp2, i);
                    }
                }
                else
                {
                    if (iflag)
                    {
                        ICOPY_LIST(f1, tp2, b);
                        ICOPY_ELT(f2, tp2, i);
                    }
                    else
                    {
                        CCOPY_LIST(f1, tp2, b);
                        CCOPY_ELT(f2, tp2, i);
                    }
                }
            }
            if (f2 < l2)
            {
                if (iflag)
                    ICOPY_LIST(f2, tp2, l2);
                else
                    CCOPY_LIST(f2, tp2, l2);
            }
            else if (f1 < l1)
            {
                if (iflag)
                    ICOPY_LIST(f1, tp2, l1);
                else
                    CCOPY_LIST(f1, tp2, l1);
            }
            *p1 = l2;
        }
        tp2 = list1;    /* swap list1, list2 */
        list1 = list2;
        list2 = tp2;
        last = list2 + nmemb*size;
    }
    if (base == list2)
    {
        z_memmove(list2, list1, nmemb*size);
        list2 = list1;
    }
    z_free(list2, 0);
    return (0);
}

z_int8 *z_devname_r(z_int32 dev, z_uint16 type, z_int8 *buf, z_int32 len)
{
    register struct z_dir *dp;
    register struct z_dirent *dirp;
    struct z_stat sb;
    z_int8 _buf[sizeof(_PATH_DEV) + MAXNAMLEN];

    z_strncpy(_buf, _PATH_DEV, z_strlen(_PATH_DEV));

    if ((dp = z_opendir(_PATH_DEV)) == Z_NULL)
        return (Z_NULL);

    while ((dirp = z_readdir(dp)))
    {
        z_memmove(_buf + sizeof(_PATH_DEV) - 1, dirp->d_name, dirp->d_namlen + 1);
        if (z_lstat(_buf, &sb))
            continue;
        if (dev != sb.st_rdev)
            continue;
        if (type != (sb.st_mode & S_IFMT))
            continue;
        if (dirp->d_namlen + 1 > len)
            break;
        z_strncpy(buf, dirp->d_name, DIRENT_MAXPATHLEN);
        (z_void)z_closedir(dp);
        return (buf);
    }
    (z_void)z_closedir(dp);
    return (Z_NULL);
}

z_int8 *z_devname(z_int32 dev, z_uint16 type)
{
    static z_int8 *buf = Z_NULL;

    if (buf == Z_NULL)
    {
        buf = z_malloc(MAXNAMLEN);
        if (buf == Z_NULL)
            return Z_NULL;
    }

    return z_devname_r(dev, type, buf, MAXNAMLEN);
}

z_void z_abort(z_void)
{
    z_kill(z_getpid(), SIGABRT);
    z_exit(1);
}
