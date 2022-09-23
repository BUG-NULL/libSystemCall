//
//  z_malloc.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_malloc_h
#define z_malloc_h

#include <SystemCall/z_types.h>

z_void *z_malloc(z_uint64 size);
z_void z_free(z_void *addr, z_uint64 size);

#endif /* z_malloc_h */
