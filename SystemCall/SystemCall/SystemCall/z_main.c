//
//  ptrace.c
//  Example
//
//  Created by NULL 
//

#include "z_types.h"
#include "z_mach_trap.h"
#include "z_stdio.h"
#include "z_pthread.h"
#include "z_pthread_tlv.h"
#include "z_unistd.h"
#include "z_string.h"
#include "z_stdlib.h"
#include "z_mman.h"
#include "z_machO.h"
#include "z_atexit.h"
#include "z_mach.h"

extern z_int32 z_main(z_int32 argc, z_int8 *argv[], z_int8 *envp[], z_int8 *apple[]);

//z_int32 z_main(z_int32 argc, z_int8 *argv[], z_int8 *envp[], z_int8 *apple[])
//{
//    return 0;
//}

z_void z_init(z_int32 argc, z_int8 *argv[], z_int8 *envp[], z_int8 *apple[], struct z_mach_header_64 *mh)
{
    z_printf("enter z_init\n");
    z_get_processor_info();
    z_mach_init();
    z_pthread_set_self();
    z_pthread_init(envp, apple);
    z_pthread_tlv_initializer(mh);
    z_atexit_init();
}

z_void z_fini(z_void)
{
    z_printf("enter z_fini\n");
}

//extern void *__dso_handle;

z_int8 *z_simple_getenv(z_int8 *envp[], z_int8 *var)
{
    z_int8 **p;
    z_int32 var_len;

    var_len = z_strlen(var);
    for (p = envp; p && *p; p++)
    {
        z_int32 p_len = z_strlen(*p);
        if (p_len >= var_len
            && z_memcmp(*p, var, var_len) == 0
            && (*p)[var_len] == '=')
        {
            return &(*p)[var_len + 1];
        }
    }

    return Z_NULL;
}

z_uint64 zz_main(struct z_mach_header_64 *app_header, z_int32 argc, z_int8 *argv[], struct z_mach_header_64 *dyld_header, z_uint64 *startGlue)
{
//    struct z_mach_header_64 *header = &__dso_handle;
    z_int8 **p = Z_NULL;
    
    // kernel sets up env pointer to be just past end of agv array
    z_int8 **envp = argv + argc + 1;
    for (p = envp; *p != Z_NULL; ++p)
    {
        z_printf("%s\n", *p);
    }
    
    // kernel sets up apple pointer to be just past end of envp array
    z_int8 **apple = p + 1;
    for (p = apple; *p != Z_NULL; ++p)
    {
        z_printf("%s\n", *p);
    }

    z_int32 ret = 0;
    z_init(argc, argv, envp, apple, dyld_header);
    ret = z_main(argc, argv, envp, apple);
    z_fini();
    z_exit(ret);
    return 0;
}

/*
 The actual entry of the program is specified by the linker.
 This function obtains the startup parameters from the kernel
 and jumps to main after the environment variables.
*/
void z_start(z_void)
{
    asm (
         "popq      %rdi;"
         "pushq     $0;"
         "movq      %rsp, %rbp;"
         "andq      $-16, %rsp;"
         "subq      $16,  %rsp;"
         "movl      8(%rbp), %esi;"
         "leaq      16(%rbp), %rdx;"
         "leaq      ___dso_handle(%rip), %rcx;"
         "leaq      -8(%rbp), %r8;"
         "call      _zz_main;"
    );
}



