//  z_utils.c
//  SystemCall
//
//  Created by NULL
//

#include "z_types.h"
#include "z_error.h"
#include "z_stdio.h"
#include "z_string.h"

#define BigLittleSwap16(A)  ((((z_uint16)(A) & 0xff00) >> 8) | (((z_uint16)(A) & 0x00ff) << 8))
#define BigLittleSwap32(A)  ((((z_uint32)(A) & 0xff000000) >> 24) | (((z_uint32)(A) & 0x00ff0000) >> 8) | (((z_uint32)(A) & 0x0000ff00) << 8) | (((z_uint32)(A) & 0x000000ff) << 24))

z_int32 z_isdigit(z_uint8 c)
{
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

z_int32 z_isletter(z_uint8 c)
{
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}

z_int32 z_isspace(z_int32 ch)
{
    return (z_uint32)(ch - 9) < 5u || ch == ' ';
}

z_uint32 z_inet_aton(z_int8 *c_ipaddr)
{
    z_uint32 u_ipaddr = 0;
    z_uint32 u_tmp = 0;
    z_int8 c = 0;
    z_int32 i_base = 10;
    z_int32 i_shift = 0;
    z_int32 i_recycle = 0;

    c = *c_ipaddr;
    while (1)
    {
        u_tmp = 0;
        while (1)
        {
            if (z_isdigit(c))
            {
                u_tmp = (u_tmp * i_base) + (c - 0x30);
                c = *++c_ipaddr;
            }
            else
            {
                break;
            }
        }

        i_shift = 8 * i_recycle++;
        u_tmp <<= i_shift;
        u_ipaddr += u_tmp;

        if (c == '.')
        {
            c = *++c_ipaddr;
        }
        else
        {
            break;
        }
    }

    if (c != '\0' && (!z_isspace(c)))
    {
        goto ret_0;
    }

    return u_ipaddr;
ret_0:
    return (0);
}

z_int8 buffer[128];
z_int8 *z_inet_ntoa(z_uint32 in)
{
    z_uint8 *bytes = (z_uint8 *)&in;
    z_memset(buffer, 0, 128);
    z_sprintf(buffer, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return buffer;
}

z_int32 cpu_endian(z_void)
{
    union
    {
        z_uint32 i;
        z_uint8 a[4];
    } u;
    
    u.i = 1;
    if (u.a[0] == 1)
    {
        z_printf("little endian\n");
        return 0;
    }
    else
    {
        z_printf("big endian\n");
        return 1;
    }
}

z_uint64 z_htonl(z_uint64 h)
{
    return cpu_endian() ? h : BigLittleSwap32(h);
}

z_uint32 z_ntohl(z_uint32 n)
{
    return cpu_endian() ? n : BigLittleSwap32(n);
}

z_uint16 z_htons(z_uint16 h)
{
    return cpu_endian() ? h : BigLittleSwap16(h);
}

z_uint16 z_ntohs(z_uint16 n)
{
    return cpu_endian() ? n : BigLittleSwap16(n);
}

z_void z_itoa(z_uint32 n, z_int8 *buf)
{
    z_int32 i = 0;

    if (n < 10)
    {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    
    z_itoa(n / 10, buf);

    for (i = 0; buf[i] != '\0'; i++)
    {
        ;
    }

    buf[i] = (n % 10) + '0';
    buf[i + 1] = '\0';
}

z_int32 z_atoi(z_int8 *pstr)
{
    z_int32 int_ret = 0;
    z_int32 int_sign = 1;

    if (pstr == 0)
    {
        return -1;
    }
    
    while (((*pstr) == ' ') || ((*pstr) == '\n') || ((*pstr) == '\t') || ((*pstr) == '\b'))
    {
        pstr++;
    }

    if (*pstr == '-')
    {
        int_sign = -1;
    }
    
    if (*pstr == '-' || *pstr == '+')
    {
        pstr++;
    }

    while (*pstr >= '0' && *pstr <= '9') 
    {
        int_ret = int_ret * 10 + *pstr - '0';
        pstr++;
    }
    
    int_ret = int_sign * int_ret;
    return int_ret;
}

z_void z_xtoa(z_uint32 n, z_int8 *buf)
{
    z_int32 i = 0;

    if (n < 16)
    {
        if(n < 10)
        {
            buf[0] = n + '0';
        }
        else
        {
            buf[0] = n - 10 + 'a';
        }
        buf[1] = '\0';
        return;
    }
    
    z_xtoa(n / 16, buf);

    for (i = 0; buf[i] != '\0'; i++)
    {
        ;
    }

    if ((n % 16) < 10)
    {
        buf[i] = (n % 16) + '0';
    }
    else
    {
        buf[i] = (n % 16) - 10 + 'a';
    }
    
    buf[i + 1] = '\0';
}

z_void z_show_register(z_void)
{
    z_uint64 rax = 0;
    z_uint64 rbx = 0;
    z_uint64 rcx = 0;
    z_uint64 rdx = 0;
    
    asm ("movq %%rax, %0\n" : "=r"(rax));
    asm ("movq %%rbx, %0\n" : "=r"(rbx));
    asm ("movq %%rcx, %0\n" : "=r"(rcx));
    asm ("movq %%rdx, %0\n" : "=r"(rdx));
    
    z_printf("rax %lx, rbx %lx, rcx %lx, rdx %lx\n", rax, rbx, rcx, rdx);
}

