//
//  z_ioctl.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_ioctl_h
#define z_ioctl_h

#include <SystemCall/z_types.h>

/*
 * Types for d_type.
 * These are returned by ioctl FIODTYPE
 */
#define D_TAPE  1
#define D_DISK  2
#define D_TTY   3

/*
 * Ioctl's have the command encoded in the lower word, and the size of
 * any in or out parameters in the upper word.  The high 3 bits of the
 * upper word are used to encode the in/out status of the parameter.
 */
#define IOCPARM_MASK    0x1fff          /* parameter length, at most 13 bits */
#define IOCPARM_LEN(x)  (((x) >> 16) & IOCPARM_MASK)
#define IOCBASECMD(x)   ((x) & ~(IOCPARM_MASK << 16))
#define IOCGROUP(x)     (((x) >> 8) & 0xff)

#define IOCPARM_MAX     (IOCPARM_MASK + 1)      /* max size of ioctl args */
/* no parameters */
#define IOC_VOID        (z_uint32)0x20000000
/* copy parameters out */
#define IOC_OUT         (z_uint32)0x40000000
/* copy parameters in */
#define IOC_IN          (z_uint32)0x80000000
/* copy parameters in and out */
#define IOC_INOUT       (IOC_IN | IOC_OUT)
/* mask for IN/OUT/VOID */
#define IOC_DIRMASK     (z_uint32)0xe0000000

#define _IOC(inout, group, num, len) \
    (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define _IO(g, n)       _IOC(IOC_VOID,  (g), (n), 0)
#define _IOR(g, n, t)   _IOC(IOC_OUT,   (g), (n), sizeof(t))
#define _IOW(g, n, t)   _IOC(IOC_IN,    (g), (n), sizeof(t))
/* this should be _IORW, but stdio got there first */
#define _IOWR(g, n, t)  _IOC(IOC_INOUT, (g), (n), sizeof(t))

z_int32 z_ioctl(z_int32 fildes, z_uint64 request, ...);

#endif /* z_ioctl_h */
