//
//  test_termios.c
//  Example
//
//  Created by NULL
//

#include "test_termios.h"

struct z_termios orig_termios;

z_int32 kbhit(z_void)
{
    struct z_timeval tv;
    struct z_fd_set fds;
    
    z_memset(&fds, 0, sizeof(fds));
    z_fd_set(0, &fds);
    
    return z_select(STDIN_FILENO + 1, &fds, Z_NULL, Z_NULL, &tv);
}

z_void set_conio_terminal_mode(z_void)
{
    struct z_termios new_termios;
    z_tcgetattr(STDIN_FILENO, &orig_termios);
    z_memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    new_termios.c_lflag &= ~(ICANON | ECHO);    // 关闭行缓冲
    z_tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

z_void reset_terminal_mode(z_void)
{
    z_tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

z_void test_termios(z_void)
{
    z_uint8 buf[128];
    z_uint8 i = 0;
//    z_uint8 ch = 0;
    
    set_conio_terminal_mode();
    
    while (1)
    {
        z_memset(buf, 0, 128);
        // read 本需要输入回车才能逐行返回，关闭行缓冲之后，可以实时返回用户输入
        z_read(STDIN_FILENO, buf, 128);

        for (i = 0; i < 128; i++)
        {
            if (buf[i] == '\n' || buf[i] == 0)
            {
                break;
            }
            z_printf("\n0x%x ", buf[i]);
        }
        z_printf("\n");
        
//        ch = getchar();
//        printf("\n0x%x\n", ch);
        
        if (buf[0] == 'q')
        {
            break;
        }
    }
    
    reset_terminal_mode();
    return;
}
