//
//  z_malloc.c
//  SystemCall
//
//  Created by NULL
//

#include "z_malloc.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_mman.h"
#include "z_string.h"

z_void *z_malloc(z_uint64 size)
{
    z_void *p = z_mmap(Z_NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (p == MAP_FAILED || p == Z_NULL)
    {
        return Z_NULL;
    }

    z_memset(p, 0, size);
    return p;
}

z_void z_free(z_void *addr, z_uint64 size)
{
    if (addr)
    {
        z_int32 ret = 0;
        ret = z_munmap(addr, size);
        if (ret == 0)
        {

        }
    }
}


