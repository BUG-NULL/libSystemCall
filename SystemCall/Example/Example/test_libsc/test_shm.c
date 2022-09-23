//
//  test_shm.c
//  Example
//
//  Created by NULL
//

#include "test_shm.h"

#define TEXT_SZ             2048
struct shared_use_st
{
    z_int32     written;        // 作为一个标志，非0：表示可读，0：表示可写
    z_int8      text[TEXT_SZ];  // 记录写入 和 读取 的文本
};

z_void test_shm_read(z_int32 key)
{
    z_void  *shm = 0;
    struct shared_use_st *shared = Z_NULL; // 指向shm
    z_int32 shmid = 0; // 共享内存标识符

    // 创建共享内存
    shmid = z_shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        z_printf("shmat failed\n");
        return;
    }

    // 将共享内存连接到当前进程的地址空间
    shm = z_shmat(shmid, 0, 0);
    if (shm == (z_void *)-1)
    {
        z_printf("shmat failed\n");
        return;
    }

    z_printf("Memory attached at %p \n", shm);

    // 设置共享内存
    shared = (struct shared_use_st *)shm;
    // 注意：shm有点类似通过 malloc() 获取到的内存，所以这里需要做个 类型强制转换
    shared->written = 0;
    while (1) // 读取共享内存中的数据
    {
        // 没有进程向内存写数据，有数据可读取
        if (shared->written == 1)
        {
            z_printf("You write: %s\n", shared->text);
            z_sleep(1);

            // 读取完数据，设置written使共享内存段可写
            shared->written = 0;

            // 输入了 end，退出循环（程序）
            if (z_strncmp(shared->text, "end", 3) == 0)
            {
                break;
            }
        }
        else // 有其他进程在写数据，不能读取数据
        {
            z_sleep(1);
        }
    }

    // 把共享内存从当前进程中分离
    if (z_shmdt(shm) == -1)
    {
        z_printf("shmdt failed\n");
        return;
    }

    // 删除共享内存
    if (z_shmctl(shmid, IPC_RMID, 0) == -1)
    {
        z_printf("shmctl(IPC_RMID) failed\n");
        return;
    }
}

z_void test_shm_write(z_int32 key)
{
    z_void  *shm = 0;
    struct shared_use_st *shared = 0;
    z_int8 buffer[TEXT_SZ + 1];
    z_int32 shmid = 0;
    z_int32 ret = 0;

    z_memset(buffer, 0, TEXT_SZ + 1);
    // 创建共享内存
    shmid = z_shmget(key, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        z_printf("shmget failed\n");
        return;
    }

    // 将共享内存连接到当前的进程地址空间
    shm = z_shmat(shmid, (z_void *)0, 0);
    if (shm == (z_void *)-1)
    {
        z_printf("shmat failed\n");
        return;
    }

    z_printf("Memory attched at %p\n", shm);

    // 设置共享内存
    shared = (struct shared_use_st *)shm;
    while (1) // 向共享内存中写数据
    {
        // 数据还没有被读取，则等待数据被读取，不能向共享内存中写入文本
        while (shared->written == 1)
        {
            z_sleep(1);
            z_printf("Waiting...\n");
        }

        // 向共享内存中写入数据
        z_printf("Enter some text: \n");
        ret = z_read(STDIN_FILENO, buffer, 32);
        z_memcpy(buffer, buffer, z_strlen(buffer));
        z_strncpy(shared->text, buffer, TEXT_SZ);

        // 写完数据，设置written使共享内存段可读
        shared->written = 1;

        // 输入了end，退出循环（程序）
        if (z_strncmp(buffer, "end", 3) == 0)
        {
            break;
        }
    }

    // 把共享内存从当前进程中分离
    if (z_shmdt(shm) == -1)
    {
        z_printf("shmdt failed\n");
        return;
    }

    z_sleep(2);
}

z_void test_shm(z_void)
{
    z_int32 pid = 0;
    z_int32 key = 1234;

    pid = z_fork();
    if (pid == 0)
    {
        z_printf("I'm child gaga .......\n");
        test_shm_read(key);
        z_printf("Child exit .......\n");
    }
    else
    {
        z_printf("I am father .......\n");
        test_shm_write(key);
        z_printf("Father exit .......\n");
    }
}
