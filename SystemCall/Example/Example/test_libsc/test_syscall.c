//
//  test_syscall.c
//  Example
//
//  Created by NULL
//

#if 0

#include "test_syscall.h"
#include <stdio.h>
#include <unistd.h>
#include <SystemCall/SystemCall.h>

/*
 字符编码（UTF-8）
 http://www.mytju.com/classcode/tools/encode_utf8.asp
 
 字符    编码 10 进制    编码 16 进制     Unicode 编码 10 进制    Unicode 编码 16 进制
 你      14990752      E4BDA0          20320                 4F60
 好      15050173      E5A5BD          22909                 597D
 
 "hello, world! 你好\n"
 Xcode 汇编是八进制    "hello, world! \344\275\240\345\245\275\n"
 Mach-O 编译后的二进制 68 65 6C 6C 6F 2C 20 77 6F 72 6C 64 21 20 E4 BD A0 E5 A5 BD 0A
*/
z_int8 *strval = "hello, world! 你好\n";
z_int32 size = 22;

z_int32 stage_1(z_void)
{
    return (z_int32)printf("%s", strval);
}

z_int32 stage_2(z_void)
{
    return (z_int32)write(STDOUT_FILENO, strval, size);
}

z_int32 stage_3(z_void)
{
    return (z_int32)syscall(SYS_write, STDOUT_FILENO, strval, size);
}

z_int32 stage_4(z_void)
{
    return (z_int32)z_syscall(SYSCALL_UNIX(SYS_write), STDOUT_FILENO, strval, size);
}

z_int32 stage_5(z_void)
{
    return (z_int32)zz_syscall(SYSCALL_UNIX(SYS_write), STDOUT_FILENO, (z_int64)strval, size, 0, 0, 0);
}

z_void test_syscall(z_void)
{
//    stage_1();
//    stage_2();
//    stage_3();
    stage_4();
    stage_5();
}

#endif

