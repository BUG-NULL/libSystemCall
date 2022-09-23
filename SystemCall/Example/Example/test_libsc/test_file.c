//
//  test_file.c
//  Example
//
//  Created by NULL
//

#include "test_file.h"

z_void test_file(z_void)
{
    z_int32 in = 0;
    z_int32 out = 0;
    z_int32 flag = 0;
    // = {0} 这个写法，编译器会在编译期间自动加上 memset 来清零
    z_int8 buffer[1024];
    z_int8 *input = "/tmp/in.txt";
    z_int8 *output = "/tmp/out.txt";

    in = z_open(input, O_RDONLY, 0);
    if (in < 3)
    {
        z_printf("open %s failed\n", input);
        return;
    }

    out = z_open(output, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (out < 3)
    {
        z_printf("open %s failed\n", output );
        return;
    }

    z_memset(buffer, 0, 1024);
    while ((flag = z_read(in, buffer, 1024)) > 0)
    {
        z_printf("%s", buffer);
        z_write(out, buffer, flag);
    }
    z_close(in);
    z_close(out);
}
