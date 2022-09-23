//
//  z_termios.c
//  SystemCall
//
//  Created by NULL
//

#include "z_termios.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_ioctl.h"
#include "z_string.h"
#include "z_unistd.h"

z_int32 z_tcgetattr(z_int32 fd, struct z_termios *t)
{
    return z_ioctl(fd, TIOCGETA, t);
}

z_int32 z_tcsetattr(z_int32 fd, z_int32 opt, struct z_termios *t)
{
    struct z_termios localterm;

    if (opt & TCSASOFT)
    {
        z_memcpy(&localterm, t, sizeof(struct z_termios));
        localterm.c_cflag |= CIGNORE;
        t = &localterm;
    }
    
    switch (opt & ~TCSASOFT)
    {
        case TCSANOW:
            return z_ioctl(fd, TIOCSETA, t);
        case TCSADRAIN:
            return z_ioctl(fd, TIOCSETAW, t);
        case TCSAFLUSH:
            return z_ioctl(fd, TIOCSETAF, t);
        default:
            z_errno = EINVAL;
            return -1;
    }
}

z_int32 z_tcsetpgrp(z_int32 fd, z_int32 pgrp)
{
    z_int32 s;

    if (z_isatty(fd) == 0)
        return -1;

    s = pgrp;
    return z_ioctl(fd, TIOCSPGRP, &s);
}

z_int32 z_tcgetpgrp(z_int32 fd)
{
    z_int32 s;

    if (z_isatty(fd) == 0)
        return -1;

    if (z_ioctl(fd, TIOCGPGRP, &s) < 0)
        return -1;

    return s;
}

z_void z_cfmakeraw(struct z_termios *t)
{
    t->c_iflag &= ~(IMAXBEL|IXOFF|INPCK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IGNPAR);
    t->c_iflag |= IGNBRK;
    t->c_oflag &= ~OPOST;
    t->c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHONL|ICANON|ISIG|IEXTEN|NOFLSH|TOSTOP|PENDIN);
    t->c_cflag &= ~(CSIZE|PARENB);
    t->c_cflag |= CS8|CREAD;
    t->c_cc[VMIN] = 1;
    t->c_cc[VTIME] = 0;
}
