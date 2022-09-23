//
//  z_mman.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_mman_h
#define z_mman_h

#include <SystemCall/z_types.h>

/*
 * Protections are chosen from these bits, or-ed together
 */
#define PROT_NONE       0x00    /* [MC2] no permissions */
#define PROT_READ       0x01    /* [MC2] pages can be read */
#define PROT_WRITE      0x02    /* [MC2] pages can be written */
#define PROT_EXEC       0x04    /* [MC2] pages can be executed */

/*
 * Flags contain sharing type and options.
 * Sharing types; choose one.
 */
#define MAP_SHARED      0x0001          /* [MF|SHM] share changes */
#define MAP_PRIVATE     0x0002          /* [MF|SHM] changes are private */
#define MAP_COPY        MAP_PRIVATE     /* Obsolete */

/*
 * Other flags
 */
#define MAP_FIXED        0x0010 /* [MF|SHM] interpret addr exactly */
#define MAP_RENAME       0x0020 /* Sun: rename private pages to file */
#define MAP_NORESERVE    0x0040 /* Sun: don't reserve needed swap area */
#define MAP_RESERVED0080 0x0080 /* previously unimplemented MAP_INHERIT */
#define MAP_NOEXTEND     0x0100 /* for MAP_FILE, don't change file size */
#define MAP_HASSEMAPHORE 0x0200 /* region may contain semaphores */
#define MAP_NOCACHE      0x0400 /* don't cache pages for this mapping */
#define MAP_JIT          0x0800 /* Allocate a region that will be used for JIT purposes */

/*
 * Mapping type
 */
#define MAP_FILE        0x0000  /* map from file (default) */
#define MAP_ANON        0x1000  /* allocated from memory, swap space */
#define MAP_ANONYMOUS   MAP_ANON

#define MAP_FAILED      ((z_void *)-1)    /* [MF|SHM] mmap failed */
#define MAP_UNIX03       0x40000        /* UNIX03 compliance */

z_void *z_mmap(z_void *addr, z_int64 length, z_int32 prot, z_int32 flags, z_int32 fd, z_int64 offset);
z_int32 z_munmap(z_void *addr, z_int64 length);
z_int32 z_mprotect(z_void *addr, z_int32 length, z_int32 prot);

#endif /* z_mman_h */
