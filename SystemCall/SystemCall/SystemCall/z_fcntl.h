//
//  z_fcntl.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_fcntl_h
#define z_fcntl_h

#include <SystemCall/z_types.h>

#define FREAD           0x00000001
#define FWRITE          0x00000002
#define O_NONBLOCK      0x00000004      /* no delay */
#define O_APPEND        0x00000008      /* set append mode */

#define O_CREAT         0x00000200      /* create if nonexistant */
#define O_TRUNC         0x00000400      /* truncate to zero length */
#define O_EXCL          0x00000800      /* error if already exists */

#define O_RDONLY        0x0000          /* open for reading only */
#define O_WRONLY        0x0001          /* open for writing only */
#define O_RDWR          0x0002          /* open for reading and writing */
#define O_ACCMODE       0x0003          /* mask for above modes */
#define O_CLOEXEC       0x01000000      /* implicitly set FD_CLOEXEC */

/* command values */
#define F_DUPFD         0               /* duplicate file descriptor */
#define F_GETFD         1               /* get file descriptor flags */
#define F_SETFD         2               /* set file descriptor flags */
#define F_GETFL         3               /* get file status flags */
#define F_SETFL         4               /* set file status flags */
#define F_GETOWN        5               /* get SIGIO/SIGURG proc/pgrp */
#define F_SETOWN        6               /* set SIGIO/SIGURG proc/pgrp */
#define F_GETLK         7               /* get record locking information */
#define F_SETLK         8               /* set record locking information */
#define F_SETLKW        9               /* F_SETLK; wait if blocked */

#define O_DIRECTORY     0x00100000
#define O_SYMLINK       0x00200000      /* allow open of a symlink */

z_int32 z_open(z_int8 *pathname, z_int32 flags, z_int32 mode);
z_int32 z_fcntl(z_int32 fd, z_int32 cmd, z_int64 arg);
z_int32 z_openat(z_int32 fd, z_int8 *path, z_int32 oflag);

#endif /* z_fcntl_h */
