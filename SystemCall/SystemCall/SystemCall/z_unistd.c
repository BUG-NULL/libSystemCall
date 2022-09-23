//
//  z_unistd.c
//  SystemCall
//
//  Created by NULL
//

#include "z_unistd.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_time.h"
#include "z_select.h"
#include "z_string.h"
#include "z_ioctl.h"
#include "z_termios.h"
#include "z_sysctl.h"
#include "z_stat.h"
#include "z_stdlib.h"
#include "z_malloc.h"
#include "z_pthread.h"
#include "z_mach.h"

#define _PATH_DEV   "/dev/"
#define MAXNAMLEN   255
#define MAXLOGNAME  255     /* max login name length */

z_int32 zz_fork(z_void);
z_int8 *buf = Z_NULL;

z_int32 z_read(z_int32 fd, z_void *buf, z_int32 count)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_read), fd, buf, count));
}

z_int32 z_write(z_int32 fd, z_void *buf, z_int32 count)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_write), fd, buf, count));
}

z_int32 z_close(z_int32 fd)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_close), fd));
}

z_int32 z_lseek(z_int32 fd, z_int64 offset, z_int32 whence)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_lseek), fd, offset, whence));
}

z_int32 z_unlink(z_int8 *name)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_unlink), name));
}

z_int32 z_getpid(z_void)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getpid)));
}

z_int32 z_getppid(z_void)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getppid)));
}

z_int32 z_getuid(z_void)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getuid)));
}

z_uint32 z_sleep(z_uint32 second)
{
    struct z_fd_set rd;
    struct z_timeval tv;
    z_int32 err;
    
    z_memset(&rd, 0, sizeof(rd));
    tv.tv_sec = second;
    tv.tv_usec = 0;
    
    err = z_select(1, &rd, 0, 0, &tv);
    if (err == 0)
    {
//        z_printf("sleep time out!\n");
    }
    
    return err;
}

z_int32 z_access(z_int8 *name, z_int32 mode)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_access), name, mode));
}

z_int32 z_pipe(z_int32 file[2])
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_pipe), file));
}

z_int32 z_execve(z_int8 *file, z_int8 **argv, z_int8 **envp)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_execve), file, argv, envp));
}

z_void z_atfork_prepare(z_void)
{
    z_pthread_atfork_prepare_handlers();
    z_pthread_atfork_prepare();
}

z_void z_atfork_parent(z_void)
{
    z_pthread_atfork_parent();
    z_pthread_atfork_parent_handlers();
}

z_void z_atfork_child(z_void)
{
    z_pthread_atfork_child();
    z_mach_init();
    z_pthread_set_self();
    z_pthread_atfork_child_handlers();
}

z_int32 z_fork(z_void)
{
    z_int32 ret;
    
    z_atfork_prepare();
    ret = zz_fork();
    if (-1 == ret)
    {
        z_atfork_parent();
        return ret;
    }
    
    if (0 == ret)
    {
        z_atfork_child();
        return 0;
    }
    
    z_atfork_parent();
    return ret;
}

z_int32 z_isatty(z_int32 fd)
{
    z_int32 retval = 0;
    z_int32 type = 0;
    struct z_termios t;

    if (z_ioctl(fd, FIODTYPE, &type) != -1)
    {
        if ((retval = (type == D_TTY)) == 0)
        {
            z_errno = ENOTTY;
        }
    }
    else
    {
        retval = (z_tcgetattr(fd, &t) != -1);
    }
    
    return retval;
}

z_int8 *z_ttyname(z_int32 fd)
{
    struct z_stat sb;
    struct z_termios ttyb;

    /* Must be a terminal. */
    if (z_tcgetattr(fd, &ttyb) < 0)
        return (Z_NULL);
    
    /* Must be a character device. */
    if (z_fstat(fd, &sb))
        return (Z_NULL);
    
    if (!S_ISCHR(sb.st_mode))
    {
        z_errno = ENOTTY;
        return (Z_NULL);
    }

    if (buf == Z_NULL)
    {
        buf = z_malloc(sizeof(_PATH_DEV) + MAXNAMLEN);
        z_memset(buf, 0, (sizeof(_PATH_DEV) + MAXNAMLEN));
    }
    z_strncpy(buf, _PATH_DEV, sizeof(_PATH_DEV) + MAXNAMLEN);
    if (z_devname_r(sb.st_rdev, S_IFCHR, buf + z_strlen(buf), sizeof(_PATH_DEV) + MAXNAMLEN - z_strlen(buf)) == Z_NULL)
    {
        z_errno = ERANGE;
        return (Z_NULL);
    }
    return (buf);
}

z_int32 z_getlogin(z_int8 *name, z_int32 len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getlogin), name, len));
}

z_int32 z_setlogin(z_int8 *name)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_setlogin), name));
}

z_int32 z_dup(z_int32 fildes)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_dup), fildes));
}

z_int32 z_dup2(z_int32 fildes, z_int32 fildes2)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_dup2), fildes, fildes2));
}

z_int32 z_getpagesize(z_void)
{
    z_int32 mib[2];
    z_int32 value = 0;
    z_int32 size = 0;

    if (!value)
    {
        mib[0] = CTL_HW;
        mib[1] = HW_PAGESIZE;
        size = sizeof(value);
        if (z_sysctl(mib, 2, &value, &size, Z_NULL, 0) == -1)
            return -1;
    }
    return value;
}
