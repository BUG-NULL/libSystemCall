//
//  z_pthread_tlv.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_pthread_tlv_h
#define z_pthread_tlv_h

#include <SystemCall/z_pthread.h>

z_void z_pthread_tlv_initializer(struct z_mach_header_64 *mh);
z_void z_pthread_tlv_atexit(z_void *func, z_void *objAddr);
z_void z_pthread_tlv_exit(z_void);

#endif /* z_pthread_tlv_h */
