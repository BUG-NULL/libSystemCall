//
//  z_unistd.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_unistd_h
#define z_unistd_h

#include <SystemCall/z_types.h>

#define STDIN_FILENO    0    /* standard input file descriptor */
#define STDOUT_FILENO   1    /* standard output file descriptor */
#define STDERR_FILENO   2    /* standard error file descriptor */

z_int32 z_read(z_int32 fd, z_void *buf, z_int32 count);
z_int32 z_write(z_int32 fd, z_void *buf, z_int32 count);
z_int32 z_close(z_int32 fd);
z_int32 z_lseek(z_int32 fd, z_int64 offset, z_int32 whence);
z_int32 z_unlink(z_int8 *name);
z_int32 z_getpid(z_void);
z_int32 z_getppid(z_void);
z_int32 z_getuid(z_void);
z_uint32 z_sleep(z_uint32 second);
z_int32 z_access(z_int8 *name, z_int32 mode);
z_int32 z_pipe(z_int32 file[2]);
z_int32 z_execve(z_int8 *file, z_int8 **argv, z_int8 **envp);
z_int32 z_fork(z_void);
z_int32 z_isatty(z_int32 fd);
z_int8 *z_ttyname(z_int32 fd);
z_int32 z_getlogin(z_int8 *name, z_int32 len);
z_int32 z_setlogin(z_int8 *name);
z_int32 z_dup(z_int32 fildes);
z_int32 z_dup2(z_int32 fildes, z_int32 fildes2);
z_int32 z_getpagesize(z_void);

#endif /* z_unistd_h */
