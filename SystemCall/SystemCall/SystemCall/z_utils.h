//
//  z_utils.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_utils_h
#define z_utils_h

#include <stdarg.h>
#include <SystemCall/z_types.h>

z_void z_show_register(z_void);
z_int32 z_isdigit(z_uint8 c);
z_int32 z_isspace(z_int32 ch);
z_uint32 z_inet_aton(z_int8 *c_ipaddr);
z_int32 cpu_endian(z_void);
z_uint64 z_htonl(z_uint64 h);
z_uint32 z_ntohl(z_uint32 n);
z_uint16 z_htons(z_uint16 h);
z_uint16 z_ntohs(z_uint16 n);
z_int32 z_sprintf(z_int8 *str, z_int8 *fmt, ...);
z_int8 *z_inet_ntoa(z_uint32 in);
z_uint32 z_inet_aton(z_int8 *c_ipaddr);
z_void z_itoa(z_uint32 n, z_int8 *buf);
z_int32 z_atoi(z_int8 *pstr);
z_void z_xtoa(z_uint32 n, z_int8 *buf);

#endif /* z_utils_h */
