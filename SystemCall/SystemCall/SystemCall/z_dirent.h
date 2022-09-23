//
//  z_dirent.h
//  SystemCall
//
//  Created by NULL 
//

#ifndef z_dirent_h
#define z_dirent_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_stdio.h>

/* definitions for library routines operating on directories. */
#define DIRBLKSIZ       1024

/* flags for opendir2 */
#define DTF_HIDEW       0x0001  /* hide whiteout entries */
#define DTF_NODUP       0x0002  /* don't return duplicate names */
#define DTF_REWIND      0x0004  /* rewind after reading union stack */
#define __DTF_READALL   0x0008  /* everything has been read */
#define __DTF_SKIPREAD  0x0010  /* assume internal buffer is populated */
#define __DTF_ATEND     0x0020  /* there's nothing more to read in the kernel */

/*
 * File types
 */
#define DT_UNKNOWN       0
#define DT_FIFO          1
#define DT_CHR           2
#define DT_DIR           4
#define DT_BLK           6
#define DT_REG           8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14

#define READDIR_INITIAL_SIZE    2048
#define READDIR_LARGE_SIZE      (8 << 10)
#define DIRENT_MAXPATHLEN       1024

enum z_getdirentries64_flags_t
{
    GETDIRENTRIES64_EOF = 1U << 0,
};

struct z_ddloc
{
    struct
    {
        struct z_ddloc *le_next;   /* next element */
        struct z_ddloc **le_prev;  /* address of previous next element */
    } loc_lqe;
    z_int64    loc_index;    /* key associated with structure */
    z_int64    loc_seek;    /* magic cookie returned by getdirentries */
    z_int64    loc_loc;    /* offset of entry in buffer */
};

struct z_telldir
{
    struct
    {
        struct z_ddloc *lh_first;  /* first element */
    } td_locq;
    z_int64     td_loccnt;  /* index of entry for sequential readdir's */
    z_int64     seekoff;    /* 64-bit seek offset */
};

struct z_dir
{
    z_int32     __dd_fd;     /* file descriptor associated with directory */
    z_int64     __dd_loc;    /* offset in current buffer */
    z_int64     __dd_size;   /* amount of data returned */
    z_int8      *__dd_buf;   /* data buffer */
    z_int32     __dd_len;    /* size of data buffer */
    z_int64     __dd_seek;   /* magic cookie returned */
    z_int64     __padding;   /* (__dd_rewind space left for bincompat) */
    z_int32     __dd_flags;  /* flags for readdir */
    struct z_pthread_mutex_t __dd_lock; /* for thread locking */
    struct z_telldir *__dd_td; /* telldir position recording */
};

struct z_dirent
{
    z_uint64    d_ino;      /* file number of entry */
    z_uint64    d_seekoff;  /* seek offset (optional, used by servers) */
    z_uint16    d_reclen;   /* length of this record */
    z_uint16    d_namlen;   /* length of string in d_name */
    z_uint8     d_type;     /* file type, see below */
    z_int8      d_name[DIRENT_MAXPATHLEN]; /* entry name (up to MAXPATHLEN bytes) */
};

z_int32 z_getdirentries(z_int32 fd, z_int8 *buf, z_int32 nbytes, z_int64 *basep);
struct z_dir *z_opendir(z_int8 *filename);
z_int32 z_closedir(struct z_dir *dirp);
struct z_dirent *z_readdir(struct z_dir *dirp);

#endif /* z_dirent_h */
