//
//  z_string.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_string_h
#define z_string_h

#include <SystemCall/z_types.h>

z_int8 *z_strsignal(z_int32 sig);
z_int8 *z_strerror(z_int32 errnum);
z_int32 z_strncmp(z_int8 *s1, z_int8 *s2, z_uint64 n);
z_int8 *z_strncpy(z_int8 *dest, z_int8 *src, z_uint64 count);
z_int8 *z_strncat(z_int8 *dest, z_int8 *str, z_uint64 n);
z_int32 z_strlen(z_int8 *s);
z_void *z_memset(z_void *s, z_int32 c, z_uint64 n);
z_void *z_memcpy(z_void *dest, z_void *src, z_uint64 n);
z_int32 z_memcmp(z_void *s1, z_void *s2, z_int32 n);
z_int8 *z_strdup(z_int8 *str);
z_void *z_memmove(z_void *dst0, z_void *src0, z_int32 length);

#endif /* z_string_h */
