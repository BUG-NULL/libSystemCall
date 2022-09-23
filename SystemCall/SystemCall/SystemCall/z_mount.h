//
//  z_mount.h
//  SystemCall
//
//  Created by NULL 
//

#ifndef z_mount_h
#define z_mount_h

#include <SystemCall/z_types.h>

#define MFSNAMELEN      15      /* length of fs type name, not inc. null */
#define MFSTYPENAMELEN  16      /* length of fs type name including null */
#define MAXPATHLEN      4096

/*
 * User specifiable flags.
 *
 * Unmount uses MNT_FORCE flag.
 */
#define MNT_RDONLY      0x00000001      /* read only filesystem */
#define MNT_SYNCHRONOUS 0x00000002      /* file system written synchronously */
#define MNT_NOEXEC      0x00000004      /* can't exec from filesystem */
#define MNT_NOSUID      0x00000008      /* don't honor setuid bits on fs */
#define MNT_NODEV       0x00000010      /* don't interpret special files */
#define MNT_UNION       0x00000020      /* union with underlying filesystem */
#define MNT_ASYNC       0x00000040      /* file system written asynchronously */
#define MNT_CPROTECT    0x00000080      /* file system supports content protection */

struct z_fsid
{
    z_int32 val[2];
};

struct z_statfs64
{
    z_uint32    f_bsize;        /* fundamental file system block size */
    z_int32     f_iosize;       /* optimal transfer block size */
    z_uint64    f_blocks;       /* total data blocks in file system */
    z_uint64    f_bfree;        /* free blocks in fs */
    z_uint64    f_bavail;       /* free blocks avail to non-superuser */
    z_uint64    f_files;        /* total file nodes in file system */
    z_uint64    f_ffree;        /* free file nodes in fs */
    struct z_fsid f_fsid;       /* file system id */
    z_uint32    f_owner;        /* user that mounted the filesystem */
    z_uint32    f_type;         /* type of filesystem */
    z_uint32    f_flags;        /* copy of mount exported flags */
    z_uint32    f_fssubtype;    /* fs sub-type (flavor) */
    z_int8      f_fstypename[MFSTYPENAMELEN];   /* fs type name */
    z_int8      f_mntonname[MAXPATHLEN];        /* directory on which mounted */
    z_int8      f_mntfromname[MAXPATHLEN];      /* mounted filesystem */
    z_uint32    f_flags_ext;    /* extended flags */
    z_uint32    f_reserved[7];  /* For future use */
};

z_int32 z_statfs(z_int8 *path, struct z_statfs64 *buf);
z_int32 z_fstatfs(z_int32 fd, struct z_statfs64 *buf);

#endif /* z_mount_h */
