//
//  z_pthread_tsd.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_pthread_tsd_h
#define z_pthread_tsd_h

#include <SystemCall/z_pthread.h>

z_void z_thread_set_tsd_base(z_void *tsd_base);
z_int32 z_pthread_key_create(z_uint64 *key, z_void (*destructor)(z_void *));
z_int32 z_pthread_key_delete(z_uint64 key);
z_void z_pthread_tsd_cleanup(struct z_pthread_t *thread);
z_uint64 z_pthread_getspecific(z_uint64 key);
z_int32 z_pthread_setspecific(z_uint64 key, z_uint64 value);
z_uint64 z_pthread_getspecific_direct(z_uint64 slot);
z_int32 z_pthread_setspecific_direct(z_uint64 slot, z_uint64 val);

#endif /* z_pthread_tsd_h */
