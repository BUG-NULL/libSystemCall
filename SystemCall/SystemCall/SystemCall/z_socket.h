//
//  z_socket.h
//  SystemCall
//
//  Created by NULL
//

#ifndef z_socket_h
#define z_socket_h

#include <SystemCall/z_types.h>

#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
/*
 * Types
 */
#define SOCK_STREAM     1               /* stream socket */
#define SOCK_DGRAM      2               /* datagram socket */
#define SOCK_RAW        3               /* raw-protocol interface */
#if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
#define SOCK_RDM        4               /* reliably-delivered message */
#endif  /* (!_POSIX_C_SOURCE || _DARWIN_C_SOURCE) */
#define SOCK_SEQPACKET  5               /* sequenced packet stream */

/*
 * howto arguments for shutdown(2), specified by Posix.1g.
 */
#define SHUT_RD         0               /* shut down the reading side */
#define SHUT_WR         1               /* shut down the writing side */
#define SHUT_RDWR       2               /* shut down both sides */

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define SOL_SOCKET      0xffff          /* options for socket level */

/*
 * Option flags per-socket.
 */
#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER       0x0080          /* linger on close if data present (in ticks) */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */
#define SO_REUSEPORT    0x0200          /* allow local address & port reuse */
#define SO_TIMESTAMP    0x0400          /* timestamp received dgram traffic */
#define SO_TIMESTAMP_MONOTONIC  0x0800  /* Monotonically increasing timestamp on rcvd dgram */
#define SO_DONTTRUNC    0x2000          /* APPLE: Retain unread data */
#define SO_WANTMORE     0x4000          /* APPLE: Give hint when more data ready */
#define SO_WANTOOBFLAG  0x8000          /* APPLE: Want OOB in MSG_FLAG on receive */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF       0x1001          /* send buffer size */
#define SO_RCVBUF       0x1002          /* receive buffer size */
#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */
#define SO_ERROR        0x1007          /* get error status and clear */
#define SO_TYPE         0x1008          /* get socket type */
#define SO_LABEL        0x1010          /* deprecated */
#define SO_PEERLABEL    0x1011          /* deprecated */
#define SO_NREAD        0x1020          /* APPLE: get 1st-packet byte count */
#define SO_NKE          0x1021          /* APPLE: Install socket-level NKE */
#define SO_NOSIGPIPE    0x1022          /* APPLE: No SIGPIPE on EPIPE */
#define SO_NOADDRERR    0x1023          /* APPLE: Returns EADDRNOTAVAIL when src is not available anymore */
#define SO_NWRITE       0x1024          /* APPLE: Get number of bytes currently in send socket buffer */
#define SO_REUSESHAREUID    0x1025          /* APPLE: Allow reuse of port/socket by different userids */
#define SO_NOTIFYCONFLICT   0x1026  /* APPLE: send notification if there is a bind on a port which is already in use */
#define SO_UPCALLCLOSEWAIT  0x1027  /* APPLE: block on close until an upcall returns */
#define SO_LINGER_SEC       0x1080          /* linger on close if data present (in seconds) */
#define SO_RANDOMPORT       0x1082  /* APPLE: request local port randomization */
#define SO_NP_EXTENSIONS        0x1083  /* To turn off some POSIX behavior */
#define SO_NUMRCVPKT            0x1112  /* number of datagrams in receive socket buffer */
#define SO_NET_SERVICE_TYPE     0x1116  /* Network service type */
#define SO_NETSVC_MARKING_LEVEL 0x1119  /* Get QoS marking in effect for socket */

struct z_sockaddr
{
    z_uint8     sa_len;         /* total length */
    z_uint8     sa_family;      /* [XSI] address family */
    z_int8      sa_data[14];    /* [XSI] addr value (actually larger) */
};

struct z_in_addr
{
    z_uint32 s_addr;
};

struct z_sockaddr_in
{
    z_uint8             sin_len;
    z_uint8             sin_family;
    z_uint16            sin_port;
    struct z_in_addr    sin_addr;
    z_int8              sin_zero[8];
};

struct z_iovec
{
    z_void      *iov_base;      /* [XSI] Base address of I/O memory region */
    z_int32     iov_len;        /* [XSI] Size of region iov_base points to */
};

struct z_msghdr
{
    z_void          *msg_name;      /* [XSI] optional address */
    z_uint32        msg_namelen;    /* [XSI] size of address */
    struct z_iovec  *msg_iov;       /* [XSI] scatter/gather array */
    z_int32         msg_iovlen;     /* [XSI] # elements in msg_iov */
    z_void          *msg_control;   /* [XSI] ancillary data, see below */
    z_uint32        msg_controllen; /* [XSI] ancillary data buffer len */
    z_int32         msg_flags;      /* [XSI] flags on received message */
};

z_int32 z_socket(z_int32 domain, z_int32 type, z_int32 protocol);
z_int32 z_socketpair(z_int32 domain, z_int32 type, z_int32 protocol, z_int32 socket_vector[2]);
z_int32 z_connect(z_int32 socket, struct z_sockaddr *address, z_uint32 addrlen);
z_int32 z_accept(z_int32 socket, struct z_sockaddr *address, z_uint32 *addrlen);
z_int32 z_bind(z_int32 socket, struct z_sockaddr *address, z_uint32 addrlen);
z_int32 z_listen(z_int32 socket, z_int32 backlog);
z_int32 z_send(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags);
z_int32 z_sendmsg(z_int32 socket, struct z_msghdr *message, z_int32 flags);
z_int32 z_sendto(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags, struct z_sockaddr *dest_addr, z_uint32 dest_len);
z_int32 z_recv(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags);
z_int32 z_recvfrom(z_int32 socket, z_void *buffer, z_int32 length, z_int32 flags, struct z_sockaddr *address, z_uint32 *address_len);
z_int32 z_recvmsg(z_int32 socket, struct z_msghdr *message, z_int32 flags);
z_int32 z_getsockopt(z_int32 socket, z_int32 level, z_int32 option_name, z_void *option_value, z_uint32 *option_len);
z_int32 z_setsockopt(z_int32 socket, z_int32 level, z_int32 option_name, z_void *option_value, z_uint32 option_len);
z_int32 z_shutdown(z_int32 socket, z_int32 how);
z_int32 z_getpeername(z_int32 socket, struct z_sockaddr *address, z_uint32 *address_len);
z_int32 z_getsockname(z_int32 socket, struct z_sockaddr *address, z_uint32 *address_len);
    
#endif /* z_socket_h */
