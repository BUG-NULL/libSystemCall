//
//  z_stdlib.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_stdlib_h
#define z_stdlib_h

#include <SystemCall/z_types.h>

#define z_assert(e) ((void) ((e) ? ((void)0) : _z_assert(#e, __FILE__, __LINE__)))
#define _z_assert(e, file, line) \
    ((void)z_printf ("%s:%d: failed assertion `%s'\n", file, line, e), z_abort())

z_void z_abort(z_void);
z_void z_exit(z_int32 status);
z_int8 *z_getenv(z_int8 *name, z_int8 **environ);
z_int32 z_rand(z_void);
z_int32 z_rand_r(z_uint32 *ctx);
z_void z_sranddev(z_void);
z_int32 z_grantpt(z_int32 fd);
z_int32 z_ptsname_r(z_int32 fd, z_int8 *buffer, z_int32 buflen);
z_int8 *z_ptsname(z_int32 fd);
z_int32 z_unlockpt(z_int32 fd);
z_int32 z_posix_openpt(z_int32 flags);
z_int32 z_mergesort(z_void *base, z_int32 nel, z_int32 width, z_int32 (*compar)(z_void *, z_void *));
z_int8 *z_devname(z_int32 dev, z_uint16 type);
z_int8 *z_devname_r(z_int32 dev, z_uint16 type, z_int8 *buf, z_int32 len);

#endif /* z_stdlib_h */
