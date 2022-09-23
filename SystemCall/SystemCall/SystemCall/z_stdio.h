//
//  z_stdio.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_stdio_h
#define z_stdio_h

#include <SystemCall/z_types.h>
#include <stdarg.h>

#define __SLBF      0x0001      /* line buffered */
#define __SNBF      0x0002      /* unbuffered */
#define __SRD       0x0004      /* OK to read */
#define __SWR       0x0008      /* OK to write */
    /* RD and WR are never simultaneously asserted */
#define __SRW       0x0010      /* open for reading & writing */
#define __SEOF      0x0020      /* found EOF */
#define __SERR      0x0040      /* found error */
#define __SMBF      0x0080      /* _buf is from malloc */
#define __SAPP      0x0100      /* fdopen()ed in append mode */
#define __SSTR      0x0200      /* this is an sprintf/snprintf string */
#define __SOPT      0x0400      /* do fseek() optimisation */
#define __SNPT      0x0800      /* do not do fseek() optimisation */
#define __SOFF      0x1000      /* set iff _offset is in fact correct */
#define __SMOD      0x2000      /* true => fgetln modified _p text */
#define __SALC      0x4000      /* allocate string space dynamically */
#define __SIGN      0x8000      /* ignore this file in _fwalk */

#define SEEK_SET    0           /* set file offset to offset */
#define SEEK_CUR    1           /* set file offset to current plus offset */
#define SEEK_END    2           /* set file offset to EOF plus offset */

#define ULLONG_MAX  0xffffffffffffffffULL       /* max unsigned long long */
#define LLONG_MAX   0x7fffffffffffffffLL        /* max signed long long */
#define LLONG_MIN   (-0x7fffffffffffffffLL-1)   /* min signed long long */

#define USHRT_MAX   65535       /* max value for an unsigned short */
#define SHRT_MAX    32767       /* max value for a short */
#define SHRT_MIN    (-32768)    /* min value for a short */

#define OFF_MIN     LLONG_MIN   /* min value for an off_t */
#define OFF_MAX     LLONG_MAX   /* max value for an off_t */

#define EOF         (-1)

/* must be == _POSIX_STREAM_MAX <limits.h> */
#define FOPEN_MAX    20     /* must be <= OPEN_MAX <sys/syslimits.h> */
#define FILENAME_MAX 1024   /* must be <= PATH_MAX <sys/syslimits.h> */

struct z_sbuf
{
    z_uint8     *_base;
    z_int32     _size;
};

typedef union
{
    z_int8      __mbstate8[128];
    z_int64     _mbstateL;                      /* for alignment */
} z_mbstate_t;

struct z_pthread_mutex_t
{
    z_int64     __sig;
    z_int8      __opaque[56];
};

struct z_sfilex
{
    z_uint8     *up;                    /* saved _p when _p is doing ungetc data */
    struct z_pthread_mutex_t fl_mutex;  /* used for MT-safety */
    z_int32     orientation:2;          /* orientation for fwide() */
    z_int32     counted:1;              /* stream counted against STREAM_MAX */
    z_mbstate_t mbstate;                /* multibyte conversion state */
};

struct z_file
{
    z_uint8         *_p;        /* current position in (some) buffer */
    z_int32         _r;         /* read space left for getc() */
    z_int32         _w;         /* write space left for putc() */
    z_int16         _flags;     /* flags, below; this FILE is free if 0 */
    z_int16         _file;      /* fileno, if Unix descriptor, else -1 */
    struct z_sbuf   _bf;        /* the buffer (at least 1 byte, if !NULL) */
    z_int32         _lbfsize;   /* 0 or -_bf._size, for inline putc */
    /* operations */
    z_void          *_cookie;   /* cookie passed to io functions */
    z_int32         (* _close)(z_void *);
    z_int32         (* _read) (z_void *, z_int8 *, z_int32);
    z_int64         (* _seek) (z_void *, z_int64, z_int32);
    z_int32         (* _write)(z_void *, z_int8 *, z_int32);
    /* separate buffer for long sequences of ungetc() */
    struct z_sbuf   _ub;        /* ungetc buffer */
    struct z_sfilex *_extra;    /* additions to FILE to not break ABI */
    z_int32         _ur;        /* saved _r when _r is counting ungetc data */
    /* tricks to meet minimum requirements even when malloc() fails */
    z_uint8         _ubuf[3];   /* guarantee an ungetc() buffer */
    z_uint8         _nbuf[1];   /* guarantee a getc() buffer */

    /* separate buffer for fgetln() when line crosses buffer boundary */
    struct z_sbuf   _lb;        /* buffer for fgetln() */

    /* Unix stdio files get aligned to block boundaries on fseek() */
    z_int32         _blksize;   /* stat.st_blksize (may be != _bf._size) */
    z_int64         _offset;    /* current lseek offset (see WARNING) */
};

struct _z_siov
{
    void            *iov_base;
    z_int32         iov_len;
};

struct _z_suio
{
    struct _z_siov  *uio_iov;
    z_int32         uio_iovcnt;
    z_int32         uio_resid;
};

z_void z_vprintf(z_int8 *fmt, va_list ap);
z_void z_vfdprintf(z_int32 fd, z_int8 *fmt, va_list ap);
z_void z_printf(z_int8 *fmt, ...);
z_void z_fdprintf(z_int32 fd, z_int8 *fmt, ...);
z_void z_perror(z_int8 *str);
z_int32 z_sprintf(z_int8 *str, z_int8 *fmt, ...);
z_int32 z_getchar(z_void);
z_int32 z_getc(struct z_file *fp);

#endif /* z_stdio_h */
