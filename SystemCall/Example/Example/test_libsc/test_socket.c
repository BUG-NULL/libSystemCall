//
//  test_socket.c
//  Example
//
//  Created by NULL
//

#include "test_socket.h"

#define MAX_LISTEN_NUM      5
#define SEND_BUF_SIZE       128
#define RECV_BUF_SIZE       1024
#define SERVER_PORT         80

z_void test_socket_server(z_void)
{
    z_int32 ret = 0;
    z_int32 listen_sock = 0;
    z_int32 app_sock = 0;
    struct z_sockaddr_in hostaddr;
    struct z_sockaddr_in clientaddr;
    z_uint32 socklen = sizeof(clientaddr);
    z_int8 sendbuf[SEND_BUF_SIZE];
    z_int8 recvbuf[RECV_BUF_SIZE];
    z_int32 sendlen = 0;
    z_int32 recvlen = 0;
    z_int32 retlen = 0;
    z_int32 leftlen = 0;
    z_int8 *ptr = Z_NULL;
    z_int8 exit = 0;

    z_memset(sendbuf, 0, SEND_BUF_SIZE);
    z_memset(recvbuf, 0, RECV_BUF_SIZE);
    z_memset((z_void *)&hostaddr, 0, sizeof(hostaddr));
    z_memset((z_void *)&clientaddr, 0, sizeof(clientaddr));
    hostaddr.sin_family = AF_INET;
    hostaddr.sin_port = z_htons(9090);
    hostaddr.sin_addr.s_addr = 0;
    listen_sock = z_socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        z_printf("create socket failed\n");
        return;
    }

    z_int32 reuse = 1;
    ret = z_setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ret = z_setsockopt(listen_sock, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    ret = z_bind(listen_sock, (struct z_sockaddr *)&hostaddr, sizeof(hostaddr));
    if (ret < 0)
    {
        z_int64 err = z_errno;
        if (err == EADDRINUSE)
        {

        }
        z_printf("bind socket failed\n");
        return;
    }

    ret = z_listen(listen_sock, MAX_LISTEN_NUM);
    if (ret < 0)
    {
        z_printf("listen failed\n");
        return;
    }

    z_getsockname(listen_sock, (struct z_sockaddr *)&hostaddr, &socklen);
    z_printf("listen on %s port %d\n", z_inet_ntoa(hostaddr.sin_addr.s_addr), z_ntohs(hostaddr.sin_port));
    while (!exit)
    {
        app_sock = z_accept(listen_sock, (struct z_sockaddr *)&clientaddr, &socklen);
        if (app_sock < 0)
        {
            z_printf("accept failed\n");
            break;
        }

        z_getpeername(app_sock, (struct z_sockaddr *)&clientaddr, &socklen);
        z_printf("client address %s port %d\n", z_inet_ntoa(clientaddr.sin_addr.s_addr), z_ntohs(clientaddr.sin_port));
        z_sprintf(sendbuf, "welcome %s:%d here!\n", z_inet_ntoa(clientaddr.sin_addr.s_addr), clientaddr.sin_port);
        // send data
        sendlen = z_strlen(sendbuf) + 1;
        retlen = 0;
        leftlen = sendlen;
        ptr = sendbuf;
        while (leftlen)
        {
            retlen = z_send(app_sock, ptr, sendlen, 0);
            if (retlen < 0)
            {
                if (z_errno == EINTR)
                    retlen = 0;
                else
                    break;
            }
            leftlen -= retlen;
            ptr += retlen;
        }

        // receive data
        recvlen = 0;
        retlen = 0;
        ptr = recvbuf;
        leftlen = RECV_BUF_SIZE -1;
        do
        {
            retlen = z_recv(app_sock, ptr, leftlen, 0);
            if (retlen < 0)
            {
                if (z_errno == EINTR)
                    retlen = 0;
                else
                    break;
            }

            z_memset(sendbuf, 0, SEND_BUF_SIZE);
            z_memcpy(sendbuf, ptr, retlen);
            if (z_strncmp(sendbuf, "end", 3) == 0)
            {
                exit = 1;
                break;
            }
            z_send(app_sock, sendbuf, retlen, 0);
            recvlen += retlen;
            leftlen -= retlen;
            ptr += retlen;
        } while (recvlen && leftlen);

        z_printf("receive data is : \n", recvbuf);
        z_printf("%s\n", recvbuf);
        z_close(app_sock);
    }

    z_close(listen_sock);
    return;
}

z_void test_socket_client(z_void)
{
    z_int32 ret = 0;
    z_int32 sock_fd = 0;
    z_int8 recvbuf[RECV_BUF_SIZE];
    z_int8 sendbuf[SEND_BUF_SIZE];
    z_int32 recvlen = 0;
    z_int32 retlen = 0;
    z_int32 sendlen = 0;
    z_int32 leftlen = 0;
    z_int8 *ptr = Z_NULL;
    struct z_sockaddr_in ser_addr;
    struct z_sockaddr_in local_addr;
    z_uint32 addrlen = sizeof(local_addr);

    z_memset(sendbuf, 0, SEND_BUF_SIZE);
    z_memset(recvbuf, 0, RECV_BUF_SIZE);
    z_memset(&ser_addr, 0, addrlen);
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = z_inet_aton("123.207.6.123");
    ser_addr.sin_port = z_htons(80);
    sock_fd = z_socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        z_printf("create socket failed\n");
        return;
    }

    z_int32 flags = z_fcntl(sock_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ret = z_fcntl(sock_fd, F_SETFL, flags);

    ret = z_connect(sock_fd, (struct z_sockaddr *)&ser_addr, addrlen);
    if (ret < 0)
    {
        z_int64 err = z_errno;
        if (err != EINPROGRESS)
        {
            z_printf("connect socket failed\n");
            return;
        }
    }

    struct z_fd_set readfds;
    struct z_fd_set writefds;
    struct z_timeval timeout;

    z_memset(&readfds, 0, sizeof(readfds));
    z_memset(&writefds, 0, sizeof(readfds));
    z_fd_set(sock_fd, &readfds);
    z_fd_set(sock_fd, &writefds);

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    ret = z_select(sock_fd + 1, Z_NULL, &writefds, Z_NULL, &timeout);
    if (ret <= 0)
    {
        z_printf("connection time out\n");
        z_close(sock_fd);
        return;
    }

    if (!z_fd_isset(sock_fd, &writefds))
    {
        z_printf("no events on sockfd foundn");
        z_close(sock_fd);
        return;
    }

    z_int32 error = 0;
    z_uint32 length = sizeof(error);
    // 调用 getsockopt 来获取并清除 sockfd 上的错误
    if (z_getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error, &length) < 0)
    {
        z_printf("get socket option failedn");
        z_close(sock_fd);
        return;
    }

    if (error != 0)
    {
        z_printf("connection failed after select with the error: %sn", z_strerror(error));
        z_close(sock_fd);
        return;
    }

    z_getsockname(sock_fd, (struct z_sockaddr *)&local_addr, &addrlen);
    z_printf("client address %s port %d\n", z_inet_ntoa(local_addr.sin_addr.s_addr), z_ntohs(local_addr.sin_port));

    // send data
    z_int8 *sendstr = "hello server\n";
    z_memcpy(sendbuf, sendstr, z_strlen(sendstr));
    sendlen = z_strlen(sendbuf) + 1;
    retlen = 0;
    leftlen = sendlen;
    ptr = sendbuf;
    while (leftlen > 0)
    {
        retlen = z_send(sock_fd, ptr, sendlen, 0);
        if (retlen < 0)
        {
            if (z_errno == EINTR)
                retlen = 0;
            else
                return;
        }
        leftlen -= retlen;
        ptr += retlen;
    }

    // receive data
    ret = z_select(sock_fd + 1, &readfds, Z_NULL, Z_NULL, &timeout);
    if (ret <= 0)
    {
        z_printf("connection time out\n");
        z_close(sock_fd);
        return;
    }

    if (!z_fd_isset(sock_fd, &readfds))
    {
        z_printf("no events on sockfd foundn");
        z_close(sock_fd);
        return;
    }

    recvlen = 0;
    retlen = 0;
    ptr = recvbuf;
    leftlen = RECV_BUF_SIZE -1;
    do
    {
        retlen = z_recv(sock_fd, ptr, leftlen, 0);
        if (retlen < 0)
        {
            if (z_errno == EINTR)
                retlen = 0;
            else
                return;
        }
        else if (retlen == 0)
        {
            break;
        }
        recvlen += retlen;
        leftlen -= retlen;
        ptr += retlen;
    } while (recvlen && leftlen);

    z_printf("receive data is : \n");
    z_printf("%s\n", recvbuf);
    z_shutdown(sock_fd, SHUT_WR);
}

z_void test_socket(z_void)
{
    test_socket_client();
    test_socket_server();
}
