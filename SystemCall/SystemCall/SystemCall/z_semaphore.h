//
//  z_semaphore.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_semaphore_h
#define z_semaphore_h

#include <SystemCall/z_types.h>

typedef z_int32     z_sem_t;

z_int32 z_sem_close(z_sem_t *sem);
z_sem_t *z_sem_open(z_int8 *name, z_int32 oflag, z_int32 mode, z_uint32 value);
z_int32 z_sem_post(z_sem_t *sem);
z_int32 z_sem_trywait(z_sem_t *sem);
z_int32 z_sem_unlink(z_int8 *name);
z_int32 z_sem_wait(z_sem_t *sem);

#endif /* z_semaphore_h */
