//
//  test_sem.c
//  Example
//
//  Created by NULL
//

#include "test_sem.h"

z_void test_sem(z_void)
{
    z_int32 ret = 0;
    z_int8 *name = "sem";

    // sem_unlink 的作用是删除已存在的信号量，注意有名信号量在程序结束后可能依然存在
    // 所以需要调用 sem_unlink 手动删除
    ret = z_sem_unlink(name);
    z_sem_t *sem = z_sem_open(name, O_CREAT, 0, 1);
    if (sem)
    {
        ret = z_sem_trywait(sem);
//        ret = z_sem_wait(sem);
        if (ret)
        {
            ret = z_sem_close(sem);
        }
    }
}
