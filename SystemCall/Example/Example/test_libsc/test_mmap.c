//
//  test_mmap.c
//  Example
//
//  Created by NULL
//

#include "test_mmap.h"

z_int32 test_mmap_2(z_void)
{
    z_int32 N = 5;

    z_int32 *ptr = z_mmap(Z_NULL, N * sizeof(z_int32), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (ptr == MAP_FAILED)
    {
        z_printf("Mapping Failed\n");
        return -1;
    }

    for (z_int32 i = 0; i < N; i++)
    {
        ptr[i] = i + 1;
    }

    z_printf("Initial values of the array elements :\n");
    for (z_int32 i = 0; i < N; i++ )
    {
        z_printf(" %d", ptr[i]);
    }
    z_printf("\n");

    z_int32 child_pid = z_fork();
    if (child_pid == 0)
    {
        // child
        for (z_int32 i = 0; i < N; i++)
        {
            ptr[i] = ptr[i] * 10;
        }
    }
    else
    {
        // parent
        z_wait4(child_pid, Z_NULL, 0, Z_NULL);
        z_printf("\nParent:\n");

        z_printf("Updated values of the array elements :\n");
        for (z_int32 i = 0; i < N; i++)
        {
            z_printf(" %d", ptr[i]);
        }
        z_printf("\n");
    }

    z_int32 err = z_munmap(ptr, N * sizeof(z_int32));
    if (err != 0)
    {
        z_printf("UnMapping Failed\n");
        return -1;
    }
    return 0;
}

z_int32 test_mmap_1(z_void)
{
    z_int32 fd = 0;
    z_int8 *z_buf = 0;
    z_int8 *buf = 0;
    struct z_stat sb;
    z_int8 *fname = "/tmp/in.txt";
    buf = buf;

    fd = z_open(fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        z_printf("open\n");
        return 1;
    }

    z_write(fd, "Hello, 12345\n", z_strlen("Hello, 12345"));

    if (z_fstat(fd, &sb) == -1)
    {
        z_printf("fstat\n");
        return 1;
    }

    z_buf = z_mmap(Z_NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_UNIX03, fd, 0);
    if (z_buf == MAP_FAILED)
    {
        z_printf("mmap\n");
        return 1;
    }

    z_printf("%s", z_buf);

    if (z_close(fd) == -1)
    {
        z_printf("close\n");
        return 1;
    }

    if (z_munmap(z_buf, sb.st_size) == -1)
    {
        z_printf("munmap\n");
        return 1;
    }

    return 0;
}

z_void test_mmap(z_void)
{
    test_mmap_1();
    test_mmap_2();
}
