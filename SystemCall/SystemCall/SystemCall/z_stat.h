//
//  z_stat.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_stat_h
#define z_stat_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_time.h>

/* File mode */
/* Read, write, execute/search by owner */
#define S_IRWXU         0000700         /* [XSI] RWX mask for owner */
#define S_IRUSR         0000400         /* [XSI] R for owner */
#define S_IWUSR         0000200         /* [XSI] W for owner */
#define S_IXUSR         0000100         /* [XSI] X for owner */
/* Read, write, execute/search by group */
#define S_IRWXG         0000070         /* [XSI] RWX mask for group */
#define S_IRGRP         0000040         /* [XSI] R for group */
#define S_IWGRP         0000020         /* [XSI] W for group */
#define S_IXGRP         0000010         /* [XSI] X for group */
/* Read, write, execute/search by others */
#define S_IRWXO         0000007         /* [XSI] RWX mask for other */
#define S_IROTH         0000004         /* [XSI] R for other */
#define S_IWOTH         0000002         /* [XSI] W for other */
#define S_IXOTH         0000001         /* [XSI] X for other */

#define DEFFILEMODE     (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/* File type */
#define S_IFMT          0170000         /* [XSI] type of file mask */
#define S_IFIFO         0010000         /* [XSI] named pipe (fifo) */
#define S_IFCHR         0020000         /* [XSI] character special */
#define S_IFDIR         0040000         /* [XSI] directory */
#define S_IFBLK         0060000         /* [XSI] block special */
#define S_IFREG         0100000         /* [XSI] regular */
#define S_IFLNK         0120000         /* [XSI] symbolic link */
#define S_IFSOCK        0140000         /* [XSI] socket */
#define S_IFWHT         0160000         /* OBSOLETE: whiteout */

/*
 * [XSI] The following macros shall be provided to test whether a file is
 * of the specified type.  The value m supplied to the macros is the value
 * of st_mode from a stat structure.  The macro shall evaluate to a non-zero
 * value if the test is true; 0 if the test is false.
 */
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)     /* block special */
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)     /* char special */
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)     /* directory */
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)     /* fifo or socket */
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)     /* regular file */
#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)     /* symbolic link */
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)    /* socket */
#define S_ISWHT(m)      (((m) & S_IFMT) == S_IFWHT)     /* OBSOLETE: whiteout */

struct z_stat
{
    z_int32             st_dev;             /* [XSI] ID of device containing file */
    z_uint16            st_mode;            /* [XSI] Mode of file (see below) */
    z_uint16            st_nlink;           /* [XSI] Number of hard links */
    z_uint64            st_ino;             /* [XSI] File serial number */
    z_uint32            st_uid;             /* [XSI] User ID of the file */
    z_uint32            st_gid;             /* [XSI] Group ID of the file */
    z_int32             st_rdev;            /* [XSI] Device ID */
    struct z_timespec   st_atimespec;       /* time of last access */
    struct z_timespec   st_mtimespec;       /* time of last data modification */
    struct z_timespec   st_ctimespec;       /* time of last status change */
    struct z_timespec   st_birthtimespec;   /* time of file creation(birth) */
    z_int64             st_size;            /* [XSI] file size, in bytes */
    z_int64             st_blocks;          /* [XSI] blocks allocated for file */
    z_int32             st_blksize;         /* [XSI] optimal blocksize for I/O */
    z_uint32            st_flags;           /* user defined flags for file */
    z_uint32            st_gen;             /* file generation number */
    z_int32             st_lspare;          /* RESERVED: DO NOT USE! */
    z_int64             st_qspare[2];       /* RESERVED: DO NOT USE! */
};

z_int32 z_fstat(z_int32 fd, struct z_stat *stat);
z_int32 z_lstat(z_int8 *path, struct z_stat *buf);
z_int32 z_mkfifo(z_int8 *name, z_uint16 mode);

#endif /* z_stat_h */
