//
//  z_error.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_error_h
#define z_error_h

#include <SystemCall/z_types.h>

#define EPERM           1               /* Operation not permitted */
#define ENOENT          2               /* No such file or directory */
#define ESRCH           3               /* No such process */
#define EINTR           4               /* Interrupted system call */
#define EIO             5               /* Input/output error */
#define EBADF           9               /* Bad file descriptor */
#define ECHILD          10              /* No child processes */
#define EDEADLK         11              /* Resource deadlock avoided */
#define EFAULT          14              /* Bad address */
#define EBUSY           16              /* Device / Resource busy */
#define ENOTDIR         20              /* Not a directory */
#define EINVAL          22              /* Invalid argument */
#define EMFILE          24              /* Too many open files */
#define ENOTTY          25              /* Inappropriate ioctl for device */
#define ESPIPE          29              /* Illegal seek */
#define EDOM            33              /* Numerical argument out of domain */
#define ERANGE          34              /* Result too large */
#define EAGAIN          35              /* Resource temporarily unavailable */
#define EWOULDBLOCK     EAGAIN          /* Operation would block */
#define EINPROGRESS     36              /* Operation now in progress */
#define EALREADY        37              /* Operation already in progress */
/* ipc/network software -- argument errors */
#define ENOTSOCK        38              /* Socket operation on non-socket */
#define EDESTADDRREQ    39              /* Destination address required */
#define EMSGSIZE        40              /* Message too long */
#define EPROTOTYPE      41              /* Protocol wrong type for socket */
#define ENOPROTOOPT     42              /* Protocol not available */
#define EPROTONOSUPPORT 43              /* Protocol not supported */
#define ESOCKTNOSUPPORT 44              /* Socket type not supported */
#define ENOTSUP         45              /* Operation not supported */
#define EOPNOTSUPP       ENOTSUP        /* Operation not supported on socket */
#define EPFNOSUPPORT    46              /* Protocol family not supported */
#define EAFNOSUPPORT    47              /* Address family not supported by protocol family */
#define EADDRINUSE      48              /* Address already in use */
#define EADDRNOTAVAIL   49              /* Can't assign requested address */
#define ETIMEDOUT       60              /* Operation timed out */
#define ECONNREFUSED    61              /* Connection refused */
#define ENOTRECOVERABLE 104             /* State not recoverable */
#define EOWNERDEAD      105             /* Previous owner died */

#define z_errno         (*z_perrno())

z_int64 *z_perrno(z_void);

#endif /* z_error_h */
