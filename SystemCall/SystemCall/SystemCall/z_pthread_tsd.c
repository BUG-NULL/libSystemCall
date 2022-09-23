//
//  z_pthread_tsd.c
//  SystemCall
//
//  Created by NULL
//

#include "z_pthread_tsd.h"
#include "z_mman.h"
#include "z_malloc.h"
#include "z_string.h"
#include "z_error.h"

#define PTHREAD_DESTRUCTOR_ITERATIONS     4

z_int32 __pthread_tsd_first = __TSD_RESERVED_MAX + 1;
z_int32 __pthread_tsd_start = _INTERNAL_POSIX_THREAD_KEYS_MAX;
z_int32 __pthread_tsd_end = _INTERNAL_POSIX_THREAD_KEYS_END;
z_int32 __pthread_tsd_max = __TSD_RESERVED_MAX + 1;

struct
{
    z_uint64 destructor;
} z_pthread_keys[_INTERNAL_POSIX_THREAD_KEYS_END];

z_int8 z_pthread_key_get_destructor(z_uint64 key, z_void (**destructor)(z_void *))
{
    z_uint64 value = z_pthread_keys[key].destructor;
    if (destructor)
    {
        *destructor = (z_void (*)(z_void *))(~value);
    }
    return (value != 0);
}

z_void z_pthread_tsd_cleanup_key(struct z_pthread_t *self, z_uint64 key)
{
    z_void (*destructor)(z_void *);
    if (z_pthread_key_get_destructor(key, &destructor))
    {
        z_void **ptr = (z_void **)(&(self->tsd[key]));
        z_void *value = *ptr;
        if (value)
        {
            *ptr = Z_NULL;
            if (destructor)
            {
                destructor(value);
            }
        }
    }
}

z_void z_pthread_tsd_cleanup_new(struct z_pthread_t *self)
{
    z_int32 j = 0;

    // clean up all keys
    for (j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; j++)
    {
        z_uint64 k;
        for (k = __pthread_tsd_start; k <= self->max_tsd_key; k++)
        {
            z_pthread_tsd_cleanup_key(self, k);
        }

        for (k = __pthread_tsd_first; k <= __pthread_tsd_max; k++)
        {
            z_pthread_tsd_cleanup_key(self, k);
        }
    }

    self->max_tsd_key = 0;
}

z_void z_pthread_tsd_cleanup(struct z_pthread_t *self)
{
    z_pthread_tsd_cleanup_new(self);
}

z_int8 z_pthread_key_set_destructor(z_uint64 key, z_void (*destructor)(z_void *))
{
    z_uint64 *ptr = &z_pthread_keys[key].destructor;
    z_uint64 value = ~(z_uint64)destructor;
    if (*ptr == 0)
    {
        *ptr = value;
        return 1;
    }
    return 0;
}

z_int8 z_pthread_key_unset_destructor(z_uint64 key)
{
    z_uint64 *ptr = &z_pthread_keys[key].destructor;
    if (*ptr != 0)
    {
        *ptr = 0;
        return 1;
    }
    return 0;
}

z_int32 z_pthread_setspecific(z_uint64 key, z_uint64 value)
{
    z_int32 res = EINVAL;

    if (key >= __pthread_tsd_first && key < __pthread_tsd_end)
    {
        z_int8 created = z_pthread_key_get_destructor(key, Z_NULL);
        if (key < __pthread_tsd_start || created)
        {
            struct z_pthread_t *self = z_pthread_self();
            self->tsd[key] = value;
            res = 0;
            if (key < __pthread_tsd_start)
            {
                // XXX: is this really necessary?
                z_pthread_key_set_destructor(key, Z_NULL);
            }
            if (key > self->max_tsd_key)
            {
                self->max_tsd_key = (z_uint16)key;
            }
        }
    }

    return res;
}

z_uint64 z_pthread_getspecific(z_uint64 key)
{
    return z_pthread_getspecific_direct(key);
}

z_int32 z_pthread_key_create(z_uint64 *key, z_void (*destructor)(z_void *))
{
    z_int32 res = EAGAIN; // Returns EAGAIN if key cannot be allocated.
    z_uint64 k;

    for (k = __pthread_tsd_start; k < __pthread_tsd_end; k++)
    {
        if (z_pthread_key_set_destructor(k, destructor))
        {
            *key = k;
            res = 0;
            break;
        }
    }
    return res;
}

z_int32 z_pthread_key_delete(z_uint64 key)
{
    z_int32 res = EINVAL; // Returns EINVAL if key is not allocated.

    if (key >= __pthread_tsd_start && key < __pthread_tsd_end)
    {
        if (z_pthread_key_unset_destructor(key))
        {
//            struct _pthread *p;
//            TAILQ_FOREACH(p, &__pthread_head, tl_plist) {
//                // No lock for word-sized write.
//                p->tsd[key] = 0;
//            }
            res = 0;
        }
    }

    return res;
}

z_uint64 z_pthread_getspecific_direct(z_uint64 slot)
{
    return z_tsd_get_direct(slot);
}

z_int32 z_pthread_setspecific_direct(z_uint64 slot, z_uint64 val)
{
    return z_tsd_set_direct(slot, val);
}
