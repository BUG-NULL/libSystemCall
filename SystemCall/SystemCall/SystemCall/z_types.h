//
//  z_types.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_types_h
#define z_types_h

// char 1, short 2, int 4, long 8, long int 8, int long 8, long long 8
typedef char                    z_int8;
typedef unsigned char           z_uint8;
typedef short                   z_int16;
typedef unsigned short          z_uint16;
typedef int                     z_int32;
typedef unsigned int            z_uint32;
typedef long                    z_int64;
typedef unsigned long           z_uint64;
typedef z_uint64                z_intptr;
typedef void                    z_void;

#define Z_NULL          0

#define NL_ARGMAX       9
#define NL_LANGMAX      14
#define NL_MSGMAX       32767
#define NL_NMAX         1
#define NL_SETMAX       255
#define NL_TEXTMAX      2048

#define SCHAR_MAX       127             /* min value for a signed char */
#define SCHAR_MIN       (-128)          /* max value for a signed char */

#define UCHAR_MAX       255             /* max value for an unsigned char */
#define CHAR_MAX        127             /* max value for a char */
#define CHAR_MIN        (-128)          /* min value for a char */

#define USHRT_MAX       65535           /* max value for an unsigned short */
#define SHRT_MAX        32767           /* max value for a short */
#define SHRT_MIN        (-32768)        /* min value for a short */

#define UINT_MAX        0xffffffff      /* max value for an unsigned int */
#define INT_MAX         2147483647      /* max value for an int */
#define INT_MIN         (-2147483647-1) /* min value for an int */

#define NSEC_PER_SEC    1000000000ull
#define NSEC_PER_MSEC   1000000ull
#define USEC_PER_SEC    1000000ull
#define NSEC_PER_USEC   1000ull

#endif /* z_types_h */
