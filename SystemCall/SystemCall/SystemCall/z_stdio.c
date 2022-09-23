//
//  z_stdio.c
//  SystemCall
//
//  Created by NULL
//

#include "z_stdio.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_unistd.h"
#include "z_string.h"
#include "z_utils.h"
#include <stdarg.h>
#include "z_malloc.h"
#include "z_fcntl.h"
#include "z_stat.h"

#define OUTBUFSIZE          128
#define UPREFIX             "Unknown error"
#define EBUFSIZE            (20 + 2 + sizeof(UPREFIX))
#define COUNT               0

#define _PTHREAD_ERRORCHECK_MUTEX_SIG_init  0x32AAABA1
#define _PTHREAD_RECURSIVE_MUTEX_SIG_init   0x32AAABA2
#define _PTHREAD_FIRSTFIT_MUTEX_SIG_init    0x32AAABA3

#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER {_PTHREAD_RECURSIVE_MUTEX_SIG_init, {0}}
#define __sFXInit3 {.fl_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER, .counted = 1}

#define z_stdin     _z_stdinp
#define z_stdout    _z_stdoutp
#define z_stderr    _z_stderrp

#define _z_sgetc(p) (--(p)->_r < 0 ? _z_srget(p) : (z_int32)(*(p)->_p++))

z_int32 _z_sclose(z_void *cookie);
z_int32 _z_sread(z_void *cookie, z_int8 *buf, z_int32 n);
z_int32 _z_swrite(z_void *cookie, z_int8 *buf, z_int32 n);
z_int64 _z_sseek(z_void *cookie, z_int64 offset, z_int32 whence);

z_int8 __puts_null__[] = "(null)";
struct z_sfilex _z_sFX[3] = {__sFXInit3, __sFXInit3, __sFXInit3};
struct z_file _z_sF[3] =
{
    {
        ._flags = (__SRD),
        ._file = (STDIN_FILENO),
        ._cookie = _z_sF + (STDIN_FILENO),
        ._close = _z_sclose,
        ._read = _z_sread,
        ._seek = _z_sseek,
        ._write = _z_swrite,
        ._extra = _z_sFX + STDIN_FILENO,
    },
    {
        ._flags = (__SWR),
        ._file = (STDOUT_FILENO),
        ._cookie = _z_sF + (STDOUT_FILENO),
        ._close = _z_sclose,
        ._read = _z_sread,
        ._seek = _z_sseek,
        ._write = _z_swrite,
        ._extra = _z_sFX + STDOUT_FILENO,
    },
    {
        ._flags = (__SWR | __SNBF),
        ._file = (STDERR_FILENO),
        ._cookie = _z_sF + (STDERR_FILENO),
        ._close = _z_sclose,
        ._read = _z_sread,
        ._seek = _z_sseek,
        ._write = _z_swrite,
        ._extra = _z_sFX + STDERR_FILENO,
    }
};

struct z_file *_z_stdinp = &_z_sF[0];
struct z_file *_z_stdoutp = &_z_sF[1];
struct z_file *_z_stderrp = &_z_sF[2];

z_int32 lastfd = -1;
z_int8 outbuf[OUTBUFSIZE];
z_int8 *outptr;

z_void kprintn(z_int32, z_uint64, z_int32);
z_void kdoprnt(z_int32, z_int8 *, va_list);
z_void z_flushbuf(z_void);
z_void putcharfd(z_int32, z_int32);

z_void putcharfd(z_int32 c, z_int32 fd)
{
    z_int8 b = c;
    z_int32 len;

    if (fd != lastfd)
    {
        z_flushbuf();
        lastfd = fd;
    }
    *outptr++ = b;
    len = (z_int32)(outptr - outbuf);
    if ((len >= OUTBUFSIZE) || (b == '\n') || (b == '\r'))
    {
        z_flushbuf();
    }
}

z_void z_flushbuf()
{
    z_int32 len = (z_int32)(outptr - outbuf);
    if (len != 0)
    {
        if (lastfd != -1)
            z_write(lastfd, outbuf, len);
        outptr = outbuf;
    }
}

z_void z_printf(z_int8 *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    kdoprnt(STDERR_FILENO, fmt, ap);
    va_end(ap);
}

z_void z_vprintf(z_int8 *fmt, va_list ap)
{
    kdoprnt(STDERR_FILENO, fmt, ap);
}

z_void z_fdprintf(z_int32 fd, z_int8 *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    kdoprnt(fd, fmt, ap);
    va_end(ap);
}

z_void z_vfdprintf(z_int32 fd, z_int8 *fmt, va_list ap)
{
    kdoprnt(fd, fmt, ap);
}

z_void kdoprnt(z_int32 fd, z_int8 *fmt, va_list ap)
{
    z_uint64 ul;
    z_int32 lflag, ch;
    z_int8 *p;
    z_int32 init = 0;

    if (!init)
    {
        outptr = outbuf;
        init = 1;
    }

    for (;;)
    {
        while ((ch = *fmt++) != '%')
        {
            if (ch == '\0')
                return;
            putcharfd(ch, fd);
        }
        lflag = 0;
reswitch:
        switch (ch = *fmt++)
        {
        case 'l':
            lflag = 1;
            goto reswitch;
        case 'c':
            ch = va_arg(ap, z_int32);
            putcharfd(ch & 0x7f, fd);
            break;
        case 's':
            p = va_arg(ap, z_int8 *);
            while ((ch = *p++))
                putcharfd(ch, fd);
            break;
        case 'd':
            ul = lflag ? va_arg(ap, z_int64) : va_arg(ap, z_int32);
            if ((z_int64)ul < 0)
            {
                putcharfd('-', fd);
                ul = -(z_int64)ul;
            }
            kprintn(fd, ul, 10);
            break;
        case 'o':
            ul = lflag ? va_arg(ap, z_uint64) : va_arg(ap, z_uint32);
            kprintn(fd, ul, 8);
            break;
        case 'u':
            ul = lflag ? va_arg(ap, z_uint64) : va_arg(ap, z_uint32);
            kprintn(fd, ul, 10);
            break;
        case 'p':
            putcharfd('0', fd);
            putcharfd('x', fd);
            lflag += sizeof(z_void *) == sizeof(z_uint64) ? 1 : 0;
        case 'x':
            ul = lflag ? va_arg(ap, z_uint64) : va_arg(ap, z_uint32);
            kprintn(fd, ul, 16);
            break;
        case 'X':
        {
            z_int32 l;

            ul = lflag ? va_arg(ap, z_uint64) : va_arg(ap, z_uint32);
            if (lflag)
                l = (sizeof(z_uint64) * 8) - 4;
            else
                l = (sizeof(z_uint32) * 8) - 4;
            while (l >= 0)
            {
                putcharfd("0123456789abcdef"[(ul >> l) & 0xf], fd);
                l -= 4;
            }
            break;
        }
        default:
            putcharfd('%', fd);
            if (lflag)
                putcharfd('l', fd);
            putcharfd(ch, fd);
        }
    }
//    z_flushbuf();
}

z_void kprintn(z_int32 fd, z_uint64 ul, z_int32 base)
{
    z_int8 *p, buf[(sizeof(z_int64) * 8 / 3) + 1];

    p = buf;
    do
    {
        *p++ = "0123456789abcdef"[ul % base];
    } while (ul /= base);
    
    do
    {
        putcharfd(*--p, fd);
    } while (p > buf);
}

z_void z_perror(z_int8 *str)
{
    z_int8 *error = z_strerror((z_int32)z_errno);
    z_printf("%s: %s\n", str, error);
}

z_int32 z_sprintf(z_int8 *str, z_int8 *fmt, ...)
{
    z_int32 count = 0;
    z_int8 c;
    z_int8 *s;
    z_int32 n;
    
    z_int32 index = 0;
    z_int32 ret = 2;
    
    z_int8 buf[65];
    z_int8 digit[16];
    z_int32 num = 0;
    z_int32 len = 0;
    
    z_memset(buf, 0, sizeof(buf));
    z_memset(digit, 0, sizeof(digit));

    va_list ap;
    va_start(ap, fmt);
    
    while (*fmt != '\0')
    {
//        z_printf("*fmt = [%c]\n", *fmt);
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
                case 'd':
                {
                    n = va_arg(ap, z_int32);
                    if (n < 0)
                    {
                        *str = '-';
                        str++;
                        n = -n;
                    }
//                    z_printf("case d n = [%d]\n", n);
                    z_itoa(n, buf);
//                    z_printf("case d buf = [%s]\n", buf);
                    z_memcpy(str, buf, z_strlen(buf));
                    str += z_strlen(buf);
                    break;
                }
                case 'c':
                {
                    c = va_arg(ap, z_int32);
                    *str = c;
                    str++;
                    break;
                }
                case 'x':
                {
                    n = va_arg(ap, z_int32);
                    z_xtoa(n, buf);
                    z_memcpy(str, buf, z_strlen(buf));
                    str += z_strlen(buf);
                    break;
                }
                case 's':
                {
                    s = va_arg(ap, z_int8 *);
                    z_memcpy(str, s, z_strlen(s));
                    str += z_strlen(s);
                    break;
                }
                case '%':
                {
                    *str = '%';
                    str++;
                    break;
                }
                case '0':
                {
                    index = 0;
                    num = 0;
                    z_memset(digit, 0, sizeof(digit));
                    
                    while (1)
                    {
                        fmt++;
                        ret = z_isdigit(*fmt);
                        if (ret == 1)
                        {
                            digit[index] = *fmt;
                            index++;
                        }
                        else
                        {
                            num = z_atoi(digit);
                            break;
                        }
                    }
                    switch(*fmt)
                    {
                        case 'd':
                        {
                            n = va_arg(ap, z_int32);
                            if (n < 0)
                            {
                                *str = '-';
                                str++;
                                n = -n;
                            }
                            
                            z_itoa(n, buf);
                            len = z_strlen(buf);
                            if (len >= num)
                            {
                                z_memcpy(str, buf, z_strlen(buf));
                                str += z_strlen(buf);
                            }
                            else
                            {
                                z_memset(str, '0', num - len);
                                str += num - len;
                                z_memcpy(str, buf, z_strlen(buf));
                                str += z_strlen(buf);
                            }
                            break;
                        }
                        case 'x':
                        {
                            n = va_arg(ap, z_int32);
                            z_xtoa(n, buf);
                            len = z_strlen(buf);
                            if (len >= num)
                            {
                                z_memcpy(str, buf, len);
                                str += len;
                            }
                            else
                            {
                                z_memset(str, '0', num - len);
                                str += num - len;
                                z_memcpy(str, buf, len);
                                str += len;
                            }
                            break;
                        }
                        case 's':
                        {
                            s = va_arg(ap, z_int8 *);
                            len = z_strlen(s);
                            if (len >= num)
                            {
                                z_memcpy(str, s, z_strlen(s));
                                str += z_strlen(s);
                            }
                            else
                            {
                                z_memset(str, '0', num - len);
                                str += num - len;
                                z_memcpy(str, s, z_strlen(s));
                                str += z_strlen(s);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
                default:
                    break;
            }
        }
        else
        {
            *str = *fmt;
            str++;
            if (*fmt == '\n')
            {
                    
            }
        }
        fmt++;
    }

    va_end(ap);
    return count;
}

z_int32 _z_sclose(z_void *cookie)
{
    return z_close(((struct z_file *)cookie)->_file);
}

z_int32 _z_sread(z_void *cookie, z_int8 *buf, z_int32 n)
{
    struct z_file *fp = cookie;
    return z_read(fp->_file, buf, (z_int32)n);
}

z_int32 _z_swrite(z_void *cookie, z_int8 *buf, z_int32 n)
{
    struct z_file *fp = cookie;
    return z_write(fp->_file, buf, (z_int32)n);
}

z_int64 _z_sseek(z_void *cookie, z_int64 offset, z_int32 whence)
{
    struct z_file *fp = cookie;
    return z_lseek(fp->_file, (z_int64)offset, whence);
}

z_int32 z_sread(struct z_file *fp, z_int8 *buf, z_int32 n)
{
    z_int32 ret = 0;

    ret = (*fp->_read)(fp->_cookie, buf, n);
    if (ret > 0)
    {
        if (fp->_flags & __SOFF)
        {
            if (fp->_offset <= OFF_MAX - ret)
            {
                fp->_offset += ret;
            }
            else
            {
                fp->_flags &= ~__SOFF;
            }
        }
    }
    else if (ret < 0)
    {
        fp->_flags &= ~__SOFF;
    }
    
    return ret;
}

z_int64 z_sseek(struct z_file *fp, z_int64 offset, z_int32 whence)
{
    z_int64 ret;
    z_int64 serrno, errret;

    serrno = z_errno;
    z_errno = 0;
    ret = (*fp->_seek)(fp->_cookie, offset, whence);
    errret = z_errno;
    if (z_errno == 0)
        z_errno = serrno;
    /*
     * Disallow negative seeks per POSIX.
     * It is needed here to help upper level caller
     * in the cases it can't detect.
     */
    if (ret < 0)
    {
        if (errret == 0)
        {
            if (offset != 0 || whence != SEEK_CUR)
            {
                fp->_p = fp->_bf._base;
                fp->_r = 0;
                fp->_flags &= ~__SEOF;
            }
            fp->_flags |= __SERR;
            z_errno = EINVAL;
        }
        else if (errret == ESPIPE)
        {
            fp->_flags &= ~__SAPP;
        }
        fp->_flags &= ~__SOFF;
        ret = -1;
    }
    else if (fp->_flags & __SOPT)
    {
        fp->_flags |= __SOFF;
        fp->_offset = ret;
    }
    return ret;
}

z_int32 z_swrite(struct z_file *fp, z_int8 *buf, z_int32 n)
{
    z_int32 ret;
    z_int64 serrno;

    if (fp->_flags & __SAPP)
    {
        serrno = z_errno;
        if (z_sseek(fp, (z_int64)0, SEEK_END) == -1 && (fp->_flags & __SOPT))
        {
            return -1;
        }
        z_errno = serrno;
    }
    ret = (*fp->_write)(fp->_cookie, buf, n);
    /* __SOFF removed even on success in case O_APPEND mode is set. */
    if (ret >= 0)
    {
        if ((fp->_flags & (__SAPP | __SOFF)) == (__SAPP | __SOFF)
            && fp->_offset <= OFF_MAX - ret)
        {
            fp->_offset += ret;
        }
        else
        {
            fp->_flags &= ~__SOFF;
        }

    }
    else if (ret < 0)
    {
        fp->_flags &= ~__SOFF;
    }
    return ret;
}

z_int32 z_sflush(struct z_file *fp)
{
    z_uint8 *p;
    z_uint64 n, t;

    t = fp->_flags;
    if ((p = fp->_bf._base) == Z_NULL)
    {
        return 0;
    }

    /*
     * SUSv3 requires that fflush() on a seekable input stream updates the file
     * position indicator with the underlying seek function.  Use a dumb fseek
     * for this (don't attempt to preserve the buffers).
     */
    if ((t & __SRD) != 0)
    {
        if (fp->_seek == Z_NULL)
        {
            /*
             * No way to seek this file -- just return "success."
             */
            return 0;
        }

        n = fp->_r;
        if (n > 0)
        {
            /*
             * See _fseeko's dumb path.
             */
            if (_z_sseek(fp, (z_int64)-n, SEEK_CUR) == -1)
            {
                if (z_errno == ESPIPE)
                {
                    /*
                     * Ignore ESPIPE errors, since there's no way to put the bytes
                     * back into the pipe.
                     */
                    return 0;
                }
                return -1;
            }

            fp->_p = fp->_bf._base;
            fp->_r = 0;
            fp->_flags &= ~__SEOF;
        }
        return 0;
    }

    if ((t & __SWR) != 0)
    {
        n = fp->_p - p;        /* write this much */
        /*
         * Set these immediately to avoid problems with longjmp and to allow
         * exchange buffering (via setvbuf) in user write function.
         */
        fp->_p = p;
        fp->_w = t & (__SLBF | __SNBF) ? 0 : fp->_bf._size;

        for (; n > 0; n -= t, p += t)
        {
            t = z_swrite(fp, (z_int8 *)p, (z_int32)n);
            if (t <= 0)
            {
                /* 5340694: reset _p and _w on EAGAIN */
                if (t < 0 && z_errno == EAGAIN)
                {
                    if (p > fp->_p)
                    {
                        /* some was written */
                        z_memmove(fp->_p, p, (z_int32)n);
                        fp->_p += n;
                        if (!(fp->_flags & (__SLBF | __SNBF)))
                        {
                            fp->_w -= n;
                        }
                    }
                }
                fp->_flags |= __SERR;
                return -1;
            }
        }
    }
    return 0;
}

z_int32 z_lflush(struct z_file *fp)
{
    z_int32 ret = 0;

    if ((fp->_flags & (__SLBF | __SWR)) == (__SLBF | __SWR))
    {
        ret = z_sflush(fp);
    }
    return ret;
}

z_int32 _z_srefill0(struct z_file *fp)
{
    /* make sure stdio is set up */
    fp->_r = 0;        /* largely a convenience for callers */

    /* SysV does not make this test; take it out for compatibility */
    if (fp->_flags & __SEOF)
    {
        return -1;
    }

    /* if not already reading, have to be reading and writing */
    if ((fp->_flags & __SRD) == 0)
    {
        if ((fp->_flags & __SRW) == 0)
        {
            z_errno = EBADF;
            fp->_flags |= __SERR;
            return -1;
        }
        /* switch to reading */
        if (fp->_flags & __SWR)
        {
            if (z_sflush(fp))
            {
                return -1;
            }
            fp->_flags &= ~__SWR;
            fp->_w = 0;
            fp->_lbfsize = 0;
        }
        fp->_flags |= __SRD;
    }
    else
    {

    }

    /*
     * Before reading from a line buffered or unbuffered file,
     * flush all line buffered output files, per the ANSI C
     * standard.
     */
    if (fp->_flags & (__SLBF | __SNBF))
    {
        /* Now flush this file without locking it. */
        if ((fp->_flags & (__SLBF | __SWR)) == (__SLBF | __SWR))
        {
            z_sflush(fp);
        }
    }
    return 1;
}

z_int32 _z_srefill1(struct z_file *fp)
{
    fp->_p = fp->_bf._base;
    fp->_r = z_sread(fp, (z_int8 *)fp->_p, fp->_bf._size);
    fp->_flags &= ~__SMOD;    /* buffer contents are again pristine */
    if (fp->_r <= 0)
    {
        if (fp->_r == 0)
        {
            fp->_flags |= __SEOF;
        }
        else
        {
            fp->_r = 0;
            fp->_flags |= __SERR;
        }
        return -1;
    }
    return 0;
}

z_int32 _z_srefill(struct z_file *fp)
{
    z_int32 ret = 0;

    if ((ret = _z_srefill0(fp)) <= 0)
    {
        return ret;
    }
    return _z_srefill1(fp);
}

z_int32 _z_srget(struct z_file *fp)
{
    if (_z_srefill(fp) == 0)
    {
        fp->_r--;
        return (*fp->_p++);
    }
    return -1;
}

z_int32 z_getchar(z_void)
{
    z_int32 retval;
    retval = _z_sgetc(z_stdin);
    return retval;
}

z_int32 z_getc(struct z_file *fp)
{
    z_int32 retval;
    retval = _z_sgetc(fp);
    return retval;
}

z_int32 _z_fflush(struct z_file *fp)
{
    z_int32 retval;

//    if (fp == Z_NULL)
//        return (z_fwalk(sflush_locked));
    
    if ((fp->_flags & (__SWR | __SRW)) == 0)
    {
        retval = 0;
    }
    else
    {
        retval = z_sflush(fp);
    }
    return retval;
}

z_int32 _z_swbuf(z_int32 c, struct z_file *fp)
{
    z_uint64 n;

    /*
     * In case we cannot write, or longjmp takes us out early,
     * make sure _w is 0 (if fully- or un-buffered) or -_bf._size
     * (if line buffered) so that we will get called again.
     * If we did not do this, a sufficient number of putc()
     * calls might wrap _w from negative to positive.
     */
    fp->_w = fp->_lbfsize;

    c = (z_uint8)c;
    /*
     * If it is completely full, flush it out.  Then, in any case,
     * stuff c into the buffer.  If this causes the buffer to fill
     * completely, or if c is '\n' and the file is line buffered,
     * flush it (perhaps a second time).  The second flush will always
     * happen on unbuffered streams, where _bf._size==1; fflush()
     * guarantees that putc() will always call wbuf() by setting _w
     * to 0, so we need not do anything else.
     */
    n = fp->_p - fp->_bf._base;
    if (n >= fp->_bf._size)
    {
        if (_z_fflush(fp))
            return EOF;
        n = 0;
    }
    fp->_w--;
    *fp->_p++ = c;
    if (++n == fp->_bf._size || (fp->_flags & __SLBF && c == '\n'))
        if (_z_fflush(fp))
            return EOF;
    
    return c;
}

z_int32 _z_sputc(z_int32 _c, struct z_file *_p)
{
    if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
    {
        return (*_p->_p++ = _c);
    }
    else
    {
        return (_z_swbuf(_c, _p));
    }
}

z_int32 z_putchar(z_int32 c)
{
    z_int32 retval;
    retval = _z_sputc(c, z_stdout);
    return retval;
}

z_int32 z_putc(z_int32 c, struct z_file *fp)
{
    z_int32 retval;
    retval = _z_sputc(c, fp);
    return (retval);
}

z_int32 z_puts(z_int8 *s)
{
    z_int32 retval = 0;
    z_int32 c;
    struct _z_suio uio;
    struct _z_siov iov[2];

    // 3340719: __puts_null__ is used if s is NULL
    if (s == Z_NULL)
        s = __puts_null__;
    
    iov[0].iov_base = (z_void *)s;
    iov[0].iov_len = c = z_strlen(s);
    iov[1].iov_base = "\n";
    iov[1].iov_len = 1;
    uio.uio_resid = c + 1;
    uio.uio_iov = &iov[0];
    uio.uio_iovcnt = 2;
//    retval = __sfvwrite(z_stdout, &uio) ? EOF : '\n';
    return retval;
}

z_int32 _z_sflags(z_int8 *mode, z_int32 *optr)
{
    z_int32 ret, m, o;

    switch (*mode++)
    {
        case 'r':    /* open for reading */
            ret = __SRD;
            m = O_RDONLY;
            o = 0;
            break;

        case 'w':    /* open for writing */
            ret = __SWR;
            m = O_WRONLY;
            o = O_CREAT | O_TRUNC;
            break;

        case 'a':    /* open for appending */
            ret = __SWR;
            m = O_WRONLY;
            o = O_CREAT | O_APPEND;
            break;

        default:    /* illegal mode */
            z_errno = EINVAL;
            return (0);
    }

    /* [rwa]\+ or [rwa]b\+ means read and write */
    if (*mode == 'b')
        mode++;
    
    if (*mode == '+')
    {
        ret = __SRW;
        m = O_RDWR;
        mode++;
        if (*mode == 'b')
            mode++;
    }
    
    if (*mode == 'x')
        o |= O_EXCL;
    *optr = m | o;
    
    return ret;
}

struct z_file *z_fopen(z_int8 *file, z_int8 *mode)
{
    struct z_file *fp = Z_NULL;
    z_int32 f;
    z_int32 flags, oflags;

    if ((flags = _z_sflags(mode, &oflags)) == 0)
        return Z_NULL;

    if ((f = z_open(file, oflags, DEFFILEMODE)) < 0)
    {
        return Z_NULL;
    }
    /*
     * File descriptors are a full int, but _file is only a short.
     * If we get a valid file descriptor that is greater than
     * SHRT_MAX, then the fd will get sign-extended into an
     * invalid file descriptor.  Handle this case by failing the
     * open.
     */
    if (f > SHRT_MAX)
    {
        fp->_flags = 0;            /* release */
        z_close(f);
        z_errno = EMFILE;
        return Z_NULL;
    }
    fp->_file = f;
    fp->_flags = flags;
    fp->_cookie = fp;
    fp->_read = _z_sread;
    fp->_write = _z_swrite;
    fp->_seek = _z_sseek;
    fp->_close = _z_sclose;
    /*
     * When opening in append mode, even though we use O_APPEND,
     * we need to seek to the end so that ftell() gets the right
     * answer.  If the user then alters the seek pointer, or
     * the file extends, this will fail, but there is not much
     * we can do about this.  (We could set __SAPP and check in
     * fseek and ftell.)
     */
    if (oflags & O_APPEND)
        (z_void)z_sseek(fp, (z_int64)0, SEEK_END);
    
    return fp;
}


