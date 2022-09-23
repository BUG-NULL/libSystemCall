//
//  z_termios.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_termios_h
#define z_termios_h

#include <SystemCall/z_types.h>
#include <SystemCall/z_ioctl.h>

#define VEOF            0       /* ICANON */
#define VEOL            1       /* ICANON */
#define VEOL2           2       /* ICANON together with IEXTEN */
#define VERASE          3       /* ICANON */
#define VWERASE         4       /* ICANON together with IEXTEN */
#define VKILL           5       /* ICANON */
#define VREPRINT        6       /* ICANON together with IEXTEN */
/*            7       spare 1 */
#define VINTR           8       /* ISIG */
#define VQUIT           9       /* ISIG */
#define VSUSP           10      /* ISIG */
#define VDSUSP          11      /* ISIG together with IEXTEN */
#define VSTART          12      /* IXON, IXOFF */
#define VSTOP           13      /* IXON, IXOFF */
#define VLNEXT          14      /* IEXTEN */
#define VDISCARD        15      /* IEXTEN */
#define VMIN            16      /* !ICANON */
#define VTIME           17      /* !ICANON */
#define VSTATUS         18      /* ICANON together with IEXTEN */
/*            19       spare 2 */

#define NCCS            20
#define TCSANOW         0               /* make change immediate */
#define TCSADRAIN       1               /* drain output, then change */
#define TCSAFLUSH       2               /* drain output, flush input */
#define TCSASOFT        0x10            /* flag - don't alter h.w. state */

#define ECHOKE          0x00000001      /* visual erase for line kill */
#define ECHOE           0x00000002      /* visually erase chars */
#define ECHOK           0x00000004      /* echo NL after line kill */
#define ECHO            0x00000008      /* enable echoing */
#define ECHONL          0x00000010      /* echo NL even if ECHO is off */
#define ECHOPRT         0x00000020      /* visual erase mode for hardcopy */
#define ECHOCTL         0x00000040      /* echo control chars as ^(Char) */
#define ISIG            0x00000080      /* enable signals INTR, QUIT, [D]SUSP */
#define ICANON          0x00000100      /* canonicalize input lines */
#define ALTWERASE       0x00000200      /* use alternate WERASE algorithm */
#define IEXTEN          0x00000400      /* enable DISCARD and LNEXT */
#define EXTPROC         0x00000800      /* external processing */
#define TOSTOP          0x00400000      /* stop background jobs from output */
#define FLUSHO          0x00800000      /* output being flushed (state) */
#define NOKERNINFO      0x02000000      /* no kernel output from VSTATUS */
#define PENDIN          0x20000000      /* XXX retype pending input (state) */
#define NOFLSH          0x80000000      /* don't flush after interrupt */

#define CIGNORE         0x00000001      /* ignore control flags */
#define CSIZE           0x00000300      /* character size mask */
#define CS5             0x00000000      /* 5 bits (pseudo) */
#define CS6             0x00000100      /* 6 bits */
#define CS7             0x00000200      /* 7 bits */
#define CS8             0x00000300      /* 8 bits */
#define CSTOPB          0x00000400      /* send 2 stop bits */
#define CREAD           0x00000800      /* enable receiver */
#define PARENB          0x00001000      /* parity enable */
#define PARODD          0x00002000      /* odd parity, else even */
#define HUPCL           0x00004000      /* hang up on last close */
#define CLOCAL          0x00008000      /* ignore modem status lines */
#define CCTS_OFLOW      0x00010000      /* CTS flow control of output */
#define CRTSCTS         (CCTS_OFLOW | CRTS_IFLOW)
#define CRTS_IFLOW      0x00020000      /* RTS flow control of input */
#define CDTR_IFLOW      0x00040000      /* DTR flow control of input */
#define CDSR_OFLOW      0x00080000      /* DSR flow control of output */
#define CCAR_OFLOW      0x00100000      /* DCD flow control of output */
#define MDMBUF          0x00100000      /* old name for CCAR_OFLOW */

#define IGNBRK          0x00000001      /* ignore BREAK condition */
#define BRKINT          0x00000002      /* map BREAK to SIGINTR */
#define IGNPAR          0x00000004      /* ignore (discard) parity errors */
#define PARMRK          0x00000008      /* mark parity and framing errors */
#define INPCK           0x00000010      /* enable checking of parity errors */
#define ISTRIP          0x00000020      /* strip 8th bit off chars */
#define INLCR           0x00000040      /* map NL into CR */
#define IGNCR           0x00000080      /* ignore CR */
#define ICRNL           0x00000100      /* map CR to NL (ala CRMOD) */
#define IXON            0x00000200      /* enable output flow control */
#define IXOFF           0x00000400      /* enable input flow control */
#define IXANY           0x00000800      /* any char will restart after stop */
#define IMAXBEL         0x00002000      /* ring bell on input queue full */
#define IUTF8           0x00004000      /* maintain state for UTF-8 VERASE */

#define OPOST           0x00000001      /* enable following output processing */
#define ONLCR           0x00000002      /* map NL to CR-NL (ala CRMOD) */
#define OXTABS          0x00000004      /* expand tabs to spaces */
#define ONOEOT          0x00000008      /* discard EOT's (^D) on output) */
 
//#define FIODTYPE        _IOR('f', 122, z_int32)     /* get d_type */
#define FIODTYPE (0x40000000 | ((sizeof(z_int32) & 0x1fff) << 16) | (('f') << 8) | (122))
#define TIOCGETA        _IOR('t', 19, struct z_termios)     /* get termios struct */
#define TIOCSETA        _IOW('t', 20, struct z_termios)     /* set termios struct */
#define TIOCSETAW       _IOW('t', 21, struct z_termios)     /* drain output, set */
#define TIOCSETAF       _IOW('t', 22, struct z_termios)     /* drn out, fls in, set */
#define TIOCGWINSZ      _IOR('t', 104, struct z_winsize)    /* get window size */
#define TIOCSWINSZ      _IOW('t', 103, struct z_winsize)    /* set window size */
#define TIOCPTYGRANT    _IO('t', 84)                        /* grantpt(3) */
#define TIOCPTYGNAME    _IOC(IOC_OUT, 't', 83, 128)         /* ptsname(3) */
#define TIOCPTYUNLK     _IO('t', 82)                        /* unlockpt(3) */
#define TIOCGPGRP       _IOR('t', 119, int)                 /* get pgrp of tty */
#define TIOCSPGRP       _IOW('t', 118, int)                 /* set pgrp of tty */

typedef z_uint64    tcflag_t;
typedef z_uint8     cc_t;
typedef z_uint64    speed_t;

struct z_termios
{
    tcflag_t    c_iflag;        /* input flags */
    tcflag_t    c_oflag;        /* output flags */
    tcflag_t    c_cflag;        /* control flags */
    tcflag_t    c_lflag;        /* local flags */
    cc_t        c_cc[NCCS];     /* control chars */
    speed_t     c_ispeed;       /* input speed */
    speed_t     c_ospeed;       /* output speed */
};

struct z_winsize
{
    z_uint16    ws_row;         /* rows, in characters */
    z_uint16    ws_col;         /* columns, in characters */
    z_uint16    ws_xpixel;      /* horizontal size, pixels */
    z_uint16    ws_ypixel;      /* vertical size, pixels */
};

z_int32 z_tcgetattr(z_int32 fd, struct z_termios *t);
z_int32 z_tcsetattr(z_int32 fd, z_int32 opt, struct z_termios *t);
z_int32 z_tcsetpgrp(z_int32 fd, z_int32 pgrp);
z_int32 z_tcgetpgrp(z_int32 fd);
z_void z_cfmakeraw(struct z_termios *t);

#endif /* z_termios_h */
