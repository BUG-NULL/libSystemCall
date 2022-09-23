//
//  z_socket.c
//  SystemCall
//
//  Created by NULL
//

#include "z_socket.h"
#include "z_syscall.h"
#include "z_error.h"

z_int32 z_socket(z_int32 domain, z_int32 type, z_int32 protocol)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_socket), domain, type, protocol));
}

z_int32 z_socketpair(z_int32 domain, z_int32 type, z_int32 protocol, z_int32 socket_vector[2])
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_socketpair), domain, type, protocol, socket_vector));
}

z_int32 z_connect(z_int32 socket, struct z_sockaddr *address, z_uint32 addrlen)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_connect), socket, address, addrlen));
}

z_int32 z_accept(z_int32 socket, struct z_sockaddr *address, z_uint32 *addrlen)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_accept), socket, address, addrlen));
}

z_int32 z_bind(z_int32 socket, struct z_sockaddr *address, z_uint32 addrlen)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_bind), socket, address, addrlen));
}

z_int32 z_listen(z_int32 socket, z_int32 backlog)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_listen), socket, backlog));
}

z_int32 z_send(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags)
{
    return z_sendto(socket, buffer, length, flags, 0, 0);
}

z_int32 z_sendmsg(z_int32 socket, struct z_msghdr *message, z_int32 flags)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sendmsg), socket, message, flags));
}

z_int32 z_sendto(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags, struct z_sockaddr *dest_addr, z_uint32 dest_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_sendto), socket, buffer, length, flags, dest_addr, dest_len));
}

z_int32 z_recv(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags)
{
    return z_recvfrom(socket, buffer, length, flags, 0, 0);
}

z_int32 z_recvfrom(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags, struct z_sockaddr *address, z_uint32 *address_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_recvfrom), socket, buffer, length, flags, address, address_len));
}

z_int32 z_recvmsg(z_int32 socket, struct z_msghdr *message, z_int32 flags)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_recvmsg), socket, message, flags));
}

z_int32 z_getsockopt(z_int32 socket, z_int32 level, z_int32 option_name, z_void *option_value, z_uint32 *option_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getsockopt), socket, level, option_name, option_value, option_len));
}

z_int32 z_setsockopt(z_int32 socket, z_int32 level, z_int32 option_name, z_void *option_value, z_uint32 option_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_setsockopt), socket, level, option_name, option_value, option_len));
}

z_int32 z_shutdown(z_int32 socket, z_int32 how)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_shutdown), socket, how));
}

z_int32 z_getpeername(z_int32 socket, struct z_sockaddr *address, z_uint32 *address_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getpeername), socket, address, address_len));
}

z_int32 z_getsockname(z_int32 socket, struct z_sockaddr *address, z_uint32 *address_len)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getsockname), socket, address, address_len));
}



